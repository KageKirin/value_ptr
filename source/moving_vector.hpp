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
		iterator_base & operator+=(difference_type const offset) {
			ptr += offset;
			return *this;
		}
		iterator_base & operator-=(difference_type const offset) {
			ptr -= offset;
			return *this;
		}
		friend constexpr difference_type operator-(iterator_base const lhs, iterator_base const rhs) {
			return lhs.ptr - rhs.ptr;
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
		using indirection_type = typename std::conditional<std::is_const<U>::value, element_type const, element_type>::type;
		friend class moving_vector;
		constexpr explicit iterator_base(indirection_type * const other) noexcept:
			ptr(other) {
		}
		constexpr typename container_type::const_iterator make_base_iterator(container_type const & base) const noexcept {
			auto const offset = ptr - base.data();
			return base.begin() + offset;
		}
		indirection_type * ptr;
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
		*this = moving_vector(count, value);
	}
	template<typename InputIterator>
	void assign(InputIterator first, InputIterator last) {
		*this = moving_vector(first, last);
	}
	void assign(std::initializer_list<T> init) {
		*this = moving_vector(init);
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
	
	// data() intentionally missing
	
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
		return begin() + static_cast<difference_type>(size());
	}
	iterator end() noexcept {
		return begin() + static_cast<difference_type>(size());
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
	
	bool empty() const noexcept {
		return size() == 0;
	}
	size_type size() const noexcept {
		return container.size();
	}
	size_type max_size() const noexcept {
		return container.max_size();
	}

	size_type capacity() const noexcept {
		return container.capacity();
	}
	void reserve(size_type const new_capacity) noexcept {
		return container.reserve(new_capacity);
	}
	void shink_to_fit() {
		container.shrink_to_fit();
	}
	
	void clear() noexcept {
		container.clear();
	}
	
	template<typename... Args>
	iterator emplace(const_iterator const position, Args && ... args) {
		return container.emplace(position.make_base_iterator(container), make_value<T>(std::forward<Args>(args)...));
	}
	
	iterator insert(const_iterator const position, T const & value) {
		return emplace(position, value);
	}
	iterator insert(const_iterator const position, T && value) {
		return emplace(position, std::move(value));
	}
	iterator insert(const_iterator const position, size_type const count, T const & value) {
		for (size_type n = 0; n != count; ++n) {
			emplace(position, value);
		}
	}
	template<typename InputIterator>
	iterator insert(const_iterator const position, InputIterator first, InputIterator const last) {
		if (first == last) {
			return position;
		}
		for ( ; first != last; ++first) {
			emplace(position, *first);
		}
		return std::next(position);
	}
	iterator insert(const_iterator const position, std::initializer_list<T> ilist) {
		if (ilist.empty()) {
			return position;
		}
		for (auto const & value : ilist) {
			emplace(position, value);
		}
		return std::next(position);
	}

	template<typename... Args>
	void emplace_back(Args && ... args) {
		container.emplace_back(make_value<T>(std::forward<Args>(args)...));
	}
	
	void push_back(T const & value) {
		emplace_back(value);
	}
	void push_back(T && value) {
		emplace_back(std::move(value));
	}
	
	iterator erase(const_iterator const position) {
		return container.erase(position.make_base_iterator(container));
	}
	iterator erase(const_iterator const first, const_iterator const last) {
		return container.erase(first.make_base_iterator(container), last.make_base_iterator(container));
	}
	void pop_back() {
		container.pop_back();
	}
	
	void resize(size_type const new_size) {
		auto old_size = size();
		container.resize(new_size);
		for ( ; old_size <= new_size; ++old_size) {
			container[old_size] = make_value<T>();
		}
	}
	void resize(size_type const count, T const & value) {
		container.resize(count, make_value<T>(value));
	}
	void resize(size_type const count, T && value) {
		container.resize(count, make_value<T>(std::move(value)));
	}
	
	void swap(moving_vector & other) noexcept {
		container.swap(other.container);
	}

	friend bool operator==(moving_vector const & lhs, moving_vector const & rhs) noexcept {
		return lhs.size() == rhs.size() and std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	friend bool operator<(moving_vector const & lhs, moving_vector const & rhs) noexcept {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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

	template<typename U>
	friend constexpr iterator_base<U> operator+(iterator_base<U> lhs, typename iterator_base<U>::difference_type const rhs) {
		return lhs += rhs;
	}
	template<typename U>
	friend constexpr iterator_base<U> operator+(typename iterator_base<U>::difference_type const lhs, iterator_base<U> rhs) {
		return rhs += lhs;
	}
	template<typename U>
	friend constexpr iterator_base<U> operator-(iterator_base<U> lhs, typename iterator_base<U>::difference_type const rhs) {
		return lhs -= rhs;
	}
private:
	container_type container;
};

template<typename T, typename Allocator>
void swap(moving_vector<T, Allocator> & lhs, moving_vector<T, Allocator> & rhs) noexcept {
	lhs.swap(rhs);
}

template<typename T, typename Allocator>
bool operator!=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs == rhs);
}
template<typename T, typename Allocator>
bool operator>(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return rhs < lhs;
}
template<typename T, typename Allocator>
bool operator<=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs > rhs);
}
template<typename T, typename Allocator>
bool operator>=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs < rhs);
}

}	// namespace smart_pointer
#endif	// VALUE_PTR_MOVING_VECTOR_HPP_
