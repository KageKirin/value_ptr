// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This implementation was made to match std::default_delete as much as
// possible. It is used for the copy constructor and copy assignment operator of
// value_ptr.

#ifndef DEFAULT_NEW_HPP_
#define DEFAULT_NEW_HPP_

#include "requires.hpp"

#include <algorithm>
#include <memory>
#include <type_traits>

namespace smart_pointer {

template<typename T>
class default_new {
public:
	constexpr default_new() noexcept {}
	template<typename U>
	constexpr default_new(default_new<U> const &) noexcept {}
	// This is a template to delay instantiation of this function until the
	// point of use. The static_assert is only triggered if you actually try the
	// operation that causes an error (cloning a polymorphic class), rather than
	// when you instantiate the value_ptr. This also allows perfect forwarding.
	template<typename U>
	T * operator()(U && other) const {
		static_assert(
			!std::is_polymorphic<T>::value and !std::is_polymorphic<U>::value,
			"You must either specialize default_new to properly clone your polymorphic type or provide a custom cloner."
		);
		return new T(std::forward<U>(other));
	}
};
template<typename T, std::size_t n>
class default_new<T[n]> {
public:
	constexpr default_new() noexcept {}
	template<typename U, std::size_t m>
	constexpr default_new(default_new<U[m]> const &) noexcept {}
	T * operator()(T const (& other)[n]) const {
		auto result = new T[n];
		std::copy(std::begin(other), std::end(other), std::begin(result));
		return result;
	}
};

}	// namespace smart_pointer
#endif	// DEFAULT_NEW_HPP_
