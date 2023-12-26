#include<map>
#include<unordered_map>
#include<vector>

#include "OrderList.h"
#include "Order.h"

class OrderTree{

  private:



  public:
    int volume;
    std::map<double, OrderList> price_map;
    std::unordered_map<int, Order> order_map;
    int num_orders;
    int depth;
    
    OrderTree();
    size_t size() const;
    OrderList& get_price_list(int price);
    Order& get_order(int order_id);
    void create_price(int price);
    void remove_price(int price);
    bool price_exists(double price);
    bool order_exists(int order_id);

    void insert_order(Quote* quote);
    void update_order(Quote* order_update);
    void remove_order_by_id(int order_id);

    double max_price();
    double min_price();
    OrderList* max_price_list();
    OrderList* min_price_list();
};