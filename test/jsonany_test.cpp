/**
 * @author Huahang Liu
 * @since 2020-08-17
 */

#include <gtest/gtest.h>
#include <json/any.h>

#include <memory>

#include "test_structs.h"

template <typename T>
void doTestOnStruct(const char* json) {
  T a1, a2;
  std::shared_ptr<T> a3(new T);
  json::Parse(a1, json);
  json::Any a4, a5;
  std::string json1, json2, json3, json4, json5;
  json1 = json::Dump(a1);
  EXPECT_EQ(json1, json);
  json::Parse(a2, json1);
  json2 = json::Dump(a2);
  EXPECT_EQ(json1, json2);
  json::Parse(a4, json1);
  json4 = json::Dump(a4);
  EXPECT_EQ(json1, json4);
  a4.Cast<T>(*a3);
  json3 = json::Dump(*a3);
  EXPECT_EQ(json1, json3);
  json4 = json::Dump(a4);
  EXPECT_EQ(json1, json4);
  a5 = a3;
  json5 = json::Dump(a5);
  EXPECT_EQ(json1, json5);
}

TEST(JsonAnyTest, TestNonCopyable) {
  doTestOnStruct<NonCopyable>("{\"name\":\"Ana\"}");
}

TEST(JsonAnyTest, TestSinger) {
  doTestOnStruct<Singer>("{\"type\":\"rapper\",\"age\":18}");
}

TEST(JsonAnyTest, TestPersion1) {
  static const char* json =
      "{\"name\":\"p1\",\"age\":4,\"address\":{\"country\":\"china\",\"city\":"
      "\"beijing\",\"street\":\"wangjing\",\"neighbors\":[{\"name\":\"p2\","
      "\"age\":3,\"address\":{\"country\":\"china\",\"city\":\"shanghai\","
      "\"street\":\"putuo\",\"neighbors\":[]},\"friends\":[],\"secret\":null}]}"
      ",\"friends\":[{\"relation\":\"my best "
      "friend\",\"secret\":{\"type\":\"rocker\",\"age\":18}},{\"relation\":"
      "\"new friend\",\"secret\":\"little girl\"},{\"relation\":\"third "
      "friend\",\"secret\":3}],\"secret\":\"the kind!\"}";
  doTestOnStruct<Person>(json);
}

TEST(JsonAnyTest, TestPersion2) {
  static const char* json =
      "{\"name\":\"p2\",\"age\":3,\"address\":{\"country\":\"china\",\"city\":"
      "\"shanghai\",\"street\":\"putuo\",\"neighbors\":[]},\"friends\":[],"
      "\"secret\":null}";
  doTestOnStruct<Person>(json);
}

TEST(JsonAnyTest, TestBand) {
  static const char* json =
      "{\"singers\":[{\"type\":\"rapper\",\"age\":16},{\"type\":\"rocker\","
      "\"age\":18}]}";
  doTestOnStruct<Band>(json);
}

TEST(JsonAnyTest, TestFriend1) {
  static const char* json =
      "{\"relation\":\"my best "
      "friend\",\"secret\":{\"type\":\"rocker\",\"age\":18}}";
  doTestOnStruct<Friend>(json);
}

TEST(JsonAnyTest, TestFriend2) {
  static const char* json =
      "{\"relation\":\"new friend\",\"secret\":\"little girl\"}";
  doTestOnStruct<Friend>(json);
}

TEST(JsonAnyTest, TestFriend3) {
  static const char* json = "{\"relation\":\"third friend\",\"secret\":3}";
  doTestOnStruct<Friend>(json);
}

TEST(JsonAnyTest, TestAddress) {
  static const char* json =
      "{\"country\":\"china\",\"city\":\"beijing\",\"street\":\"wangjing\","
      "\"neighbors\":[{\"name\":\"p2\",\"age\":3,\"address\":{\"country\":"
      "\"china\",\"city\":\"shanghai\",\"street\":\"putuo\",\"neighbors\":[]},"
      "\"friends\":[],\"secret\":null}]}";
  doTestOnStruct<Address>(json);
}

TEST(JsonAnyTest, TestAll) {
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
    Band bandCasted;
    any.Cast(bandCasted);
  } catch (const std::bad_cast&) {
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
  EXPECT_EQ(json, json2);
  EXPECT_EQ(json, json3);
}
