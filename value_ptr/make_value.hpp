// Factory function to create a value_ptr
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

// The implementation of unique_ptr was referenced from
// en.cppreference.com/w/cpp/memory/unique_ptr

#ifndef VALUE_PTR_MAKE_VALUE_HPP_
#define VALUE_PTR_MAKE_VALUE_HPP_

#include "class.hpp"

namespace smart_pointer {
namespace detail {

template<typename T>
class value_if {
public:
	using object = value_ptr<T>;
};
template<typename T>
class value_if<T[]> {
public:
	using array = value_ptr<T[]>;
};
template<typename T, std::size_t n>
class value_if<T[n]> {
public:
	using known_bound = void;
};

template<typename T, typename Cloner, typename Deleter>
class value_if_general {
public:
	using object = value_ptr<T, Cloner, Deleter>;
};
template<typename T, typename Cloner, typename Deleter>
class value_if_general<T[], Cloner, Deleter> {
public:
	using array = value_ptr<T[], Cloner, Deleter>;
};

}	// namespace detail

template<typename T, typename ... Args>
typename detail::value_if<T>::object make_value(Args && ... args) {
	return value_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
typename detail::value_if<T>::array make_value(std::size_t const n) {
	using U = std::remove_extent_t<T>;
	return value_ptr<T>(new U[n]());
}

template<typename T, typename ... Args>
typename detail::value_if<T>::known_bound make_value(Args && ...) = delete;


template<typename T, typename Cloner, typename Deleter, typename ... Args>
typename detail::value_if_general<T, Cloner, Deleter>::object
make_value_general(Cloner && cloner, Deleter && deleter, Args && ... args) {
	static_assert(std::is_nothrow_move_constructible<Cloner>::value, "The specified cloner's move constructor can throw.");
	static_assert(std::is_nothrow_move_constructible<Deleter>::value, "The specified deleter's move constructor can throw.");
	return value_ptr<T, Cloner, Deleter>(new T(std::forward<Args>(args)...), std::forward<Cloner>(cloner), std::forward<Deleter>(deleter));
}

template<typename T, typename Cloner, typename Deleter, typename ... Args>
typename detail::value_if_general<T, Cloner, Deleter>::array
make_value_general(std::size_t const n, Cloner && cloner, Deleter && deleter) {
	static_assert(std::is_nothrow_move_constructible<Cloner>::value, "The specified cloner's move constructor can throw.");
	static_assert(std::is_nothrow_move_constructible<Deleter>::value, "The specified deleter's move constructor can throw.");
	using U = std::remove_extent_t<T>;
	return value_ptr<T, Cloner, Deleter>(new U[n](), std::forward<Cloner>(cloner), std::forward<Deleter>(deleter));
}

template<typename T, typename ... Args>
typename detail::value_if<T>::known_bound make_value_general(Args && ...) = delete;

}	// namespace smart_pointer
#endif	// VALUE_PTR_MAKE_VALUE_HPP_
