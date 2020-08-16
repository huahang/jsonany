/**
 * @author Huahang Liu
 * @since 2020-08-15
 */

#include <stdlib.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "json/any.h"

struct Person;

struct Singer {
  std::string type;
  int age;

  template <typename AllocatorType>
  void Dump(rapidjson::Value& v, AllocatorType& alloc) {
    v.SetObject();
    v.AddMember("type", type, alloc);
    v.AddMember("age", age, alloc);
  }

  void Parse(const rapidjson::Value& v) {
    using rapidjson::Value;
    if (!v.HasMember("type")) {
      throw std::invalid_argument("No 'type' in JSON");
    }
    const Value& typeValue = v["type"];
    if (!typeValue.IsString()) {
      throw std::invalid_argument("Invalid 'type' in JSON");
    }
    this->type = typeValue.GetString();
    if (!v.HasMember("age")) {
      throw std::invalid_argument("No 'age' in JSON");
    }
    const Value& ageValue = v["age"];
    if (!ageValue.IsInt()) {
      throw std::invalid_argument("Invalid 'age' in JSON");
    }
    this->age = ageValue.GetInt();
  }
};

struct Band {
  std::vector<Singer> singers;

  template <typename AllocatorType>
  void Dump(rapidjson::Value& v, AllocatorType& alloc) {
    using rapidjson::Value;
    Value singersValue;
    json::Dump(singersValue, alloc, singers);
    v.SetObject();
    v.AddMember("singers", singersValue, alloc);
  }

  void Parse(const rapidjson::Value& v) {
    using rapidjson::Value;
    if (!v.HasMember("singers")) {
      throw std::invalid_argument("No 'singers' in JSON");
    }
    const Value& singersValue = v["singers"];
    if (!singersValue.IsArray()) {
      throw std::invalid_argument("Invalid 'singers' in JSON");
    }
    this->singers = json::ParseArray<Singer>(singersValue);
  }
};

struct Address {
  std::string country;
  std::string city;
  std::string street;
  std::vector<Person> neighbors;

  template <typename AllocatorType>
  void Dump(rapidjson::Value& v, AllocatorType& alloc) {
    using rapidjson::Value;
    v.SetObject();
    v.AddMember("country", country, alloc);
    v.AddMember("city", city, alloc);
    v.AddMember("street", street, alloc);
    Value neighborsValue;
    json::Dump(neighborsValue, alloc, neighbors);
    v.AddMember("neighbors", neighborsValue, alloc);
  }

  void Parse(const rapidjson::Value& v) {
    using rapidjson::Value;
    if (!v.HasMember("country")) {
      throw std::invalid_argument("No 'country' in JSON");
    }
    const Value& countryValue = v["country"];
    if (!countryValue.IsString()) {
      throw std::invalid_argument("Invalid 'country' in JSON");
    }
    this->country = countryValue.GetString();
    if (!v.HasMember("city")) {
      throw std::invalid_argument("No 'city' in JSON");
    }
    const Value& cityValue = v["city"];
    if (!cityValue.IsString()) {
      throw std::invalid_argument("Invalid 'city' in JSON");
    }
    this->city = cityValue.GetString();
    if (!v.HasMember("street")) {
      throw std::invalid_argument("No 'street' in JSON");
    }
    const Value& streetValue = v["street"];
    if (!streetValue.IsString()) {
      throw std::invalid_argument("Invalid 'street' in JSON");
    }
    this->street = streetValue.GetString();
    if (!v.HasMember("neighbors")) {
      throw std::invalid_argument("No 'neighbors' in JSON");
    }
    const Value& neighborsValue = v["neighbors"];
    if (!neighborsValue.IsArray()) {
      throw std::invalid_argument("Invalid 'neighbors' in JSON");
    }
    this->neighbors = json::ParseArray<Person>(neighborsValue);
  }
};

struct Friend {
  std::string relation;
  json::Any secret;

  template <typename AllocatorType>
  void Dump(rapidjson::Value& v, AllocatorType& alloc) {
    using rapidjson::Value;
    v.SetObject();
    v.AddMember("relation", relation, alloc);
    Value secretValue;
    secret.Dump(secretValue, alloc);
    v.AddMember("secret", secretValue, alloc);
  }

  void Parse(const rapidjson::Value& v) {
    using rapidjson::Value;
    if (!v.HasMember("relation")) {
      throw std::invalid_argument("No 'relation' in JSON");
    }
    const Value& relationValue = v["relation"];
    if (!relationValue.IsString()) {
      throw std::invalid_argument("Invalid 'relation' in JSON");
    }
    this->relation = relationValue.GetString();
    if (!v.HasMember("secret")) {
      throw std::invalid_argument("No 'secret' in JSON");
    }
    const Value& secretValue = v["secret"];
    this->secret.Parse(secretValue);
  }
};

struct Person {
  std::string name;
  int age;
  Address address;
  std::vector<Friend> friends;
  json::Any secret;

  template <typename AllocatorType>
  void Dump(rapidjson::Value& v, AllocatorType& alloc) {
    using rapidjson::Value;
    v.SetObject();
    v.AddMember("name", name, alloc);
    v.AddMember("age", age, alloc);
    Value addressValue;
    address.Dump(addressValue, alloc);
    v.AddMember("address", addressValue, alloc);
    Value friendsValue;
    json::Dump(friendsValue, alloc, friends);
    v.AddMember("friends", friendsValue, alloc);
    Value secretValue;
    secret.Dump(secretValue, alloc);
    v.AddMember("secret", secretValue, alloc);
  }

  void Parse(const rapidjson::Value& v) {
    using rapidjson::Value;
    if (!v.HasMember("name")) {
      throw std::invalid_argument("No 'name' in JSON");
    }
    const Value& nameValue = v["name"];
    if (!nameValue.IsString()) {
      throw std::invalid_argument("Invalid 'name' in JSON");
    }
    this->name = nameValue.GetString();
    if (!v.HasMember("age")) {
      throw std::invalid_argument("No 'age' in JSON");
    }
    const Value& ageValue = v["age"];
    if (!ageValue.IsInt()) {
      throw std::invalid_argument("Invalid 'age' in JSON");
    }
    this->age = ageValue.GetInt();
    if (!v.HasMember("address")) {
      throw std::invalid_argument("No 'address' in JSON");
    }
    const Value& addressValue = v["address"];
    if (!addressValue.IsObject()) {
      throw std::invalid_argument("Invalid 'address' in JSON");
    }
    this->address.Parse(addressValue);
    if (!v.HasMember("friends")) {
      throw std::invalid_argument("No 'friends' in JSON");
    }
    const Value& friendsValue = v["friends"];
    if (!friendsValue.IsArray()) {
      throw std::invalid_argument("Invalid 'friends' in JSON");
    }
    this->friends = json::ParseArray<Friend>(friendsValue);
    if (!v.HasMember("secret")) {
      throw std::invalid_argument("No 'secret' in JSON");
    }
    const Value& secretValue = v["secret"];
    this->secret.Parse(secretValue);
  }
};

int main() {
  using rapidjson::Document;
  std::string json;
  // Singer
  Singer s1{"rapper", 16};
  json = json::DumpPretty(s1);
  std::cout << json << std::endl;
  json = json::Dump(s1);
  std::cout << "s1: " << json << std::endl;
  Singer s1Parsed = json::Parse<Singer>(json);
  json = json::Dump(s1Parsed);
  std::cout << "s1 parsed: " << json << std::endl;
  json::Any any = s1;
  json = json::Dump(any);
  std::cout << "s1 any: " << json << std::endl;
  Singer s1Casted = json::AnyCast<Singer>(any);
  json = json::Dump(s1Casted);
  std::cout << "s1 casted: " << json << std::endl;
  // Any a = std::make_shared<Singer>(xxxx);
  auto spSinger = std::make_shared<Singer>(Singer{"rapper", 16});
  any = spSinger;
  json = json::Dump(any);
  std::cout << "s1 sp: " << json << std::endl;
  // Bad cast
  try {
    json::AnyCast<Band>(any);
  } catch (const std::bad_cast& e) {
    std::cout << "Can't cast any to Band" << std::endl;
  }
  try {
    json::Parse<Singer>("[]");
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  // Band
  Band band{{s1, s1}};
  json = json::Dump(band);
  std::cout << "band: " << json << std::endl;
  Band bandParsed = json::Parse<Band>(json);
  json = json::Dump(bandParsed);
  std::cout << "band parsed: " << json << std::endl;
  any = band;
  json = json::Dump(any);
  std::cout << "band any: " << json << std::endl;
  // Friend
  Friend f1{"my best friend", Singer{"rocker", 18}};
  json = json::Dump(f1);
  std::cout << json << std::endl;
  Friend f2{"new friend", "little girl"};
  json = json::Dump(f2);
  std::cout << json << std::endl;
  Friend f3{"third friend", 3};
  json = json::Dump(f3);
  std::cout << json << std::endl;
  // Person
  Person p2{"p2", 3, Address{"china", "shanghai", "putuo"}};
  json = json::Dump(p2);
  std::cout << json << std::endl;
  // Address
  Address addr1{"china", "beijing", "wangjing", {p2}};
  json = json::Dump(addr1);
  std::cout << json << std::endl;
  // Final!
  Person p1{"p1", 4, addr1, {f1, f2, f3}, "the kind!"};
  json = json::Dump(p1);
  std::cout << json << std::endl;
  Person p1Parsed = json::Parse<Person>(json);
  std::string json2;
  json2 = json::Dump(p1Parsed);
  std::cout << json2 << std::endl;
  json::Any anyPerson = std::make_shared<Person>(p1);
  std::string json3;
  json3 = json::Dump(anyPerson);
  std::cout << json3 << std::endl;
  if (json != json2) {
    std::cout << "Bad!" << std::endl;
    return EXIT_FAILURE;
  }
  if (json != json3) {
    std::cout << "Bad!" << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Good!" << std::endl;
  return EXIT_SUCCESS;
}
