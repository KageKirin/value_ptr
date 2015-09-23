// Default way to create a new object
// Copyright (C) 2014 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

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
	template<
		typename U,
		SMART_POINTER_REQUIRES(
			std::is_same<std::decay_t<T>, std::decay_t<U>>::value and
			!std::is_array<U>::value
		)
	>
	T * operator()(U && other) const {
		static_assert(
			!std::is_polymorphic<T>::value,
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
