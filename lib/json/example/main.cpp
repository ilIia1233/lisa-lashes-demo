// Includes required
#include <json/parse.h>

// Using namespace
using namespace json;

int main(int argc, char *argv[]) {
  object person;
  person["name"] = "Adit";
  person["age"] = 21;
  person.insert("height", 186.5);
  person["alive"] = true;
  person["links"]["github"] = "https://github.com/jadit19";
  person["links"]["linkedin"] = "https://www.linkedin.com/in/jadit19/";

  object college;
  college["name"] = "Indian Institute of Technology Kanpur";
  college["graduation"] = 2024;
  college["major"] = "Electrical Engineering";
  college["minors"].push_back("Operating Systems");
  college["minors"].push_back("Machine Learning");
  college["minors"].resize(3);
  college["minors"][2] = "English Literature";
  person["college"] = college;

  person["languages"].resize(1);
  person["languages"][0] = "English";
  person["languages"].push_back("Hindi");
  person["languages"].push_back("Spanish");
  person["languages"][2] = "French";

  parser parser;
  std::string data = person.dumps();
  object duplicate = parser.loads(data);

  logger::info(duplicate.dumps(2));

  std::string name = person["name"];
  int age = person["age"];
  bool alive = person["alive"];
  logger::info(name + " is " + std::to_string(age) + " years old and is " +
               (alive ? "alive" : "dead") + ".");

  return EXIT_SUCCESS;
}