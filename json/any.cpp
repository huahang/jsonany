/**
 * @author Huahang Liu
 * @since 2020-08-15
 */

#include "any.h"

namespace json {

template <>
void Any::SharedPointerHolder<std::string>::Dump(Any& any) {
  any.jsonValue.SetString(*this->value, any.jsonDoc.GetAllocator());
}

template <>
void Any::SharedPointerHolder<int>::Dump(Any& any) {
  any.jsonValue.SetInt(*this->value);
}

template <>
void Any::ValueHolder<std::string>::Dump(Any& any) {
  any.jsonValue.SetString(this->value, any.jsonDoc.GetAllocator());
}

template <>
void Any::ValueHolder<int>::Dump(Any& any) {
  any.jsonValue.SetInt(this->value);
}

}  // namespace json
