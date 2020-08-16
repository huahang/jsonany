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

//#ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
//#define BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS 1
//#endif // BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <boost/hana.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace json {

class Any;

template <typename ValueType>
const ValueType& AnyCast(const Any& a);

template <typename T>
void Serialize(const T& o, rapidjson::Writer<rapidjson::StringBuffer>& w);

class Any final {
 public:
  // constructors
  Any() : holder(nullptr) {}

  Any(const Any& a) : Any() {
    if (a.holder == nullptr) {
      this->holder = nullptr;
      return;
    }
    this->holder = a.holder->Clone();
  }

  Any(Any&& a) : Any() { this->Swap(a); }

  // destructor
  ~Any() {
    if (this->holder != nullptr) {
      delete this->holder;
    }
  }

  // assignment operator
  Any& operator=(const Any& a) {
    Any copy(a);
    return this->Swap(copy);
  }

  // constructors from value
  template <typename ValueType>
  Any(const ValueType& v) : holder(new ValueHolder<ValueType>(v)) {}

  Any(const char* str)
      : holder(new ValueHolder<std::string>(std::string(str))) {}

  // assignment from value
  template <typename ValueType>
  Any& operator=(const ValueType& v) {
    Any a(v);
    return this->Swap(a);
  }

  // get type info
  const std::type_info& TypeInfo() const {
    return holder != nullptr ? holder->TypeInfo() : typeid(void);
  }

  // is null
  const bool IsNull() const { return holder == nullptr; }

  // get value pointer
  template <typename ValueType>
  const ValueType* ValuePointer() const {
    const auto& holderType = TypeInfo();
    const auto& valueType = typeid(ValueType);
    if (holderType != valueType) {
      return nullptr;
    }
    return &static_cast<ValueHolder<ValueType>*>(holder)->value;
  }

  // swap
  Any& Swap(Any& a) {
    std::swap(holder, a.holder);
    return *this;
  }

 private:
  class HolderInterface {
   public:
    virtual HolderInterface* Clone() const = 0;
    virtual const std::type_info& TypeInfo() const = 0;
    virtual ~HolderInterface() {}
  };

  template <typename ValueType>
  class ValueHolder : public HolderInterface {
   public:
    ValueHolder(const ValueType& v) : value(v) {}

    ValueHolder(ValueType&& v) : value(v) {}

    virtual HolderInterface* Clone() const { return new ValueHolder(value); };

    virtual const std::type_info& TypeInfo() const { return typeid(ValueType); }

    //        virtual auto Serialize() const -> decltype(ValueType()) {
    //          return value;
    //        }

    const ValueType value;
  };

  HolderInterface* holder;
};

template <typename ValueType>
const ValueType& AnyCast(const Any& a) {
  const ValueType* result = a.ValuePointer<ValueType>();
  if (result != nullptr) {
    return *result;
  }
  throw std::bad_cast();
}

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
    } else if (typeInfo == typeid(json::Any)) {
      w.String("(json::Any)");
    } else {
      w.String("(unknown)");
    }
  });
  w.EndObject();
}

}  // namespace json

#endif  // JSON_ANY_H