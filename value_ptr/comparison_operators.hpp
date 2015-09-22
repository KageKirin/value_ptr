// A smart pointer with value semantics
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

#ifndef VALUE_PTR_COMPARISON_OPERATORS_HPP_
#define VALUE_PTR_COMPARISON_OPERATORS_HPP_

#include "class.hpp"

namespace smart_pointer {

template<typename T1, typename C1, typename D1, typename T2, typename C2, typename D2>
bool operator==(value_ptr<T1, C1, D1> const & lhs, value_ptr<T2, C2, D2> const & rhs) {
	return lhs.get() == rhs.get();
}

template<typename T1, typename C1, typename D1, typename T2, typename C2, typename D2>
bool operator!=(value_ptr<T1, C1, D1> const & lhs, value_ptr<T2, C2, D2> const & rhs) {
	return lhs.get() != rhs.get();
}

template<typename T1, typename C1, typename D1, typename T2, typename C2, typename D2>
bool operator<(value_ptr<T1, C1, D1> const & lhs, value_ptr<T2, C2, D2> const & rhs) {
	using P1 = typename value_ptr<T1, C1, D1>::pointer;
	using P2 = typename value_ptr<T2, C2, D2>::pointer;
	using CT = std::common_type_t<P1, P2>;
	return std::less<CT>()(lhs.get(), rhs.get());
}

template<typename T1, typename C1, typename D1, typename T2, typename C2, typename D2>
bool operator<=(value_ptr<T1, C1, D1> const & lhs, value_ptr<T2, C2, D2> const & rhs) {
	return !(rhs < lhs);
}

template<typename T1, typename C1, typename D1, typename T2, typename C2, typename D2>
bool operator>(value_ptr<T1, C1, D1> const & lhs, value_ptr<T2, C2, D2> const & rhs) {
	return rhs < lhs;
}

template<typename T1, typename C1, typename D1, typename T2, typename C2, typename D2>
bool operator>=(value_ptr<T1, C1, D1> const & lhs, value_ptr<T2, C2, D2> const & rhs) {
	return !(lhs < rhs);
}


template<typename T, typename C, typename D>
bool operator==(value_ptr<T, C, D> const & ptr, std::nullptr_t) noexcept {
	return !ptr;
}
template<typename T, typename C, typename D>
bool operator==(std::nullptr_t, value_ptr<T, C, D> const & ptr) noexcept {
	return !ptr;
}

template<typename T, typename C, typename D>
bool operator!=(value_ptr<T, C, D> const & ptr, std::nullptr_t) noexcept {
	return static_cast<bool>(ptr);
}
template<typename T, typename C, typename D>
bool operator!=(std::nullptr_t, value_ptr<T, C, D> const & ptr) noexcept {
	return static_cast<bool>(ptr);
}

template<typename T, typename C, typename D>
bool operator<(value_ptr<T, C, D> const & ptr, std::nullptr_t) {
	return std::less<typename value_ptr<T, C, D>::pointer>()(ptr.get(), nullptr);
}
template<typename T, typename C, typename D>
bool operator<(std::nullptr_t, value_ptr<T, C, D> const & ptr) {
	return std::less<typename value_ptr<T, C, D>::pointer>()(nullptr, ptr.get());
}

template<typename T, typename C, typename D>
bool operator<=(value_ptr<T, C, D> const & ptr, std::nullptr_t) {
	return !(nullptr < ptr);
}
template<typename T, typename C, typename D>
bool operator<=(std::nullptr_t, value_ptr<T, C, D> const & ptr) {
	return !(ptr < nullptr);
}

template<typename T, typename C, typename D>
bool operator>(value_ptr<T, C, D> const & ptr, std::nullptr_t) {
	return nullptr < ptr;
}
template<typename T, typename C, typename D>
bool operator>(std::nullptr_t, value_ptr<T, C, D> const & ptr) {
	return ptr < nullptr;
}

template<typename T, typename C, typename D>
bool operator>=(value_ptr<T, C, D> const & ptr, std::nullptr_t) {
	return !(ptr < nullptr);
}
template<typename T, typename C, typename D>
bool operator>=(std::nullptr_t, value_ptr<T, C, D> const & ptr) {
	return !(nullptr < ptr);
}

}	// namespace smart_pointer
#endif	// VALUE_PTR_COMPARISON_OPERATORS_HPP_
