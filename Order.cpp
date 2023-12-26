#include "OrderList.h"
#include "Order.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

Order::Order(Quote* quote, OrderList* order_list) :
  timestamp((quote->timestamp)), quantity((quote->quantity)), price((quote->price)),
  order_id((quote->order_id)), trade_id((quote->trade_id)), order_list(order_list)
{
  next_order = nullptr;
  prev_order = nullptr;
}

Order* Order::get_next_order()
{
  return this->next_order;
};

Order* Order::get_prev_order()
{
  return this->prev_order;
}

void Order::update_quantity(double new_quantity, double new_timestamp)
{
  if (new_quantity > this->quantity && (this->order_list)->tail_order != this){
    (this->order_list)->move_to_tail(this);
  }

  this->order_list->volume -= (this->quantity - new_quantity);
  this->timestamp = new_timestamp;
  this->quantity = new_quantity;
}

std::string Order::toString() const {
  std::ostringstream result;
  result << this->quantity << "@" << price << "/" << this->trade_id << " - " << this->timestamp;
  return result.str();
}