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

typedef rapidjson::Writer<rapidjson::StringBuffer> Writer;

class Any;

template <typename ValueType> const ValueType &AnyCast(const Any &a);

template <typename T> auto Dump(const T &o, Writer &w) -> decltype(o.Dump(w));

template <typename T> void Dump(const std::vector<T> &, Writer &);

template <typename T>
std::enable_if_t<boost::hana::Struct<T>::value, void> Dump(const T &, Writer &);

class Any final {
public:
  // constructors
  Any() : holder(nullptr) {}

  Any(const Any &a) : Any() {
    if (a.holder == nullptr) {
      this->holder = nullptr;
      return;
    }
    this->holder = a.holder->Clone();
  }

  Any(Any &&a) : Any() { this->Swap(a); }

  // destructor
  ~Any() {
    if (this->holder != nullptr) {
      delete this->holder;
    }
  }

  // assignment operator
  Any &operator=(const Any &a) {
    Any copy(a);
    return this->Swap(copy);
  }

  // constructors from value
  template <typename ValueType>
  Any(const ValueType &v) : holder(new ValueHolder<ValueType>(v)) {}

  Any(const char *str)
      : holder(new ValueHolder<std::string>(std::string(str))) {}

  // assignment from value
  template <typename ValueType> Any &operator=(const ValueType &v) {
    Any a(v);
    return this->Swap(a);
  }

  // get type info
  const std::type_info &TypeInfo() const {
    return holder != nullptr ? holder->TypeInfo() : typeid(void);
  }

  // is null
  const bool IsNull() const { return holder == nullptr; }

  // get value pointer
  template <typename ValueType> const ValueType *ValuePointer() const {
    const auto &holderType = TypeInfo();
    const auto &valueType = typeid(ValueType);
    if (holderType != valueType) {
      return nullptr;
    }
    return &static_cast<ValueHolder<ValueType> *>(holder)->value;
  }

  // swap
  Any &Swap(Any &a) {
    std::swap(holder, a.holder);
    return *this;
  }

  // serialization
  void Dump(Writer &w) const { holder->Dump(w); }

private:
  class HolderInterface {
  public:
    virtual HolderInterface *Clone() const = 0;
    virtual const std::type_info &TypeInfo() const = 0;
    virtual void Dump(Writer &) const = 0;
    virtual ~HolderInterface() {}
  };

  template <typename ValueType> class ValueHolder : public HolderInterface {
  public:
    ValueHolder(const ValueType &v) : value(v) {}

    virtual HolderInterface *Clone() const { return new ValueHolder(value); };

    virtual const std::type_info &TypeInfo() const { return typeid(ValueType); }

    virtual void Dump(Writer &w) const {
      w.String("(ValueHolder)");
      // json::Dump(value, w);
    };

    const ValueType value;
  };

  HolderInterface *holder;
};

template <typename ValueType> const ValueType &AnyCast(const Any &a) {
  const ValueType *result = a.ValuePointer<ValueType>();
  if (result != nullptr) {
    return *result;
  }
  throw std::bad_cast();
}

inline void Dump(const Any &any, Writer &w) { any.Dump(w); }

inline void Dump(const bool b, Writer &w) { w.Bool(b); }

inline void Dump(const int i, Writer &w) { w.Int(i); }

inline void Dump(const int64_t i, Writer &w) { w.Int64(i); }

inline void Dump(const std::string &s, Writer &w) { w.String(s); }

template <typename T> auto Dump(const T &o, Writer &w) -> decltype(o.Dump(w)) {
  o.Dump(w);
}

template <typename T>
std::enable_if_t<boost::hana::Struct<T>::value, void> Dump(const T &o,
                                                           Writer &w) {
  namespace hana = boost::hana;
  w.StartObject();
  boost::hana::for_each(boost::hana::keys(o), [&](auto name) {
    auto const &member = boost::hana::at_key(o, name);
    w.String(boost::hana::to<char const *>(name));
    Dump(member, w);
  });
  w.EndObject();
}

template <typename T> void Dump(const std::vector<T> &array, Writer &w) {
  w.StartArray();
  for (const auto &item : array) {
    Dump(item, w);
  }
  w.EndArray();
}

// template <typename T> void Dump(const T & o, Writer &w) {
//  w.String("(Unknown)");
//}

template <typename T>
std::enable_if_t<boost::hana::Struct<T>::value, std::string> Dump(const T &o) {
  rapidjson::StringBuffer sb;
  Writer w(sb);
  Dump(o, w);
  return sb.GetString();
}

} // namespace json

#endif // JSON_ANY_H