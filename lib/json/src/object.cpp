#include <utility>

#include <json/object.h>

// Constructors
json::object::object() noexcept(true)
    : type(json::TYPE::UNDEFINED), value(nullptr) {
  return;
}
json::object::object(json::object::AllValues data) noexcept(false)
    : type(json::TYPE::UNDEFINED), value(nullptr) {
  this->reset();
  switch (data.index()) {
  case 0:
    this->value = std::get<std::nullptr_t>(data);
    this->type = json::TYPE::NUL;
    break;
  case 1:
    this->value = std::get<bool>(data);
    this->type = json::TYPE::BOOLEAN;
    break;
  case 2:
    this->value = std::get<long long int>(data);
    this->type = json::TYPE::INTEGER;
    break;
  case 3:
    this->value = std::get<double>(data);
    this->type = json::TYPE::NUMBER;
    break;
  case 4:
    this->value = std::get<std::string>(data);
    this->type = json::TYPE::STRING;
    break;
  case 5:
    this->array = std::get<std::vector<json::object>>(data);
    this->type = json::TYPE::ARRAY;
    break;
  case 6:
    this->map = std::get<std::map<std::string, json::object>>(data);
    this->type = json::TYPE::OBJECT;
    break;
  default:
    logger::error("The data type is not supported",
                  "json::object::object(json::object::AllValues data)");
    break;
  }
  return;
}
json::object::object(const char *data) noexcept(false)
    : type(json::TYPE::UNDEFINED), value(nullptr) {
  *this = std::string(data);
  return;
}
json::object::~object() noexcept(true) {
  this->clearVariant();
  return;
}

// Assignment operators
json::object &json::object::operator=(std::nullptr_t data) noexcept(true) {
  return *this = json::object(data);
}
json::object &json::object::operator=(bool data) noexcept(true) {
  return *this = json::object(data);
}
json::object &json::object::operator=(int data) noexcept(true) {
  return *this = json::object(data);
}
json::object &json::object::operator=(long long int data) noexcept(true) {
  return *this = json::object(data);
}
json::object &json::object::operator=(double data) noexcept(true) {
  return *this = json::object(data);
}
json::object &json::object::operator=(const char *data) noexcept(true) {
  return *this = json::object(data);
}
json::object &json::object::operator=(const std::string &data) noexcept(true) {
  return *this = json::object(data);
}
json::object &
json::object::operator=(std::vector<json::object> data) noexcept(true) {
  return *this = json::object(data);
}
json::object &json::object::operator=(
    std::map<std::string, json::object> data) noexcept(true) {
  return *this = json::object(data);
}

// Array operations
json::object &json::object::operator[](size_t index) noexcept(false) {
  this->assertIsArray();
  if (index >= this->array.size()) {
    this->array.resize(index + 1);
  }
  return this->array[index];
}
json::object &json::object::operator[](int index) noexcept(false) {
  return this->operator[]((size_t)index);
}
void json::object::push_back(const json::object &data) noexcept(false) {
  this->assertIsArray();
  this->array.push_back(data);
  return;
}
void json::object::pop_back() noexcept(false) {
  this->assertIsArray();
  this->array.pop_back();
  return;
}
void json::object::reserve(size_t size) noexcept(false) {
  this->assertIsArray();
  this->array.reserve(size);
  return;
}
void json::object::resize(size_t size) noexcept(false) {
  this->assertIsArray();
  this->array.resize(size);
  return;
}
void json::object::shrink_to_fit() noexcept(false) {
  this->assertIsArray();
  this->array.shrink_to_fit();
  return;
}

// Map operations
json::object &json::object::operator[](const char *key) noexcept(false) {
  return this->operator[](std::string(key));
}
json::object &json::object::operator[](const std::string &key) noexcept(false) {
  this->assertIsMap();
  return this->map[key];
}
std::map<std::string, json::object>::iterator
json::object::find(const std::string key) noexcept(false) {
  this->assertIsMap();
  return this->map.find(key);
}
std::map<std::string, json::object>::iterator
json::object::begin() noexcept(false) {
  this->assertIsMap();
  return this->map.begin();
}
std::map<std::string, json::object>::iterator
json::object::end() noexcept(false) {
  this->assertIsMap();
  return this->map.end();
}
void json::object::insert(const std::string key,
                          json::object::AllValues value) noexcept(false) {
  this->assertIsMap();
  this->map[key] = json::object(value);
  return;
}

// Array and Map operators
void json::object::clear() noexcept(false) {
  if (this->type == json::TYPE::ARRAY) {
    this->clearArray();
  } else if (this->type == json::TYPE::OBJECT) {
    this->clearMap();
  } else {
    logger::error("The object is not an array or a map",
                  "void json::object::clear()");
  }
  return;
}
size_t json::object::size() const noexcept(false) {
  if (this->type == json::TYPE::ARRAY) {
    return this->array.size();
  } else if (this->type == json::TYPE::OBJECT) {
    return this->map.size();
  } else if (this->type == json::TYPE::STRING) {
    return std::get<std::string>(this->value).size();
  } else {
    logger::error("The object is not a string, array, or map",
                  "size_t json::object::size()");
  }
  return 0;
}

// Object operators
void json::object::reset() noexcept(true) {
  this->clearVariant();
  this->clearArray();
  this->clearMap();
  this->type = json::TYPE::UNDEFINED;
  return;
}
void json::object::dump(std::string filename, size_t indent) const
    noexcept(false) {
  std::ofstream file;

  try {
    file.open(filename);
    if (!file.is_open()) {
      logger::error(
          "Failed to open file " + filename,
          "void json::object::dump(std::string filename, size_t indent)");
    }

    file << this->dumps(indent);
    file.close();
  } catch (const std::exception &e) {
    logger::error(
        "Error encountered for: " + filename + " - " + std::string(e.what()),
        "void json::object::dump(std::string filename, size_t indent)");
  } catch (...) {
    logger::error(
        "An unknown error occurred for file: " + filename,
        "void json::object::dump(std::string filename, size_t indent)");
  }
  return;
}
std::string json::object::dumps(size_t indent, size_t baseIndent) const
    noexcept(false) {
  std::string result = "";
  int newBaseIndent = indent + baseIndent;
  switch (this->type) {
  case json::TYPE::UNDEFINED:
    result = "undefined";
    break;
  case json::TYPE::NUL:
    result = "null";
    break;
  case json::TYPE::BOOLEAN:
    result = std::get<bool>(this->value) ? "true" : "false";
    break;
  case json::TYPE::INTEGER:
    result = std::to_string(std::get<long long int>(this->value));
    break;
  case json::TYPE::NUMBER:
    result = std::to_string(std::get<double>(this->value));
    break;
  case json::TYPE::STRING:
    result = "\"" + std::get<std::string>(this->value) + "\"";
    break;
  case json::TYPE::ARRAY:
    result = "[";
    if (this->size() == 0) {
      result += "]";
      return result;
    }
    if (indent > 0)
      result += "\n";
    for (size_t i = 0; i < this->size(); i++) {
      result += std::string(newBaseIndent, ' ');
      result += this->array[i].dumps(indent, newBaseIndent);
      if (i + 1 != this->size())
        result += ",";
      if (indent > 0)
        result += "\n";
      else if (i + 1 != this->size())
        result += " ";
    }
    result += std::string(baseIndent, ' ') + "]";
    break;
  case json::TYPE::OBJECT:
    result = "{";
    if (this->size() == 0) {
      result += "}";
      return result;
    }
    if (indent > 0)
      result += "\n";
    for (std::map<std::string, json::object>::const_iterator it =
             this->map.begin();
         it != this->map.end(); it++) {
      result += std::string(newBaseIndent, ' ');
      result += "\"" + it->first + "\": ";
      result += it->second.dumps(indent, newBaseIndent);
      if (std::next(it) != this->map.end())
        result += ",";
      if (indent > 0)
        result += "\n";
      else if (std::next(it) != this->map.end())
        result += " ";
    }
    result += std::string(baseIndent, ' ') + "}";
  }

  return result;
}

// Conversion Operators
json::object::operator std::string() const noexcept(false) {
  if (this->type == json::TYPE::STRING) {
    return std::get<std::string>(this->value);
  } else {
    logger::error("The object is not a string",
                  "json::object::operator std::string()");
  }
  return "";
}
json::object::operator int() const noexcept(false) {
  if (this->type == json::TYPE::INTEGER) {
    return (int)std::get<long long int>(this->value);
  } else {
    logger::error("The object is not an integer",
                  "json::object::operator int()");
  }
  return 0;
}
json::object::operator long long int() const noexcept(false) {
  if (this->type == json::TYPE::INTEGER) {
    return (long long int)std::get<long long int>(this->value);
  } else {
    logger::error("The object is not an integer",
                  "json::object::operator long long int()");
  }
  return 0;
}
json::object::operator double() const noexcept(false) {
  if (this->type == json::TYPE::NUMBER) {
    return std::get<double>(this->value);
  } else {
    logger::error("The object is not a number",
                  "json::object::operator double()");
  }
  return 0.0;
}
json::object::operator bool() const noexcept(false) {
  if (this->type == json::TYPE::BOOLEAN) {
    return std::get<bool>(this->value);
  } else {
    logger::error("The object is not a boolean",
                  "json::object::operator bool()");
  }
  return false;
}

// Private methods
void json::object::clearVariant() noexcept(true) {
  this->value = nullptr;
  return;
}

void json::object::clearArray() noexcept(true) {
  this->array.clear();
  return;
}
void json::object::setArrayIfUndefined() noexcept(true) {
  if (this->type == json::TYPE::UNDEFINED) {
    this->clearVariant();
    this->clearMap();
    this->type = json::TYPE::ARRAY;
  }
  return;
}
void json::object::assertIsArray() noexcept(false) {
  this->setArrayIfUndefined();
  if (this->type != json::TYPE::ARRAY) {
    logger::error("The object is not an array",
                  "void json::object::assertIsArray()");
  }
  return;
}

void json::object::clearMap() noexcept(true) {
  this->map.clear();
  return;
}
void json::object::setMapIfUndefined() noexcept(true) {
  if (this->type == json::TYPE::UNDEFINED) {
    this->clearVariant();
    this->clearArray();
    this->type = json::TYPE::OBJECT;
  }
  return;
}
void json::object::assertIsMap() noexcept(false) {
  this->setMapIfUndefined();
  if (this->type != json::TYPE::OBJECT) {
    logger::error("The object is not a map",
                  "void json::object::assertIsMap()");
  }
  return;
}

// Friend functions
namespace json {

std::ostream &operator<<(std::ostream &os, object &obj) noexcept(true) {
  os << obj.dumps(2);
  return os;
}

} // namespace json