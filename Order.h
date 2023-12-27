// Order.h
#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <string>
#include <unordered_map>

class OrderList;

struct Quote {
  int timestamp;
  std::string type;
  std::string side;
  double quantity;
  double price;
  int trade_id;
  int order_id = 0;
  Quote() = default;

  Quote(std::string type, std::string side, double quantity, double price,
        int trade_id)
      : type(type), side(side), quantity(quantity), price(price),
        trade_id(trade_id) {}

  void setTimestamp(int newTimestamp) { timestamp = newTimestamp; }
};

class Order {
public:
  int timestamp = 0;
  double quantity;
  double price;
  int order_id;
  int trade_id;

  Order *next_order;
  Order *prev_order;
  OrderList *order_list;

  Order(Quote *quote, OrderList *order_list);
  Order *get_next_order();
  Order *get_prev_order();
  void update_quantity(double new_quantity, double new_timestamp);
  std::string toString() const;
};

#endif // ORDER_H