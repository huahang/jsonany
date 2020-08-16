/**
 * @author Huahang Liu
 * @since 2020-08-15
 */

#pragma once

#ifndef JSON_ANY_H
#define JSON_ANY_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif // RAPIDJSON_HAS_STDSTRING

#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace json {

class Any final {
public:
  Any() { jsonValue.SetNull(); }

  Any(const Any &any) {
    this->jsonValue.CopyFrom(any.jsonValue, this->jsonDoc.GetAllocator());
  }

  Any(const int i) { jsonValue.SetInt(i); }

  Any(const char *str) { jsonValue.SetString(str, jsonDoc.GetAllocator()); }

  Any(const std::string &str) {
    jsonValue.SetString(str, jsonDoc.GetAllocator());
  }

  template <typename T> Any(const T &t) {
    t.Dump(jsonValue, jsonDoc.GetAllocator());
  }

  template <typename T> T Cast() const {
    T ret;
    try {
      ret.Parse(this->jsonValue);
    } catch (const std::invalid_argument &e) {
      throw std::bad_cast();
    }
    return ret;
  }

  template <typename AllocatorType>
  void Dump(rapidjson::Value &v, AllocatorType &alloc) const {
    v.CopyFrom(this->jsonValue, alloc);
  }

  void Parse(const rapidjson::Value &v) {
    jsonValue.CopyFrom(v, jsonDoc.GetAllocator());
  }

private:
  rapidjson::Document jsonDoc;
  rapidjson::Value jsonValue;
};

template <typename T> T AnyCast(const Any &any) { return any.Cast<T>(); }

template <typename T, typename AllocatorType>
void Dump(rapidjson::Value &array, AllocatorType &alloc,
          const std::vector<T> &v) {
  using rapidjson::Value;
  array.SetArray();
  for (const T &item : v) {
    Value v;
    v.SetObject();
    auto object = v.GetObject();
    item.Dump(v, alloc);
    v.Set(object);
    array.PushBack(v, alloc);
  }
}

template <typename T, typename Writer> std::string Dump(const T &obj) {
  using rapidjson::Document;
  using rapidjson::StringBuffer;
  using rapidjson::Value;
  StringBuffer sb;
  Writer w(sb);
  Document doc;
  Value v;
  obj.Dump(v, doc.GetAllocator());
  doc.SetObject();
  doc.Set(v.GetObject());
  doc.Accept(w);
  return sb.GetString();
}

template <typename T> std::string Dump(const T &obj) {
  return Dump<T, rapidjson::Writer<rapidjson::StringBuffer>>(obj);
}

template <typename T> std::string DumpPretty(const T &obj) {
  return Dump<T, rapidjson::PrettyWriter<rapidjson::StringBuffer>>(obj);
}

template <typename T> std::vector<T> ParseArray(const rapidjson::Value &value) {
  using rapidjson::Value;
  std::vector<T> ret;
  if (!value.IsArray()) {
    throw std::invalid_argument("invalid value");
  }
  auto jsonArray = value.GetArray();
  auto itr = jsonArray.begin();
  auto end = jsonArray.end();
  for (; itr != end; itr++) {
    auto &v = *itr;
    T obj;
    obj.Parse(v);
    ret.push_back(obj);
  }
  return ret;
}

template <typename T> T Parse(const std::string &json) {
  using rapidjson::Document;
  using rapidjson::Value;
  Document doc;
  doc.Parse(json);
  if (!doc.IsObject()) {
    std::string err = "Invalid JSON: " + json;
    throw std::invalid_argument(err);
  }
  auto object = doc.GetObject();
  Value v;
  v.SetObject();
  v.Set(object);
  T ret;
  ret.Parse(v);
  return ret;
}

} // namespace json

#endif // JSON_ANY_H