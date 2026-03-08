#pragma twice

#include "../db/pg_pool.h"
#include <json/object.h>
#include <string>
#include <vector>

struct Product {
  int id = 0;
  std::string name;
  std::string description;
  std::string price; // stored as text for numeric precision
  int stock = 0;
  std::string category;
};

class ProductRepository {
private:
  PgPool &pool_;

public:
  ProductRepository(PgPool &pool);

  std::vector<Product> getAllProducts();

  void addProduct(const Product &product);

  void updateProduct(int id, json::object obj);

  void deleteProduct(int id);
};
