/**
 * @author Huahang Liu
 * @since 2020-08-17
 */

#include <json/any.h>

#include <gtest/gtest.h>

struct A
{
  std::string type;
  int age;

  template <typename AllocatorType>
  void Dump(rapidjson::Value &v, AllocatorType &alloc)
  {
    v.SetObject();
    v.AddMember("type", type, alloc);
    v.AddMember("age", age, alloc);
  }

  void Parse(const rapidjson::Value &v)
  {
    using rapidjson::Value;
    if (!v.HasMember("type"))
    {
      throw std::invalid_argument("No 'type' in JSON");
    }
    const Value &typeValue = v["type"];
    if (!typeValue.IsString())
    {
      throw std::invalid_argument("Invalid 'type' in JSON");
    }
    this->type = typeValue.GetString();
    if (!v.HasMember("age"))
    {
      throw std::invalid_argument("No 'age' in JSON");
    }
    const Value &ageValue = v["age"];
    if (!ageValue.IsInt())
    {
      throw std::invalid_argument("Invalid 'age' in JSON");
    }
    this->age = ageValue.GetInt();
  }
};

TEST(JsonAnyTest, TestDumpAndParse)
{
  A a1{"Ana", 18}, a2;
  json::Any a3;
  std::string json1, json2, json3;
  json1 = json::Dump(a1);
  a2 = json::Parse<A>(json1);
  a3 = json::Parse<json::Any>(json1);
  json2 = json::Dump(a2);
  json3 = json::Dump(a3);
  EXPECT_EQ(json1, "{\"type\":\"Ana\",\"age\":18}");
  EXPECT_EQ(json1, json2);
  EXPECT_EQ(json1, json3);
  a2 = a3.Cast<A>();
  json2 = json::Dump(a2);
  json3 = json::Dump(a3);
  EXPECT_EQ(json1, json2);
  EXPECT_EQ(json1, json3);
}
