#include "product_services.h"
#include "json/object.h"

ProductRepository::ProductRepository(const std::string &conninfo)
    : db(conninfo) {}

std::vector<Product> ProductRepository::getAllProducts() {

  std::vector<Product> products;

  auto result = db.exec_params("SELECT id, name, description, price::text, stock, category "
                               "FROM products ORDER BY id ASC",
                               {});

  for (int i = 0; i < result.GetRows(); i++) {
    Product p;
    p.id = std::stoi(result.GetEl(i, 0));
    p.name = result.GetEl(i, 1);
    p.description = result.GetEl(i, 2);
    p.price = result.GetEl(i, 3);
    p.stock = std::stoi(result.GetEl(i, 4));
    p.category = result.GetEl(i, 5);

    products.push_back(p);
  }

  return products;
}

void ProductRepository::addProduct(const Product &product) {

  db.exec_params("INSERT INTO products "
                 "(name, description, price, stock, category) "
                 "VALUES ($1, $2, $3::numeric, $4::integer, $5)",
                 {product.name, product.description, product.price,
                  std::to_string(product.stock), product.category});
}

void ProductRepository::updateProduct(int id, json::object obj) {
  std::string name = "";
  std::string description = "";
  std::string price = "";
  std::string stock = "";
  std::string category = "";

  if (obj.find("name") != obj.end())
    name = static_cast<std::string>(obj["name"]);

  if (obj.find("description") != obj.end())
    description = static_cast<std::string>(obj["description"]);

  if (obj.find("price") != obj.end())
    price = static_cast<std::string>(obj["price"]);

  if (obj.find("stock") != obj.end())
    stock = std::to_string(static_cast<int>(obj["stock"]));

  if (obj.find("category") != obj.end())
    category = static_cast<std::string>(obj["category"]);

  db.exec_params("UPDATE products SET "
                 "name = COALESCE(NULLIF($2, ''), name), "
                 "description = COALESCE(NULLIF($3, ''), description), "
                 "price = CASE WHEN $4 != '' THEN $4::numeric ELSE price END, "
                 "stock = CASE WHEN $5 != '' THEN $5::integer ELSE stock END, "
                 "category = COALESCE(NULLIF($6, ''), category) "
                 "WHERE id = $1::integer",
                 {std::to_string(id), name, description, price, stock, category});
}

void ProductRepository::deleteProduct(int id) {
  db.exec_params("DELETE FROM products WHERE id = $1::integer",
                 {std::to_string(id)});
}
