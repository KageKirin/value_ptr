// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

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
