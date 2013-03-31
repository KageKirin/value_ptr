// A smart pointer with value semantics
// Copyright (C) 2013 David Stone
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

#ifndef VALUE_PTR_HPP_
#define VALUE_PTR_HPP_

#include <memory>
#include <tuple>
#include "default_new.hpp"

namespace smart_pointer {
namespace detail {
// This has to be an element of std::tuple to workaround gcc bug 
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=56785
// A std::tuple with only two elements that contains another std::tuple of two
// elements does not properly apply the empty base class optimization, and
// std::unique_ptr is implemented via a two-element std::tuple in gcc.
class empty_class {
};
}	// namespace detail

template<typename T, typename Cloner = default_new<T>, typename Deleter = std::default_delete<T>>
class value_ptr {
private:
	typedef typename std::unique_ptr<T, Deleter> unique_ptr_type;
	typedef std::tuple<unique_ptr_type, Cloner, detail::empty_class> pair_type;
public:
	typedef typename unique_ptr_type::pointer pointer;
	typedef typename unique_ptr_type::element_type element_type;
	typedef Cloner cloner_type;
	typedef typename unique_ptr_type::deleter_type deleter_type;
	
	constexpr value_ptr() noexcept {}
	constexpr value_ptr(std::nullptr_t) noexcept:
		base(nullptr, cloner_type(), detail::empty_class()) {
	}
	explicit value_ptr(pointer p) noexcept:
		base(unique_ptr_type(p), cloner_type(), detail::empty_class()) {
	}

	value_ptr(value_ptr & other):
		base(clone(*other), other.get_cloner(), detail::empty_class()) {
	}
	value_ptr(value_ptr const & other):
		base(clone(*other), other.get_cloner(), detail::empty_class()) {
	}
	template<typename U, typename C, typename D>
	value_ptr(value_ptr<U, C, D> const & other):
		base(clone(*other), other.get_cloner(), detail::empty_class()) {
	}
	explicit value_ptr(T & other):
		base(clone(other), cloner_type(), detail::empty_class()) {
	}
	explicit value_ptr(T const & other):
		base(clone(other), cloner_type(), detail::empty_class()) {
	}

	value_ptr(value_ptr && other) noexcept:
		base(std::move(other.base)) {
	}
	template<typename U, typename D>
	value_ptr(std::unique_ptr<U, D> && other):
		base(std::move(other), cloner_type(), detail::empty_class()) {
	}
	template<typename U>
	value_ptr(std::auto_ptr<U> && other):
		base(std::move(other), cloner_type(), detail::empty_class()) {
	}
	template<typename U, typename C, typename D>
	value_ptr(value_ptr<U, C, D> && other) noexcept:
		base(std::move(other.base)) {
	}
	template<typename... Args>
	explicit value_ptr(Args && ... args) noexcept:
		base(std::forward<Args>(args)..., cloner_type(), detail::empty_class()) {
	}

	template<typename U, typename C, typename D>
	value_ptr & operator=(value_ptr<U, C, D> const & other) {
		get_unique_ptr().reset(clone(other));
		return *this;
	}
	template<typename U, typename C, typename D>
	value_ptr & operator=(value_ptr<U, C, D> & other) {
		get_unique_ptr().reset(clone(other));
		return *this;
	}
	template<typename... Args>
	value_ptr & operator=(Args && ... args) noexcept {
		unique_ptr_type::operator=(std::forward<Args>(args)...);
		return *this;
	}
	
	pointer release() noexcept {
		return get_unique_ptr().release();
	}
	void reset(pointer ptr = pointer()) {
		get_unique_ptr().reset(ptr);
	}
	void swap(value_ptr & other) noexcept {
		base.swap(other.base);
	}
	
	pointer get() const noexcept {
		return get_unique_ptr().get();
	}
	deleter_type const & get_deleter() const noexcept {
		return get_unique_ptr().get_deleter();
	}
	deleter_type & get_deleter() noexcept {
		return get_unique_ptr().get_deleter();
	}
	cloner_type const & get_cloner() const noexcept {
		return std::get<1>(base);
	}
	cloner_type & get_cloner() noexcept {
		return std::get<1>(base);
	}
	explicit operator bool() const noexcept {
		return static_cast<bool>(get_unique_ptr());
	}
	typename std::add_lvalue_reference<T>::type operator*() const {
		return *get();
	}
	pointer operator->() const noexcept {
		return get();
	}
private:
	unique_ptr_type const & get_unique_ptr() const {
		return std::get<0>(base);
	}
	unique_ptr_type & get_unique_ptr() {
		return std::get<0>(base);
	}
	unique_ptr_type clone(element_type const & other) const {
		return get_cloner()(&other);
	}
	pair_type base;
};

template<typename T, typename Cloner, typename Deleter>
void swap(value_ptr<T, Cloner, Deleter> & lhs, value_ptr<T, Cloner, Deleter> & rhs) noexcept {
	lhs.swap(rhs);
}

// Rest of the file is relational operators

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
	typedef typename value_ptr<T1, C1, D1>::pointer P1;
	typedef typename value_ptr<T2, C2, D2>::pointer P2;
	typedef typename std::common_type<P1, P2>::type CT;
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
#endif	// VALUE_PTR_HPP_
