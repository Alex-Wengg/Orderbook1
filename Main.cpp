#include "Order.h"
#include "OrderBook.h"
#include "OrderList.h"

#include <iostream>
#include <vector>

int main() {
  OrderBook order_book = OrderBook(0.0);

  // std::vector<Quote> limit_orders = {
  std::vector<Quote> *limit_orders = new std::vector<Quote>{

      {"limit", "ask", 5, 101, 100}, {"limit", "ask", 5, 103, 101},
      {"limit", "ask", 5, 101, 102}, {"limit", "ask", 5, 101, 103},
      {"limit", "bid", 5, 99, 100},  {"limit", "bid", 5, 98, 101},
      {"limit", "bid", 5, 99, 102},  {"limit", "bid", 5, 97, 103}};

  for (auto order : *limit_orders) {
    order_book.process_order(order, false, false);
  }
  std::cout << order_book.toString() << std::endl;

  Quote crossing_limit_order = {"limit", "bid", 2, 102, 109};
  order_book.process_order(crossing_limit_order, false, false);
  std::cout << "Trade occurs as incoming bid limit crosses best ask"
            << std::endl;
  std::cout << order_book.toString() << std::endl;

  Quote big_crossing_limit_order = {"limit", "bid", 50, 102, 110};
  auto [trades, order_in_book] =
      order_book.process_order(big_crossing_limit_order, false, false);
  std::cout << "Large incoming bid limit crosses best ask. Remaining volume is "
               "placed in book."
            << std::endl;

  for (const auto &trade : trades) {
    std::cout << trade.toString() << std::endl;
  }
  std::cout << order_book.toString() << std::endl;

  Quote market_order = {"market", "ask", 40, 102, 111};

  auto [market_trades, market_order_id] =
      order_book.process_order(market_order, false, false);
  std::cout << "A market order takes the specified volume from the inside of ";
  std::cout << "the book, regardless of price" << std::endl;
  std::cout << "A market ask for 40 results in:" << std::endl;

  std::cout << order_book.toString() << std::endl;
  return 0;
}
