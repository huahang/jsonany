/**
 * @author Huahang Liu
 * @since 2020-08-18
 */

#include <json/any.h>

struct Person;

struct Singer {
  std::string type;
  int age = 0;

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
  int age = 0;
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

struct NonCopyable {
  NonCopyable() {}

  NonCopyable(const NonCopyable& a) = delete;

  std::string name;

  template <typename AllocatorType>
  void Dump(rapidjson::Value& v, AllocatorType& alloc) {
    v.SetObject();
    v.AddMember("name", name, alloc);
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
  }
};
