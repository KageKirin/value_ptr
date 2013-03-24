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
#include <boost/compressed_pair.hpp>
#include "default_new.hpp"

namespace smart_pointer {

template<typename T, typename Cloner = default_new<T>, typename Deleter = std::default_delete<T>>
class value_ptr {
public:
	typedef typename std::unique_ptr<T, Deleter> unique_ptr_type;
	typedef boost::compressed_pair<unique_ptr_type, Cloner> pair_type;
public:
	typedef typename unique_ptr_type::pointer pointer;
	typedef typename unique_ptr_type::element_type element_type;
	typedef Cloner cloner_type;
	typedef typename unique_ptr_type::deleter_type deleter_type;
	
	constexpr value_ptr() noexcept {}
	constexpr value_ptr(std::nullptr_t) noexcept:
		base(nullptr) {
	}
	explicit value_ptr(pointer p) noexcept:
		base(p) {
	}

	value_ptr(value_ptr & other):
		base(clone(*other)) {
	}
	value_ptr(value_ptr const & other):
		base(clone(*other)) {
	}
	template<typename U, typename C, typename D>
	value_ptr(value_ptr<U, C, D> const & other):
		base(clone(*other)) {
	}
	explicit value_ptr(T & other):
		base(clone(other)) {
	}
	explicit value_ptr(T const & other):
		base(clone(other)) {
	}

	value_ptr(value_ptr && other) noexcept:
		base(std::move(other.base)) {
	}
	template<typename U, typename D>
	value_ptr(std::unique_ptr<U, D> && other):
		base(std::move(other)) {
	}
	template<typename U>
	value_ptr(std::auto_ptr<U> && other):
		base(std::move(other)) {
	}
	template<typename U, typename C, typename D>
	value_ptr(value_ptr<U, C, D> && other) noexcept:
		base(std::move(other.base)) {
	}
	template<typename... Args>
	explicit value_ptr(Args && ... args) noexcept:
		base(std::forward<Args>(args)...) {
	}

	template<typename U, typename C, typename D>
	value_ptr & operator=(value_ptr<U, C, D> const & other) {
		base.first.reset(clone(other));
		return *this;
	}
	template<typename U, typename C, typename D>
	value_ptr & operator=(value_ptr<U, C, D> & other) {
		base.first.reset(clone(other));
		return *this;
	}
	template<typename... Args>
	value_ptr & operator=(Args && ... args) noexcept {
		unique_ptr_type::operator=(std::forward<Args>(args)...);
		return *this;
	}
	
	pointer release() noexcept {
		return base.first.release();
	}
	void reset(pointer ptr = pointer()) {
		base.first.reset(ptr);
	}
	void swap(value_ptr & other) noexcept {
		base.swap(other.base);
	}
	
	pointer get() const noexcept {
		return base.first.get();
	}
	deleter_type const & get_deleter() const noexcept {
		return base.first.get_deleter();
	}
	deleter_type & get_deleter() noexcept {
		return base.first.get_deleter();
	}
	explicit operator bool() const noexcept {
		return static_cast<bool>(base.first);
	}
	typename std::add_lvalue_reference<T>::type operator*() const {
		return *get();
	}
	pointer operator->() const noexcept {
		return get();
	}
private:
	pointer clone(element_type const & other) const {
		return base.second(&other);
	}
	pair_type base;
};

template<typename T, typename Deleter>
void swap(value_ptr<T, Deleter> & lhs, value_ptr<T, Deleter> & rhs) noexcept {
	lhs.swap(rhs);
}

// Rest of the file is relational operators

template<typename T1, typename D1, typename T2, typename D2>
bool operator==(value_ptr<T1, D1> const & lhs, value_ptr<T2, D2> const & rhs) {
	return lhs.get() == rhs.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator!=(value_ptr<T1, D1> const & lhs, value_ptr<T2, D2> const & rhs) {
	return lhs.get() != rhs.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator<(value_ptr<T1, D1> const & lhs, value_ptr<T2, D2> const & rhs) {
	typedef typename value_ptr<T1, D1>::pointer P1;
	typedef typename value_ptr<T2, D2>::pointer P2;
	typedef typename std::common_type<P1, P2>::type CT;
	return std::less<CT>()(lhs.get(), rhs.get());
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator<=(value_ptr<T1, D1> const & lhs, value_ptr<T2, D2> const & rhs) {
	return !(rhs < lhs);
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator>(value_ptr<T1, D1> const & lhs, value_ptr<T2, D2> const & rhs) {
	return rhs < lhs;
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator>=(value_ptr<T1, D1> const & lhs, value_ptr<T2, D2> const & rhs) {
	return !(lhs < rhs);
}


template<typename T, typename D>
bool operator==(value_ptr<T, D> const & ptr, std::nullptr_t) noexcept {
	return !ptr;
}
template<typename T, typename D>
bool operator==(std::nullptr_t, value_ptr<T, D> const & ptr) noexcept {
	return !ptr;
}

template<typename T, typename D>
bool operator!=(value_ptr<T, D> const & ptr, std::nullptr_t) noexcept {
	return static_cast<bool>(ptr);
}
template<typename T, typename D>
bool operator!=(std::nullptr_t, value_ptr<T, D> const & ptr) noexcept {
	return static_cast<bool>(ptr);
}

template<typename T, typename D>
bool operator<(value_ptr<T, D> const & ptr, std::nullptr_t) {
	return std::less<typename value_ptr<T, D>::pointer>()(ptr.get(), nullptr);
}
template<typename T, typename D>
bool operator<(std::nullptr_t, value_ptr<T, D> const & ptr) {
	return std::less<typename value_ptr<T, D>::pointer>()(nullptr, ptr.get());
}

template<typename T, typename D>
bool operator<=(value_ptr<T, D> const & ptr, std::nullptr_t) {
	return !(nullptr < ptr);
}
template<typename T, typename D>
bool operator<=(std::nullptr_t, value_ptr<T, D> const & ptr) {
	return !(ptr < nullptr);
}

template<typename T, typename D>
bool operator>(value_ptr<T, D> const & ptr, std::nullptr_t) {
	return nullptr < ptr;
}
template<typename T, typename D>
bool operator>(std::nullptr_t, value_ptr<T, D> const & ptr) {
	return ptr < nullptr;
}

template<typename T, typename D>
bool operator>=(value_ptr<T, D> const & ptr, std::nullptr_t) {
	return !(ptr < nullptr);
}
template<typename T, typename D>
bool operator>=(std::nullptr_t, value_ptr<T, D> const & ptr) {
	return !(nullptr < ptr);
}

}	// namespace smart_pointer
#endif	// VALUE_PTR_HPP_
