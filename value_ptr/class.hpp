// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <memory>
#include <tuple>
#include <type_traits>
#include "default_new.hpp"
#include "requires.hpp"

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
	using unique_ptr_type = std::unique_ptr<T, Deleter>;
	using base_type = std::tuple<unique_ptr_type, Cloner, detail::empty_class>;
public:
	using cloner_type = Cloner;
	using deleter_type = typename unique_ptr_type::deleter_type;
private:
	using cloner_lvalue_reference = std::conditional_t<std::is_reference<cloner_type>::value, cloner_type, cloner_type const &>;
	using deleter_lvalue_reference = std::conditional_t<std::is_reference<deleter_type>::value, deleter_type, deleter_type const &>;
	using cloner_rvalue_reference = std::remove_reference_t<cloner_type> &&;
	using deleter_rvalue_reference = std::remove_reference_t<deleter_type> &&;
public:
	using pointer = typename unique_ptr_type::pointer;
	using element_type = typename unique_ptr_type::element_type;
	
	template<typename C, typename D, SMART_POINTER_REQUIRES(std::is_convertible<C, cloner_type>::value and std::is_convertible<D, deleter_type>::value)>
	value_ptr(pointer p, C && cloner, D && deleter) noexcept:
		base(unique_ptr_type(p, std::forward<D>(deleter)), std::forward<C>(cloner), detail::empty_class()) {
	}

	template<typename C, SMART_POINTER_REQUIRES(std::is_convertible<C, cloner_type>::value)>
	value_ptr(pointer p, C && cloner) noexcept:
		base(unique_ptr_type(p), std::forward<Cloner>(cloner), detail::empty_class()) {
	}

	template<typename D, SMART_POINTER_REQUIRES(std::is_convertible<D, deleter_type>::value)>
	value_ptr(pointer p, D && deleter) noexcept:
		value_ptr(p, cloner_type{}, std::forward<D>(deleter)) {
	}


	constexpr value_ptr(std::nullptr_t = nullptr) noexcept:
		value_ptr(nullptr, cloner_type{}) {
	}
	explicit value_ptr(pointer p) noexcept:
		value_ptr(p, cloner_type{}) {
	}

	value_ptr(value_ptr const & other):
		value_ptr(copy_construct{}, other) {
	}
	template<typename U, typename C, typename D>
	value_ptr(value_ptr<U, C, D> const & other):
		value_ptr(copy_construct{}, other) {
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
		base(std::move(other), cloner_type{}, detail::empty_class()) {
	}
	
	template<typename U, SMART_POINTER_REQUIRES(std::is_convertible<U, element_type>::value)>
	value_ptr(U && other):
		value_ptr(nullptr) {
		get_unique_ptr().reset(clone(std::forward<U>(other)));
	}


	value_ptr & operator=(value_ptr const & other) {
		assign(other);
		return *this;
	}
	template<typename U, typename C, typename D>
	value_ptr & operator=(value_ptr<U, C, D> const & other) {
		assign(other);
		return *this;
	}
	template<typename U, typename C, typename D>
	value_ptr & operator=(value_ptr<U, C, D> && other) noexcept {
		base = std::move(other.base);
		return *this;
	}
	template<typename U, typename D>
	value_ptr & operator=(std::unique_ptr<U, D> && other) noexcept {
		get_unique_ptr() = std::move(other);
		get_cloner() = cloner_type{};
		return *this;
	}
	value_ptr & operator=(std::nullptr_t) noexcept {
		reset();
		return *this;
	}
	
	// I am not sure if I should keep this around. It has the surprising effect
	// of constructing the new value and destructing the old value, rather than
	// assigning.
	template<typename U, SMART_POINTER_REQUIRES(std::is_convertible<U, element_type>::value)>
	value_ptr & operator=(U && other) {
		get_unique_ptr() = unique_ptr_type(clone(std::forward<U>(other)));
		get_cloner() = cloner_type{};
		return *this;
	}

	pointer release() noexcept {
		return get_unique_ptr().release();
	}
	void reset(pointer ptr = pointer()) noexcept {
		get_unique_ptr().reset(ptr);
		get_cloner() = cloner_type{};
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
	enum class copy_construct {};
	template<typename U, typename C, typename D>
	value_ptr(copy_construct, value_ptr<U, C, D> const & other):
		value_ptr(other != nullptr ? clone(*other) : nullptr, other.get_cloner(), other.get_deleter()) {
		static_assert(noexcept(other.get_cloner()) and noexcept(other.get_deleter()), "Must be noexcept.");
	}
	
	template<typename U, typename C, typename D>
	void assign(value_ptr<U, C, D> const & other) {
		if (other) {
			get_unique_ptr() = unique_ptr_type(clone(*other), other.get_deleter());
		}
		get_cloner() = other.get_cloner();
	}

	unique_ptr_type const & get_unique_ptr() const noexcept {
		return std::get<0>(base);
	}
	unique_ptr_type & get_unique_ptr() noexcept {
		return std::get<0>(base);
	}
	template<typename U>
	auto clone(U && other) const {
		return get_cloner()(std::forward<U>(other));
	}
	base_type base;

	template<typename U, typename C, typename D>
	friend class value_ptr;
};


}	// namespace smart_pointer
