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

namespace smart_pointer {

template<typename T, typename Deleter = std::default_delete<T>>
class value_ptr : private std::unique_ptr<T, Deleter> {
		typedef typename std::unique_ptr<T, Deleter> base;
	public:
		using base::pointer;
		using base::element_type;
		using base::deleter_type;
		
		// gcc does not yet support inheriting constructors, so I have to
		// manually define these.
		constexpr value_ptr() noexcept = default;
		constexpr value_ptr(std::nullptr_t) noexcept:
			base(nullptr) {
		}
		value_ptr(value_ptr & other):
			base(new T(*other)) {
		}
		value_ptr(value_ptr const & other):
			base(new T(*other)) {
		}
		template<typename U, typename E>
		value_ptr(value_ptr<U, E> const & other):
			base(new T(*other)) {
		}
		explicit value_ptr(T & other):
			base(new T(other)) {
		}
		explicit value_ptr(T const & other):
			base(new T(other)) {
		}
		value_ptr(value_ptr && other):
			base(std::move(other)) {
		}
		template<typename U, typename E>
		value_ptr(value_ptr<U, E> && other):
			base(std::move(other)) {
		}
		template<typename... Args>
		explicit value_ptr(Args && ... args) noexcept:
			base(std::forward<Args>(args)...) {
		}
		template<typename U, typename E>
		value_ptr & operator=(value_ptr<U, E> const & other) {
			base::reset(new T(*other));
			return *this;
		}
		using base::operator=;

		using base::release;
		using base::reset;
		using base::swap;
		
		using base::get;
		using base::get_deleter;
		using base::operator bool;

		using base::operator*;
		using base::operator->;
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
