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

#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace json {

class Any;

template <typename ValueType>
const ValueType& AnyCast(const Any& a);

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

  // serialization
  template <typename OutputStream>
  void Serialize(rapidjson::Writer<OutputStream>& writer) const {
    if (this->TypeInfo() == typeid(int)) {
      writer.Int(AnyCast<int>(*this));
      return;
    }
    if (this->TypeInfo() == typeid(int64_t)) {
      writer.Int64(AnyCast<int64_t>(*this));
      return;
    }

    writer.StartObject();
    writer.EndObject();
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
std::string Serialize(const T& o) {
  using rapidjson::PrettyWriter;
  using rapidjson::StringBuffer;
  StringBuffer sb;
  PrettyWriter<StringBuffer> writer(sb);
  o.Serialize(writer);
  return sb.GetString();
}

}  // namespace json

#endif  // JSON_ANY_H