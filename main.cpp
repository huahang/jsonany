/**
 * @author Huahang Liu
 * @since 2020-08-15
 */

#include <stdlib.h>

#include <boost/hana.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "json/any.h"

struct Person;

struct Singer {
  BOOST_HANA_DEFINE_STRUCT(Singer, (std::string, type), (int, age));
};

struct Address {
  BOOST_HANA_DEFINE_STRUCT(Address,
                           (std::string, country),
                           (std::string, city),
                           (std::string, street),
                           (std::vector<Person>, neighbors));
};

struct Friend {
  BOOST_HANA_DEFINE_STRUCT(Friend,
                           (std::string, relation),
                           (json::Any, secret));
};

struct Person {
  BOOST_HANA_DEFINE_STRUCT(Person,
                           (std::string, name),
                           (int, age),
                           (Address, address),
                           (std::vector<Friend>, friends),
                           (json::Any, secret));
};

template <typename T>
void Serialize(const T& o, rapidjson::Writer<rapidjson::StringBuffer>& w) {
  namespace hana = boost::hana;
  w.StartObject();
  auto accessors = hana::accessors<T>();
  boost::hana::for_each(accessors, [&o, &w](auto accessor) {
    auto key = boost::hana::first(accessor);
    auto get = boost::hana::second(accessor);
    auto value = get(o);
    w.String(key.c_str());
    auto& typeInfo = typeid(value);
    if (typeInfo == typeid(std::string)) {
      w.String(reinterpret_cast<const std::string&>(value));
    } else if (typeInfo == typeid(int)) {
      w.Int(reinterpret_cast<const int&>(value));
    } else if (typeInfo == typeid(int64_t)) {
      w.Int64(reinterpret_cast<const int64_t&>(value));
    } else if (typeInfo == typeid(Singer)) {
      Serialize(reinterpret_cast<const Singer&>(value), w);
    } else if (typeInfo == typeid(Address)) {
      Serialize(reinterpret_cast<const Address&>(value), w);
    } else if (typeInfo == typeid(Friend)) {
      Serialize(reinterpret_cast<const Friend&>(value), w);
    } else if (typeInfo == typeid(Person)) {
      Serialize(reinterpret_cast<const Person&>(value), w);
    } else if (typeInfo == typeid(std::vector<Friend>)) {
      const std::vector<Friend>& friends =
          reinterpret_cast<const std::vector<Friend>&>(value);
      w.StartArray();
      for (const auto& f : friends) {
        Serialize(f, w);
      }
      w.EndArray();
    } else if (typeInfo == typeid(std::vector<Person>)) {
      const std::vector<Person>& persons =
          reinterpret_cast<const std::vector<Person>&>(value);
      w.StartArray();
      for (const auto& p : persons) {
        Serialize(p, w);
      }
      w.EndArray();
    } else if (typeInfo == typeid(json::Any)) {
      json::Any& any = reinterpret_cast<json::Any&>(value);
      if (any.TypeInfo() == typeid(std::string)) {
        w.String(json::AnyCast<std::string>(any));
      } else if (any.TypeInfo() == typeid(int)) {
        w.Int(json::AnyCast<int>(any));
      } else if (any.TypeInfo() == typeid(int64_t)) {
        w.Int(json::AnyCast<int64_t>(any));
      } else if (any.TypeInfo() == typeid(Singer)) {
        Serialize(json::AnyCast<Singer>(any), w);
      } else if (any.TypeInfo() == typeid(Address)) {
        Serialize(json::AnyCast<Address>(any), w);
      } else if (any.TypeInfo() == typeid(Friend)) {
        Serialize(json::AnyCast<Friend>(any), w);
      } else if (any.TypeInfo() == typeid(Person)) {
        Serialize(json::AnyCast<Person>(any), w);
      } else if (any.IsNull()) {
        w.Null();
      } else {
        w.String("(unknown)");
      }
    } else {
      w.String("(unknown)");
    }
  });
  w.EndObject();
}

template <typename T>
std::string Serialize(const T& o) {
  using rapidjson::PrettyWriter;
  using rapidjson::StringBuffer;
  StringBuffer sb;
  PrettyWriter<StringBuffer> w(sb);
  Serialize(o, w);
  return sb.GetString();
}

template <typename T>
T Parse(const std::string& json) {
  namespace hana = boost::hana;
  using rapidjson::Document;
  Document document;
  document.Parse(json);
  T result;
  auto accessors = hana::accessors<T>();
  boost::hana::for_each(accessors, [&result](auto accessor) {
    auto key = boost::hana::first(accessor);
    auto get = boost::hana::second(accessor);
    if (std::string(key.c_str()) == "type") {
    }
  });
  return result;
}

template <typename Xs>
std::string join(Xs&& xs, std::string sep) {
  return boost::hana::fold(boost::hana::intersperse(std::forward<Xs>(xs), sep), "", boost::hana::_ + boost::hana::_);
}

std::string quote(std::string s) { return "\"" + s + "\""; }

template <typename T>
auto to_json(T const& x) -> decltype(std::to_string(x)) {
  return std::to_string(x);
}

std::string to_json(char c) { return quote({c}); }

std::string to_json(std::string s) { return quote(s); }

template <typename Xs>
std::enable_if_t<boost::hana::Sequence<Xs>::value,
  std::string> to_json(Xs const& xs) {
  auto json = boost::hana::transform(xs, [](auto const& x) {
    return to_json(x);
  });
  return "[" + join(std::move(json), ", ") + "]";
}

template <typename T>
std::enable_if_t<boost::hana::Struct<T>::value, std::string> to_json(
    T const& x) {
  auto json = boost::hana::transform(boost::hana::keys(x), [&](auto name) {
    auto const& member = boost::hana::at_key(x, name);
    return quote(boost::hana::to<char const*>(name)) + " : " + to_json(member);
  });
  return "{" + join(std::move(json), ", ") + "}";
}

int main() {
  std::string json;
  Singer s1{"rapper", 16};
  json = Serialize(s1);
  std::cout << json << std::endl;
  json = to_json(s1);
  std::cout << json << std::endl;
  Singer s1Parsed = Parse<Singer>(json);
  Friend f1{"my best friend", Singer{"rocker", 18}};
  std::cout << Serialize(f1) << std::endl;
  auto& f1Secret = json::AnyCast<Singer>(f1.secret);
  std::cout << Serialize(f1Secret) << std::endl;
  Friend f2{"new friend", "little girl"};
  std::cout << Serialize(f2) << std::endl;
  auto& f2Secret = json::AnyCast<std::string>(f2.secret);
  Friend f3{"third friend", 3};
  std::cout << Serialize(f3) << std::endl;
  auto& f3Secret = json::AnyCast<int>(f3.secret);
  Person p2{"p2", 3, Address{"china", "shanghai", "putuo"}};
  std::cout << Serialize(p2) << std::endl;
  auto& p2SecretType = p2.secret.TypeInfo();
  Address addr1{"china", "beijing", "wangjing", {p2}};
  std::cout << Serialize(addr1) << std::endl;
  Person p1{"p1", 4, addr1, {f1, f2, f3}, "the kind!"};
  auto& p1Secret = json::AnyCast<std::string>(p1.secret);
  std::cout << Serialize(p1) << std::endl;

  // TODO. 以下是伪代码，需要笔试者具体实现 // auto json = dump(p1)
  // std::cout << json << std::endl // std::cout << p1 << std::endl
  // auto pp = parse(json); // assert(p1 == pp)
  // 序列化
  // 打印序列化结果
  // 打印 Person 对象 // 反序列化
  // 反序列化的结果是对的
  return EXIT_SUCCESS;
}
