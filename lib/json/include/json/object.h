#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include <logger/log.h>

#include <json/type.h>

namespace json {

class object {
private:
  using Value =
      std::variant<std::nullptr_t, bool, long long int, double, std::string>;
  using AllValues = std::variant<std::nullptr_t, bool, long long int, double,
                                 std::string, std::vector<json::object>,
                                 std::map<std::string, json::object>>;

  json::TYPE type;

  void clearVariant() noexcept(true);

  void clearArray() noexcept(true);
  void setArrayIfUndefined() noexcept(true);
  void assertIsArray() noexcept(false);

  void clearMap() noexcept(true);
  void setMapIfUndefined() noexcept(true);
  void assertIsMap() noexcept(false);

protected:
  json::object::Value value;
  std::vector<json::object> array;
  std::map<std::string, json::object> map;

public:
  // Constructors
  object() noexcept(true);
  object(json::object::AllValues data) noexcept(false);
  object(const char *data) noexcept(false);
  ~object() noexcept(true);

  // Assignment operators
  json::object &operator=(std::nullptr_t data) noexcept(true);
  json::object &operator=(bool data) noexcept(true);
  json::object &operator=(int data) noexcept(true);
  json::object &operator=(long long int data) noexcept(true);
  json::object &operator=(double data) noexcept(true);
  json::object &operator=(const char *data) noexcept(true);
  json::object &operator=(const std::string &data) noexcept(true);
  json::object &operator=(std::vector<json::object> data) noexcept(true);
  json::object &
  operator=(std::map<std::string, json::object> data) noexcept(true);

  // Array operations
  json::object &operator[](const size_t index) noexcept(false);
  json::object &operator[](const int index) noexcept(false);
  void push_back(const json::object &data) noexcept(false);
  void pop_back() noexcept(false);
  void reserve(const size_t size) noexcept(false);
  void resize(const size_t size) noexcept(false);
  void shrink_to_fit() noexcept(false);

  // Map operations
  json::object &operator[](const char *key) noexcept(false);
  json::object &operator[](const std::string &key) noexcept(false);
  std::map<std::string, json::object>::iterator
  find(const std::string key) noexcept(false);
  std::map<std::string, json::object>::iterator begin() noexcept(false);
  std::map<std::string, json::object>::iterator end() noexcept(false);
  void insert(const std::string key,
              json::object::AllValues value) noexcept(false);

  // Array and Map operators
  void clear() noexcept(false);
  size_t size() const noexcept(false);

  // Object operators
  void reset() noexcept(true);
  void dump(std::string filename, size_t indent = 0) const noexcept(false);
  std::string dumps(size_t indent = 0, size_t baseIndent = 0) const
      noexcept(false);

  // Conversion Operators
  operator std::string() const noexcept(false);
  operator int() const noexcept(false);
  operator long long int() const noexcept(false);
  operator double() const noexcept(false);
  operator bool() const noexcept(false);

  // Output functions
  friend std::ostream &operator<<(std::ostream &os,
                                  const json::object &obj) noexcept(true);
};

} // namespace json