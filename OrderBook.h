#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <deque>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

#include <optional>

#include "Order.h"
#include "OrderTree.h"

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

  void update_time();

  std::pair<std::vector<TradeRecord>, Quote> process_order(Quote& quote, bool from_data, bool verbose);

  std::pair<int, std::vector<TradeRecord>> process_order_list(const std::string& side,
    OrderList& order_list, int quantity_still_to_trade, Quote& quote, bool verbose);

  std::vector<TradeRecord> process_market_order(Quote& quote, bool verbose);

  std::pair<std::vector<TradeRecord>, Quote> process_limit_order(
    Quote& quote, bool from_data, bool verbose);

  void cancel_order(const std::string& side, int order_id, int time = 0);
  void modify_order(int order_id, const Quote& order_update, int time);
  int get_volume_at_price(const std::string& side, double price);

  double get_best_bid();
  double get_worst_bid();
  double get_best_ask();
  double get_worst_ask();
  void tape_dump(const std::string& filename, const std::string& filemode, const std::string& tapemode);
  std::string toString() const;
};
#endif // ORDERBOOK_H