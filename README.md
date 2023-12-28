 g++ Main.cpp Order.cpp OrderList.cpp OrderBook.cpp OrderTree.cpp -o main && ./main


 valgrind --leak-check=full --track-origins=yes --verbose ./main