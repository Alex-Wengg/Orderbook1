#include "OrderList.h"
#include "Order.h"

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

OrderList::OrderList()
    : head_order(nullptr), tail_order(nullptr), length(0), volume(0.0) {}

size_t OrderList::size() const { return this->length; }

void OrderList::append_order(Order *order) {

  if (this->length == 0) {
    order->next_order = nullptr;
    order->prev_order = nullptr;
    this->head_order = order;
    this->tail_order = order;

  } else {
    order->prev_order = this->tail_order;
    order->next_order = nullptr;
    this->tail_order->next_order = order;
    this->tail_order = order;
  }
  this->length += 1;
  this->volume += order->quantity;
}

void OrderList::remove_order(Order *order) {

  this->volume -= order->quantity;
  this->length -= 1;
  if (length == 0)
    return;

  Order *next_order = order->next_order;
  Order *prev_order = order->prev_order;
  if (next_order != nullptr && prev_order != nullptr) {

    next_order->prev_order = prev_order;
    prev_order->next_order = next_order;
  } else if (next_order != nullptr) {

    next_order->prev_order = nullptr;
    this->head_order = next_order;
  } else if (prev_order != nullptr) {

    prev_order->next_order = nullptr;
    this->tail_order = prev_order;
  }

  // delete order;
}

void OrderList::move_to_tail(Order *order) {
  if (order->prev_order != nullptr) {
    order->prev_order->next_order = order->next_order;
  } else {
    this->head_order = order->next_order;
  }
  order->next_order->prev_order = order->prev_order;

  order->prev_order = this->tail_order;
  order->next_order = nullptr;

  this->tail_order->next_order = order;
  this->tail_order = order;
}

std::string OrderList::toString() const {
  std::stringstream output;
  Order *current = this->head_order;

  while (current != nullptr) {
    output << current->toString() << "\n";
    current = current->next_order;
  }
  return output.str();
}