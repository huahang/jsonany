/**
 * @author Huahang Liu
 * @since 2020-08-15
 */

#pragma once

#ifndef JSON_ANY_H
#define JSON_ANY_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif  // RAPIDJSON_HAS_STDSTRING

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <stdint.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

namespace json {

template <typename T>
std::string Dump(T& obj);

template <typename T>
void Parse(T& obj, const std::string& json);

class Any final {
 public:
  Any() : holder(nullptr) { jsonValue.SetNull(); }

  Any(const Any& any) : Any() {
    if (any.holder != nullptr) {
      holder = any.holder->Clone();
      jsonValue.SetNull();
      return;
    }
    holder = nullptr;
    this->jsonValue.CopyFrom(any.jsonValue, this->jsonDoc.GetAllocator());
  }

  template <typename T>
  Any(const std::shared_ptr<T> p) : holder(new SharedPointerHolder<T>(p)) {
    jsonValue.SetNull();
  }

  template <typename T>
  Any(const T& v) : holder(new ValueHolder<T>(v)) {
    jsonValue.SetNull();
  }

  Any(const char* s) : Any(std::string(s)) {}

  Any& operator=(const Any& any) {
    if (any.holder != nullptr) {
      holder = any.holder->Clone();
      jsonValue.SetNull();
      return *this;
    }
    holder = nullptr;
    this->jsonValue.CopyFrom(any.jsonValue, this->jsonDoc.GetAllocator());
    return *this;
  }

  template <typename T>
  Any& operator=(const T& o) {
    holder = new ValueHolder<T>(o);
    jsonValue.SetNull();
    return *this;
  }

  template <typename T>
  Any& operator=(const std::shared_ptr<T> p) {
    holder = new SharedPointerHolder<T>(p);
    jsonValue.SetNull();
    return *this;
  }

  const std::type_info& TypeInfo() const {
    return holder != nullptr ? holder->TypeInfo() : typeid(void);
  }

  template <typename T>
  void Cast(T& t) {
    jsonToHolder<T>();
    if (holder == nullptr) {
      throw std::bad_cast();
    }
    const auto& holderType = TypeInfo();
    const auto& valueType = typeid(T);
    if (holderType != valueType) {
      throw std::bad_cast();
    }
    holder->CopyOut(&t);
  }

  template <typename AllocatorType>
  void Dump(rapidjson::Value& v, AllocatorType& alloc) {
    if (jsonValue.IsNull() && holder != nullptr) {
      holder->Dump(*this);
    }
    v.CopyFrom(this->jsonValue, alloc);
  }

  void Parse(const rapidjson::Value& v) {
    jsonValue.CopyFrom(v, jsonDoc.GetAllocator());
    holder = nullptr;
  }

 private:
  template <typename T>
  bool jsonToHolder() {
    bool result = false;
    if (holder == nullptr && (!jsonValue.IsNull())) {
      try {
        holder = new SharedPointerHolder<T>();
        dynamic_cast<SharedPointerHolder<T>*>(holder)->value->Parse(
            this->jsonValue  //
        );
      } catch (const std::invalid_argument&) {
        holder = nullptr;
        result = false;
        return result;
      }
      result = true;
    }
    return result;
  }

 private:
  class HolderInterface {
   public:
    virtual HolderInterface* Clone() const = 0;
    virtual const std::type_info& TypeInfo() const = 0;
    virtual const std::type_info& HolderTypeInfo() const = 0;
    virtual void Dump(Any& any) = 0;
    virtual void CopyOut(void*) = 0;
    virtual ~HolderInterface() {}
  };

  template <typename ValueType>
  class ValueHolder : public HolderInterface {
   public:
    // ValueType is copy constructible
    ValueHolder(                                           //
        const typename std::enable_if<                     //
            std::is_copy_constructible<ValueType>::value,  //
            ValueType>::type& v)
        : value(v) {}
    virtual HolderInterface* Clone() const { return new ValueHolder(value); };
    virtual const std::type_info& TypeInfo() const { return typeid(ValueType); }
    virtual const std::type_info& HolderTypeInfo() const {
      return typeid(ValueHolder<ValueType>);
    }
    virtual void Dump(Any& any) {
      value.Dump(any.jsonValue, any.jsonDoc.GetAllocator());
    }
    virtual void CopyOut(void* v) { *reinterpret_cast<ValueType*>(v) = value; }
    ValueType value;
  };

  template <typename ValueType>
  class SharedPointerHolder : public HolderInterface {
   public:
    // ValueType must be default constructible
    SharedPointerHolder() : value(new ValueType()) {}
    // ValueType is wrapped inside a shared pointer
    SharedPointerHolder(const std::shared_ptr<ValueType>& v) : value(v) {}
    virtual HolderInterface* Clone() const {
      return new SharedPointerHolder(value);
    };
    virtual const std::type_info& TypeInfo() const { return typeid(ValueType); }
    virtual const std::type_info& HolderTypeInfo() const {
      return typeid(SharedPointerHolder<ValueType>);
    }
    virtual void Dump(Any& any) {
      value->Dump(any.jsonValue, any.jsonDoc.GetAllocator());
    }
    virtual void CopyOut(void* v) {
      auto jsonString = json::Dump<ValueType>(*value);
      json::Parse(*reinterpret_cast<ValueType*>(v), jsonString);
    }
    std::shared_ptr<ValueType> value;
  };

  HolderInterface* holder;

 private:
  rapidjson::Document jsonDoc;
  rapidjson::Value jsonValue;
};

template <>
void Any::SharedPointerHolder<std::string>::Dump(Any&);

template <>
void Any::SharedPointerHolder<int>::Dump(Any&);

template <>
void Any::ValueHolder<std::string>::Dump(Any&);

template <>
void Any::ValueHolder<int>::Dump(Any&);

template <typename T>
typename std::enable_if<std::is_copy_constructible<T>::value, T>::type  //
AnyCast(Any& any) {
  T obj;
  any.Cast(obj);
  return obj;
}

template <typename T, typename AllocatorType>
void Dump(rapidjson::Value& array, AllocatorType& alloc, std::vector<T>& v) {
  using rapidjson::Value;
  array.SetArray();
  for (auto& item : v) {
    Value v;
    v.SetObject();
    auto object = v.GetObject();
    item.Dump(v, alloc);
    v.Set(object);
    array.PushBack(v, alloc);
  }
}

template <typename T, typename Writer>
std::string Dump(T& obj) {
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

template <typename T>
std::string Dump(T& obj) {
  return Dump<T, rapidjson::Writer<rapidjson::StringBuffer>>(obj);
}

template <typename T>
std::string DumpPretty(T& obj) {
  return Dump<T, rapidjson::PrettyWriter<rapidjson::StringBuffer>>(obj);
}

template <typename T>
std::vector<T> ParseArray(const rapidjson::Value& value) {
  using rapidjson::Value;
  std::vector<T> ret;
  if (!value.IsArray()) {
    throw std::invalid_argument("invalid value");
  }
  auto jsonArray = value.GetArray();
  auto itr = jsonArray.begin();
  auto end = jsonArray.end();
  for (; itr != end; itr++) {
    auto& v = *itr;
    T obj;
    obj.Parse(v);
    ret.push_back(obj);
  }
  return ret;
}

template <typename T>
void Parse(                  //
    T& obj,                  //
    const std::string& json  //
) {
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
  obj.Parse(v);
}

template <typename T>
typename std::enable_if<std::is_copy_constructible<T>::value, T>::type
Parse(                       //
    const std::string& json  //
) {
  T ret;
  Parse(ret, json);
  return ret;
}

}  // namespace json

#endif  // JSON_ANY_H