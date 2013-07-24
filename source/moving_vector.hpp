// A vector-like class that can take advantage of cheap moves
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

#ifndef VALUE_PTR_MOVING_VECTOR_HPP_
#define VALUE_PTR_MOVING_VECTOR_HPP_

#include <vector>
#include "value_ptr.hpp"

namespace smart_pointer {

template<typename T, typename Allocator = std::allocator<T>>
class moving_vector {
private:
	using element_type = value_ptr<T>;
	using container_type = std::vector<element_type>;
	template<typename U>
	class iterator_base {
	public:
		using value_type = U;
		using difference_type = std::ptrdiff_t;
		using pointer = U *;
		using reference = U &;
		using iterator_category = std::random_access_iterator_tag;
		constexpr iterator_base() noexcept = default;
		reference operator*() const {
			return **ptr;
		}
		pointer operator->() const {
			return & this->operator*();
		}
		iterator_base & operator++() {
			++ptr;
			return *this;
		}
		iterator_base operator++(int) {
			auto & self = *this;
			operator++();
			return self;
		}
		iterator_base & operator--() {
			--ptr;
			return *this;
		}
		iterator_base operator--(int) {
			auto & self = *this;
			operator--();
			return self;
		}
		template<typename Integer>
		iterator_base & operator+=(Integer integer) {
			ptr += integer;
			return *this;
		}
		template<typename Integer>
		iterator_base & operator-=(Integer integer) {
			ptr -= integer;
			return *this;
		}
		template<typename Integer>
		reference operator[](Integer const index) {
			return ptr[index];
		}
		constexpr operator iterator_base<U const> () noexcept {
			return iterator_base<U const>(ptr);
		}
		friend constexpr bool operator==(iterator_base const lhs, iterator_base const rhs) noexcept {
			return lhs.ptr == rhs.ptr;
		}
		friend constexpr bool operator<(iterator_base const lhs, iterator_base const rhs) noexcept {
			return lhs.ptr < rhs.ptr;
		}
	private:
		friend class moving_vector;
		constexpr explicit iterator_base(element_type * const other) noexcept:
			ptr(other) {
		}
		element_type * ptr;
	};

public:
	using value_type = T;
	using allocator_type = Allocator;
	using size_type = typename container_type::size_type;
	using difference_type = typename container_type::difference_type;
	using const_reference = value_type const &;
	using reference = value_type &;
	using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
	using pointer = typename std::allocator_traits<Allocator>::pointer;
	
	using const_iterator = iterator_base<T const>;
	using iterator = iterator_base<T>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	
	explicit moving_vector(Allocator const & allocator = Allocator{}) {
	}
	explicit moving_vector(size_type count, Allocator const & allocator = Allocator{}) {
		for (size_type n = 0; n != count; ++n) {
			emplace_back();
		}
	}
	moving_vector(size_type count, T const & value, Allocator const & allocator = Allocator{}) {
		for (size_type n = 0; n != count; ++n) {
			emplace_back(value);
		}
	}
	template<typename InputIterator>
	moving_vector(InputIterator first, InputIterator last, Allocator const & allocator = Allocator{}) {
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	moving_vector(moving_vector const & other, Allocator const & allocator):
		moving_vector(other) {
	}
	moving_vector(moving_vector && other, Allocator const & allocator):
		moving_vector(std::move(other)) {
	}
	moving_vector(std::initializer_list<T> init, Allocator const & allocator = Allocator{}):
		moving_vector(std::begin(init), std::end(init), allocator) {
	}
	
	void assign(size_type count, T const & value) {
		operator=(moving_vector(count, value));
	}
	template<typename InputIterator>
	void assign(InputIterator first, InputIterator last) {
		operator=(moving_vector(first, last));
	}
	void assign(std::initializer_list<T> init) {
		operator=(moving_vector(init));
	}
	
	constexpr const_reference at(size_type position) const {
		return *container.at(position);
	}
	reference at(size_type position) {
		return *container.at(position);
	}
	constexpr const_reference operator[](size_type position) const {
		return *container[position];
	}
	reference operator[](size_type position) {
		return *container[position];
	}
	
	constexpr const_reference front() const {
		return *begin();
	}
	reference front() {
		return *begin();
	}
	constexpr const_reference back() const {
		return *std::prev(end());
	}
	reference back() {
		return *std::prev(end());
	}
	
	const_iterator begin() const noexcept {
		return const_iterator(container.data());
	}
	iterator begin() noexcept {
		return iterator(container.data());
	}
	const_iterator cbegin() const noexcept {
		return begin();
	}
	
	const_iterator end() const noexcept {
		return begin() + size();
	}
	iterator end() noexcept {
		return begin() + size();
	}
	const_iterator cend() const noexcept {
		return end();
	}
	
	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}
	const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}
	
	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}
	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}
	const_reverse_iterator crend() const noexcept {
		return rend();
	}
	
	size_type size() const noexcept {
		return container.size();
	}
	
	template<typename... Args>
	void emplace_back(Args && ... args) {
		container.emplace_back(make_value<T>(std::forward<Args>(args)...));
	}

	template<typename U>
	friend constexpr bool operator!=(iterator_base<U> const lhs, iterator_base<U> const rhs) noexcept {
		return !(lhs == rhs);
	}
	template<typename U>
	friend constexpr bool operator>(iterator_base<U> const lhs, iterator_base<U> const rhs) noexcept {
		return rhs < lhs;
	}
	template<typename U>
	friend constexpr bool operator<=(iterator_base<U> const lhs, iterator_base<U> const rhs) noexcept {
		return !(lhs > rhs);
	}
	template<typename U>
	friend constexpr bool operator>=(iterator_base<U> const lhs, iterator_base<U> const rhs) noexcept {
		return !(lhs < rhs);
	}

	template<typename U, typename Integer>
	friend constexpr iterator_base<U> operator+(iterator_base<U> lhs, Integer const rhs) {
		return lhs += rhs;
	}
	template<typename U, typename Integer>
	friend constexpr iterator_base<U> operator+(Integer const lhs, iterator_base<U> rhs) {
		return rhs += lhs;
	}
	template<typename U, typename Integer>
	friend constexpr iterator_base<U> operator-(iterator_base<U> lhs, Integer const rhs) {
		return lhs -= rhs;
	}
	template<typename U, typename Integer>
	friend constexpr iterator_base<U> operator-(Integer const lhs, iterator_base<U> rhs) {
		return rhs -= lhs;
	}
private:
	container_type container;
};

}	// namespace smart_pointer
#endif	// VALUE_PTR_MOVING_VECTOR_HPP_
