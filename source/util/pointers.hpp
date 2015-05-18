#ifndef _UTIL__POINTERS_HPP_
#define _UTIL__POINTERS_HPP_

#include <memory>

namespace util {

template <class T>
class MakeSmartPointers {
public:
	using ptr = T*;
	using sptr = std::shared_ptr<T>;
	using wptr = std::weak_ptr<T>;
	using uptr = std::unique_ptr<T>;
};

} // namespace util

#endif // _UTIL__POINTERS_HPP_
