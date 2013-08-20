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
#include <type_traits>
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
	typedef std::tuple<unique_ptr_type, Cloner, detail::empty_class> base_type;
public:
	typedef Cloner cloner_type;
	typedef typename unique_ptr_type::deleter_type deleter_type;
private:
	typedef typename std::conditional<std::is_reference<cloner_type>::value, cloner_type, cloner_type const &>::type cloner_lvalue_reference;
	typedef typename std::conditional<std::is_reference<deleter_type>::value, deleter_type, deleter_type const &>::type deleter_lvalue_reference;
	typedef typename std::remove_reference<cloner_type>::type && cloner_rvalue_reference;
	typedef typename std::remove_reference<deleter_type>::type && deleter_rvalue_reference;
public:
	typedef typename unique_ptr_type::pointer pointer;
	typedef typename unique_ptr_type::element_type element_type;
	
	constexpr value_ptr() noexcept {}
	constexpr value_ptr(std::nullptr_t) noexcept:
		base(nullptr, cloner_type(), detail::empty_class()) {
	}
	explicit value_ptr(pointer p) noexcept:
		base(unique_ptr_type(p), cloner_type(), detail::empty_class()) {
	}

	value_ptr(pointer p, cloner_lvalue_reference cloner) noexcept:
		base(unique_ptr_type(p), cloner, detail::empty_class()) {
	}
	value_ptr(pointer p, cloner_rvalue_reference cloner) noexcept:
		base(unique_ptr_type(p), std::move(cloner), detail::empty_class()) {
	}

	value_ptr(pointer p, deleter_lvalue_reference deleter) noexcept:
		base(unique_ptr_type(p, deleter), cloner_type(), detail::empty_class()) {
	}
	value_ptr(pointer p, deleter_rvalue_reference deleter) noexcept:
		base(unique_ptr_type(p, std::move(deleter)), cloner_type(), detail::empty_class()) {
	}

	value_ptr(pointer p, cloner_lvalue_reference cloner, deleter_lvalue_reference deleter) noexcept:
		base(unique_ptr_type(p, deleter), cloner, detail::empty_class()) {
	}
	value_ptr(pointer p, cloner_lvalue_reference cloner, deleter_rvalue_reference deleter) noexcept:
		base(unique_ptr_type(p, std::move(deleter)), cloner, detail::empty_class()) {
	}
	value_ptr(pointer p, cloner_rvalue_reference cloner, deleter_lvalue_reference deleter) noexcept:
		base(unique_ptr_type(p, deleter), std::move(cloner), detail::empty_class()) {
	}
	value_ptr(pointer p, cloner_rvalue_reference cloner, deleter_rvalue_reference deleter) noexcept:
		base(unique_ptr_type(p, std::move(deleter)), std::move(cloner), detail::empty_class()) {
	}

	value_ptr(value_ptr const & other):
		base(other != nullptr ? clone(*other) : nullptr, other.get_cloner(), detail::empty_class()) {
	}
	template<typename U, typename C, typename D>
	value_ptr(value_ptr<U, C, D> const & other):
		base(clone(*other), other.get_cloner(), detail::empty_class()) {
	}
	explicit value_ptr(T const & other):
		base(clone(other), cloner_type(), detail::empty_class()) {
	}

	value_ptr(value_ptr && other) noexcept:
		base(std::move(other.base)) {
	}
	template<typename U, typename C, typename D>
	value_ptr(value_ptr<U, C, D> && other) noexcept:
		base(std::move(other.base)) {
	}
	template<typename U, typename D>
	value_ptr(std::unique_ptr<U, D> && other) noexcept:
		base(std::move(other), cloner_type(), detail::empty_class()) {
	}
	template<typename U>
	value_ptr(std::auto_ptr<U> && other) noexcept:
		base(std::move(other), cloner_type(), detail::empty_class()) {
	}


	value_ptr & operator=(T const & other) {
		get_unique_ptr() = unique_ptr_type(clone(other));
		return *this;
	}
	value_ptr & operator=(value_ptr const & other) {
		operator=(*other);
		get_cloner() = other.get_cloner();
		return *this;
	}
	template<typename U, typename C, typename D>
	value_ptr & operator=(value_ptr<U, C, D> const & other) {
		operator=(*other);
		get_cloner() = other.get_cloner();
		return *this;
	}
	template<typename U, typename C, typename D>
	value_ptr & operator=(value_ptr<U, C, D> && other) noexcept {
		base = std::move(other.base);
		return *this;
	}
	template<typename U, typename D>
	value_ptr & operator=(std::unique_ptr<U, D> && other) noexcept {
		get_unique_ptr() = (std::move(other));
		get_cloner() = cloner_type();
		return *this;
	}
	value_ptr & operator=(std::nullptr_t) noexcept {
		reset();
		return *this;
	}
	
	pointer release() noexcept {
		return get_unique_ptr().release();
	}
	void reset(pointer ptr = pointer()) noexcept {
		get_unique_ptr().reset(ptr);
		get_cloner() = cloner_type();
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

	element_type & operator*() const {
		static_assert(!std::is_array<T>::value, "operator* cannot be used with array types.");
		return *get();
	}
	pointer operator->() const noexcept {
		static_assert(!std::is_array<T>::value, "operator-> cannot be used with array types.");
		return get();
	}

	element_type & operator[](std::size_t index) const {
		static_assert(std::is_array<T>::value, "operator[] can only be used with array types.");
		return get_unique_ptr()[index];
	}

private:
	unique_ptr_type const & get_unique_ptr() const noexcept {
		return std::get<0>(base);
	}
	unique_ptr_type & get_unique_ptr() noexcept {
		return std::get<0>(base);
	}
	unique_ptr_type clone(element_type const & other) const {
		return unique_ptr_type(get_cloner()(&other));
	}
	base_type base;

	template<typename U, typename C, typename D>
	friend class value_ptr;
};

namespace detail {

template<typename T, typename Cloner, typename Deleter>
class value_if_general {
public:
	typedef value_ptr<T, Cloner, Deleter> object;
};
template<typename T, typename Cloner, typename Deleter>
class value_if_general<T[], Cloner, Deleter> {
public:
	typedef value_ptr<T[], Cloner, Deleter> array;
};
template<typename T, typename Cloner, typename Deleter, std::size_t n>
class value_if_general<T[n], Cloner, Deleter> {
public:
	typedef void known_bound;
};

template<typename T>
class value_if {
public:
	typedef value_ptr<T> object;
};
template<typename T>
class value_if<T[]> {
public:
	typedef value_ptr<T[]> array;
};
template<typename T, std::size_t n>
class value_if<T[n]> {
public:
	typedef void known_bound;
};
}	// namespace detail

template<typename T, typename Cloner, typename Deleter, typename ... Args>
typename detail::value_if_general<T, Cloner, Deleter>::object
make_value_general(Cloner cloner, Deleter deleter, Args && ... args) {
	static_assert(std::is_nothrow_move_constructible<Cloner>::value, "The specified cloner's move constructor can throw.");
	static_assert(std::is_nothrow_move_constructible<Deleter>::value, "The specified deleter's move constructor can throw.");
	return value_ptr<T, Cloner, Deleter>(new T(std::forward<Args>(args)...), std::move(cloner), std::move(deleter));
}

template<typename T, typename Cloner, typename Deleter, typename ... Args>
typename detail::value_if_general<T, Cloner, Deleter>::array
make_value_general(std::size_t const n, Cloner cloner, Deleter deleter) {
	static_assert(std::is_nothrow_move_constructible<Cloner>::value, "The specified cloner's move constructor can throw.");
	static_assert(std::is_nothrow_move_constructible<Deleter>::value, "The specified deleter's move constructor can throw.");
	typedef typename std::remove_extent<T>::type U;
	return value_ptr<T, Cloner, Deleter>(new U[n](), std::move(cloner), std::move(deleter));
}

template<typename T, typename Cloner, typename Deleter, typename ... Args>
typename detail::value_if_general<T, Cloner, Deleter>::known_bound
make_value_general(std::size_t const n, Cloner cloner, Deleter deleter) = delete;

// TODO: add support for other allocators / deleters, possibly?
template<typename T, typename ... Args>
typename detail::value_if<T>::object make_value(Args && ... args) {
	return value_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
typename detail::value_if<T>::array make_value(std::size_t const n) {
	typedef typename std::remove_extent<T>::type U;
	return value_ptr<T>(new U[n]());
}

template<typename T, typename ... Args>
typename detail::value_if<T>::known_bound make_value(std::size_t const n) = delete;

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
