#ifndef ORDERLIST_H
#define ORDERLIST_H

#include <cstddef>
#include <string>

#include "Order.h"

class Order;

class OrderList {
private:
  size_t length;
  Order *last;

public:
  Order *head_order;
  Order *tail_order;
  int volume;

  OrderList();
  size_t size() const;

  class iterator {
  private:
    Order *current;

  public:
    iterator(Order *start) : current(start) {}
    iterator &operator++() {
      current = current->next_order;
      return *this;
    }

    Order *operator*() const { return current; }

    bool operator!=(const iterator &other) const {
      return current != other.current;
    }
  };

  iterator begin() const { return iterator(head_order); }
  iterator end() const { return iterator(nullptr); }

  Order *get_head_order();
  void append_order(Order *order);
  void remove_order(Order *order);
  void move_to_tail(Order *order);
  std::string toString() const;
};

#endif // ORDERLIST_H