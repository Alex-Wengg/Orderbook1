#include <map>
#include <unordered_map>
#include <vector>

#include "Order.h"
#include "OrderList.h"
#include "OrderTree.h"

OrderTree::OrderTree() : volume(0), num_orders(0), depth(0) {}

size_t OrderTree::size() const { return this->order_map.size(); }

OrderList &OrderTree::get_price_list(int price) { return price_map.at(price); }

Order &OrderTree::get_order(int order_id) { return order_map.at(order_id); }

void OrderTree::create_price(int price) {
  this->depth += 1;
  this->price_map[price] = OrderList();
}

void OrderTree::remove_price(int price) {
  this->depth -= 1;
  this->price_map.erase(price);
}

bool OrderTree::price_exists(double price) {
  return this->price_map.find(price) != price_map.end();
}

bool OrderTree::order_exists(int order_id) {
  return this->order_map.find(order_id) != order_map.end();
}

void OrderTree::insert_order(Quote *quote) {
  if (order_exists((quote->order_id))) {
    remove_order_by_id((quote->order_id));
  }
  num_orders++;

  if (this->price_map.find(quote->price) == this->price_map.end()) {
    create_price(quote->price);
  }

  // Order newOrder(quote, &price_map[quote->price]);
  Order *newOrder = new Order(quote, &price_map[quote->price]);

  price_map[quote->price].append_order(newOrder);
  std::unordered_map<int, Order> order_map;

  order_map.emplace(newOrder->order_id, *newOrder);
  volume += newOrder->quantity;
}

void OrderTree::update_order(Quote *order_update) {
  Order order = order_map.at(order_update->order_id);
  int originalQuantity = order_update->quantity;

  if (order_update->price != order.price) {
    OrderList orderList = price_map.at(order.price);
    orderList.remove_order(&order);
    if (orderList.size() == 0) {
      this->remove_price(order.price);
    }

    this->insert_order(order_update);
  } else {
    order.update_quantity(order_update->quantity, order_update->timestamp);
  }
  volume += order.quantity - originalQuantity;
}

void OrderTree::remove_order_by_id(int order_id) {
  this->num_orders--;
  Order &order = order_map.at(order_id);

  this->volume -= order.quantity;

  OrderList &orderList = price_map.at(order.price);
  orderList.remove_order(&order);

  if (orderList.size() == 0) {
    remove_price(order.price);
  }

  order_map.erase(order_id);
}

double OrderTree::max_price() {
  if (depth > 0) {
    return price_map.rbegin()->first;
  } else {
    return 0;
  }
}

double OrderTree::min_price() {
  if (depth > 0) {
    return price_map.begin()->first;
  } else {
    return 0;
  }
}

OrderList *OrderTree::max_price_list() {
  if (depth > 0) {
    return &(price_map.rbegin()->second);
  } else {
    return nullptr;
  }
}

OrderList *OrderTree::min_price_list() {
  if (depth > 0) {
    return &(price_map.begin()->second);
  } else {
    return nullptr;
  }
}
