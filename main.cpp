/**
 * @author Huahang Liu
 * @since 2020-08-15
 */

#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "json/any.h"

struct Person;

struct Singer {
  std::string type;
  int age;
};

struct Address {
  std::string country;
  std::string city;
  std::string street;
  std::vector<Person> neighbors;
};

struct Friend {
  std::string relation;
  json::Any secret;

  template<typename OutputStream>
  void Serialize(rapidjson::Writer<OutputStream>& writer) const {
    writer.StartObject();
    writer.String("relation");
    writer.String(relation);
    // writer.String("secret");
    writer.EndObject();
  }
};

struct Person {
  std::string name;
  int age;
  Address address;
  std::vector<Friend> friends;
  json::Any secret;
};

int main() {
  Friend f1{"my best friend", Singer{"rocker", 18}};
  auto& f1Secret = json::AnyCast<Singer>(f1.secret);
  Friend f2{"new friend", "little girl"};
  auto& f2Secret = json::AnyCast<std::string>(f2.secret);
  Friend f3{"third friend", 3};
  auto& f3Secret = json::AnyCast<int>(f3.secret);
  Person p2{"p2", 3, Address{"china", "shanghai", "putuo"}};
  auto& p2SecretType = p2.secret.TypeInfo();
  Address addr1{"china", "beijing", "wangjing", {p2}};
  Person p1{"p1", 4, addr1, {f1, f2, f3}, "the kind!"};
  auto& p1Secret = json::AnyCast<std::string>(p1.secret);
  std::cout << json::Serialize(f1) << std::endl;
  // TODO. 以下是伪代码，需要笔试者具体实现 // auto json = dump(p1)
  // std::cout << json << std::endl // std::cout << p1 << std::endl
  // auto pp = parse(json); // assert(p1 == pp)
  // 序列化
  // 打印序列化结果
  // 打印 Person 对象 // 反序列化
  // 反序列化的结果是对的
  return EXIT_SUCCESS;
}
