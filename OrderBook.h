#include <deque>
#include <string>
#include <iostream>
#include <cstdlib> // For std::exit
#include <fstream>
#include <sstream>

#include <optional>

#include "OrderTree.h"
#include "Order.h"

struct TradeRecord {
  int timestamp;
  double price;
  double quantity;
  int time;
  std::vector<std::string> party1;
  std::vector<std::string> party2;

  std::string toString() const {
    std::ostringstream ss;
    ss << "timestamp: " << timestamp
       << " price: " << price
       << " quantity: " << quantity
       << " time: " << time;
    return ss.str();
  }

};

class OrderBook {
  private:
  std::deque<TradeRecord> tape; // Assuming tape elements are strings

  double last_tick;
  int last_timestamp;
  double tick_size;
  int time;
  int next_order_id;

  public:
  OrderTree bids;
  OrderTree asks;
  OrderBook(double tick_size = 0.0001)
  : last_tick(0), last_timestamp(0), tick_size(tick_size), time(0), next_order_id(0) {}

  void update_time()
  {
    time++;
  }

  std::pair<std::vector<TradeRecord>, Quote> process_order(Quote& quote, bool from_data, bool verbose)
  {
    std::string order_type = quote.type;
    Quote order_in_book;

    if (from_data) {
      this->time = quote.timestamp;
    } else {
      this->update_time();
      quote.timestamp = this->time;
    }

    if (quote.quantity <= 0) {
      std::cerr << "process_order() given order of quantity <= 0" << std::endl;
      std::exit(EXIT_FAILURE);
    }

    if (!from_data) {
      this->next_order_id++;
    }

    std::vector<TradeRecord> trades;

    if (order_type == "market") {
    trades = this->process_market_order(quote, verbose);
    } else if (order_type == "limit") {
    trades = this->process_limit_order(quote, from_data, verbose).first;
    order_in_book = this->process_limit_order(quote, from_data, verbose).second;
    } else {
    std::cerr << "order_type for process_order() is neither 'market' nor 'limit'" << std::endl;
    std::exit(EXIT_FAILURE);
    }

    return {trades, order_in_book};

  }

  std::pair<int, std::vector<TradeRecord>> process_order_list(const std::string& side,
    OrderList& order_list, int quantity_still_to_trade, Quote& quote, bool verbose)
  {
    std::vector<TradeRecord> trades;
    int quantity_to_trade = quantity_still_to_trade;
    
    while (order_list.size() && quantity_to_trade > 0) {
      Order* head_order = order_list.head_order;
      double traded_price = head_order->price;
      int counter_party = head_order->trade_id;
      double new_book_quantity = 0;
      int traded_quantity = 0;

      if (quantity_to_trade < head_order->quantity) {
        traded_quantity = quantity_to_trade;
        new_book_quantity = head_order->quantity - quantity_to_trade;
        head_order->update_quantity(new_book_quantity, head_order->timestamp);
        quantity_to_trade = 0;
      } else if (quantity_to_trade == head_order->quantity) {
        traded_quantity = quantity_to_trade;
        if (side == "bid") {
          this->bids.remove_order_by_id(head_order->order_id);
        } else {
          this->asks.remove_order_by_id(head_order->order_id);
        }
        quantity_to_trade = 0;
      } else {
        traded_quantity = head_order->quantity;
        if (side == "bid") {
          this->bids.remove_order_by_id(head_order->order_id);
        } else {
          this->asks.remove_order_by_id(head_order->order_id);
        }
        quantity_to_trade -= traded_quantity;
      }

      if (verbose) {
        std::cout << "TRADE: Time - " << this->time << ", Price - " << traded_price << ", Quantity - " << traded_quantity << ", OrderListID - " << counter_party << ", Matching OrderListID - " << quote.trade_id << std::endl;
      }

      TradeRecord transaction_record;
      transaction_record.timestamp = this->time;
      transaction_record.price = traded_price;
      transaction_record.quantity = traded_quantity;
      transaction_record.time = this->time;

      if (side == "bid"){
        transaction_record.party1 = {std::to_string(counter_party), "bid",
                      std::to_string(head_order->order_id), std::to_string(new_book_quantity)};
        transaction_record.party2 = {std::to_string(quote.trade_id), "N/A", "", "N/A"};
      }else{
        transaction_record.party1 = {std::to_string(counter_party), "ask",
                  std::to_string(head_order->order_id), std::to_string(new_book_quantity)};
        transaction_record.party2 = {std::to_string(quote.trade_id), "bid", "N/A", "N/A"};
      }
      trades.push_back(transaction_record);
    }

    return {quantity_to_trade, trades};

  }

  std::vector<TradeRecord> process_market_order(Quote& quote, bool verbose)
  {
    std::vector<TradeRecord> trades;
    int quantity_to_trade = quote.quantity;
    std::string side = quote.side;

    if (side == "bid") {
      while (quantity_to_trade > 0 && asks.size()) {
        OrderList* best_price_asks = asks.min_price_list(); // Assuming min_price_list returns a pointer to OrderList
        if (!best_price_asks) {
          break; // No asks available
        }
        auto [remaining_quantity, new_trades] = process_order_list("ask", *best_price_asks, quantity_to_trade, quote, verbose);
        quantity_to_trade = remaining_quantity;
        trades.insert(trades.end(), new_trades.begin(), new_trades.end());
      }
    } else if (side == "ask") {
      while (quantity_to_trade > 0 && bids.size()) {
        OrderList* best_price_bids = bids.max_price_list(); // Assuming max_price_list returns a pointer to OrderList
        if (!best_price_bids) {
          break; // No bids available
        }
        auto [remaining_quantity, new_trades] = process_order_list("bid", *best_price_bids, quantity_to_trade, quote, verbose);
        quantity_to_trade = remaining_quantity;
        trades.insert(trades.end(), new_trades.begin(), new_trades.end());
      }
    } else {
      std::cerr << "process_market_order() received neither 'bid' nor 'ask'" << std::endl;
      std::exit(EXIT_FAILURE); // Consider using exceptions or error codes for error handling
    }

    return trades;

  }

  std::pair<std::vector<TradeRecord>, Quote> process_limit_order(
    Quote& quote, bool from_data, bool verbose)
  {
    std::vector<TradeRecord> trades;
    int quantity_to_trade = quote.quantity;
    std::string side = quote.side;
    double price = quote.price;
    std::optional<Quote> order_in_book;

    if (side == "bid") {
      while (asks.size() && price >= asks.min_price() && quantity_to_trade > 0) {
        OrderList* best_price_asks = asks.min_price_list();
        if (!best_price_asks) {
          break; // No asks available
        }
        auto [remaining_quantity, new_trades] = process_order_list("ask", *best_price_asks, quantity_to_trade, quote, verbose);
        quantity_to_trade = remaining_quantity;
        trades.insert(trades.end(), new_trades.begin(), new_trades.end());
      }
      if (quantity_to_trade > 0) {
        if (!from_data) {
          quote.order_id = next_order_id++;
        }
        quote.quantity = quantity_to_trade;
        bids.insert_order(&quote);
        order_in_book = quote;
      }
    } else if (side == "ask") {
      while (bids.size() && price <= bids.max_price() && quantity_to_trade > 0) {
        OrderList* best_price_bids = bids.max_price_list();
        if (!best_price_bids) {
          break; // No bids available
        }
        auto [remaining_quantity, new_trades] = process_order_list("bid", *best_price_bids, quantity_to_trade, quote, verbose);
        quantity_to_trade = remaining_quantity;
        trades.insert(trades.end(), new_trades.begin(), new_trades.end());
      }
      if (quantity_to_trade > 0) {
        if (!from_data) {
          quote.order_id = next_order_id++;
        }
        quote.quantity = quantity_to_trade;
        asks.insert_order(&quote); 
        order_in_book = quote;
      }
    } else {
      std::cerr << "process_limit_order() received neither 'bid' nor 'ask'" << std::endl;
      std::exit(EXIT_FAILURE); // Consider using exceptions for error handling
    }

    return {trades, *order_in_book};
  }

  void cancel_order(const std::string& side, int order_id, int time = 0) 
  {
    if (side != "bid" && side != "ask") {
    std::cerr << "cancel_order() given neither \"bid\" nor \"ask\"" << std::endl;
    std::exit(EXIT_FAILURE);
    }
  }

  void modify_order(int order_id, const Quote& order_update, int time) {
    if (time != 0) {
      this->time = time;
    } else {
      this->update_time();
    }

    std::string side = order_update.side;
    // Assuming Quote has order_id and timestamp as its members
    Quote updated_order = order_update;
    updated_order.order_id = order_id;
    updated_order.timestamp = this->time;

    if (side == "bid") {
      if (bids.order_exists(order_id)) {
        bids.update_order(&updated_order); // Assuming update_order method exists
      }
    } else if (side == "ask") {
      if (asks.order_exists(order_id)) {
        asks.update_order(&updated_order); // Assuming update_order method exists
      }
    } else {
      std::cerr << "modify_order() given neither 'bid' nor 'ask'" << std::endl;
      std::exit(EXIT_FAILURE); // Consider using exceptions or other error handling techniques
    }
  }

  int get_volume_at_price(const std::string& side, double price) {
    int volume = 0;

    if (side == "bid") {
    if (bids.price_exists(price)) {
      volume = bids.get_price_list(price).volume;
    }
    } else if (side == "ask") {
    if (asks.price_exists(price)) {
      volume = asks.get_price_list(price).volume;
    }
    } else {
    std::cerr << "get_volume_at_price() given neither 'bid' nor 'ask'" << std::endl;
    std::exit(EXIT_FAILURE); // Consider using exceptions or other error handling techniques
    }

    return volume;
  }


  double get_best_bid() {
    return bids.max_price();
  }

  double get_worst_bid() {
    return bids.min_price();
  }

  double get_best_ask() {
    return asks.min_price();
  }

  double get_worst_ask() {
    return asks.max_price();
  }

  void tape_dump(const std::string& filename, const std::string& filemode, const std::string& tapemode) {
    std::ofstream dumpfile;
    
    if (filemode == "write") {
      dumpfile.open(filename, std::ios::out);
    } else if (filemode == "append") {
      dumpfile.open(filename, std::ios::app);
    } else {
      std::cerr << "Invalid file mode." << std::endl;
      return;
    }

    if (!dumpfile.is_open()) {
      std::cerr << "Failed to open file." << std::endl;
      return;
    }

    for (const auto& tapeitem : tape) {
      dumpfile << "Time: " << tapeitem.time << ", Price: " << tapeitem.price << ", Quantity: " << tapeitem.quantity << "\n";
    }

    dumpfile.close();

    if (tapemode == "wipe") {
      tape.clear();
    }
  }


  std::string toString() const {
    std::ostringstream ss;
    ss << "***Bids***\n";
    if (bids.price_map.size()) {
      for (auto it = bids.price_map.rbegin(); it != bids.price_map.rend(); ++it) {
        ss << it->second.toString() << '\n';
      }
    }

    ss << "\n***Asks***\n";
    if (!asks.price_map.empty()) {
      for (const auto& pair : asks.price_map) {
        ss << pair.second.toString() << '\n';
      }
    }

    ss << "\n***Trades***\n";
    if (!tape.empty()) {
      int num = 0;
      for (const auto& entry : tape) {
        if (num < 10) { // get last 10 entries
          ss << entry.quantity << " @ " << entry.price << " (" << entry.timestamp << ") " << entry.party1[0] << "/" << entry.party2[0] << "\n";
          ++num;
        } else {
          break;
        }
      }
    }

    ss << "\n";
    return ss.str();
  }
};
