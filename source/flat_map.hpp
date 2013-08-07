// A map-like class that has better locality of reference
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

#ifndef VALUE_PTR_FLAT_MAP_HPP_
#define VALUE_PTR_FLAT_MAP_HPP_

#include <algorithm>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "apply_tuple.hpp"
#include "moving_vector.hpp"
#include <iostream>

namespace smart_pointer {

template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<std::pair<Key const, T>>, template<typename, typename> class Container = moving_vector>
class flat_map {
public:
	using key_type = Key;
	using mapped_type = T;
	using value_type = std::pair<key_type const, mapped_type>;
	using allocator_type = Allocator;
private:
	using container_type = Container<value_type, allocator_type>;
public:
	using size_type = typename container_type::size_type;
	using difference_type = typename container_type::difference_type;
	using const_reference = value_type const &;
	using reference = value_type &;
	using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
	using pointer = typename std::allocator_traits<Allocator>::pointer;
	using key_compare = Compare;
	
	using const_iterator = typename container_type::const_iterator;
	using iterator = typename container_type::iterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	
	class value_compare {
	public:
		using result_type = bool;
		using first_argument_type = value_type;
		using second_argument_type = value_type;
		constexpr bool operator()(value_type const & lhs, value_type const & rhs) const {
			return m_compare(lhs.first, rhs.first);
		}
	protected:
		friend class flat_map;
		value_compare(key_compare c):
			m_compare(c) {
		}
		Compare m_compare;
	};
	key_compare key_comp() const {
		return key_compare{};
	}
	value_compare value_comp() const {
		return value_compare(key_comp());
	}
	
	explicit flat_map(Compare const & compare = Compare{}, Allocator const & allocator = Allocator{}) {
	}
	explicit flat_map(Allocator const & allocator) {
	}
	template<typename InputIterator>
	flat_map(InputIterator first, InputIterator last, Compare const & compare = Compare{}, Allocator const & allocator = Allocator{}):
		container(first, last) {
		std::sort(container.indirect_begin(), container.indirect_end(), indirect_compare{value_comp()});
	}
	template<typename InputIterator>
	flat_map(InputIterator first, InputIterator last, Allocator const & allocator):
		flat_map(first, last, Compare{}, allocator) {
	}
	flat_map(flat_map const & other, Allocator const & allocator):
		flat_map(other) {
	}
	flat_map(flat_map && other, Allocator const & allocator):
		flat_map(std::move(other)) {
	}
	flat_map(std::initializer_list<value_type> init, Compare const & compare = Compare{}, Allocator const & allocator = Allocator{}):
		flat_map(std::begin(init), std::end(init), allocator) {
	}
	flat_map(std::initializer_list<value_type> init, Allocator const & allocator):
		flat_map(init, Compare{}, allocator) {
	}
	
	flat_map & operator=(std::initializer_list<value_type> init) {
		return *this = flat_map(init);
	}
	
	const_iterator begin() const noexcept {
		return container.begin();
	}
	iterator begin() noexcept {
		return container.begin();
	}
	const_iterator cbegin() const noexcept {
		return begin();
	}
	
	const_iterator end() const noexcept {
		return container.end();
	}
	iterator end() noexcept {
		return container.end();
	}
	const_iterator cend() const noexcept {
		return end();
	}
	
	const_reverse_iterator rbegin() const noexcept {
		return container.rbegin();
	}
	reverse_iterator rbegin() noexcept {
		return container.rbegin();
	}
	const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}
	
	const_reverse_iterator rend() const noexcept {
		return container.rend();
	}
	reverse_iterator rend() noexcept {
		return container.rend();
	}
	const_reverse_iterator crend() const noexcept {
		return rend();
	}
	
	bool empty() const noexcept {
		return container.empty();
	}
	size_type size() const noexcept {
		return container.size();
	}
	size_type max_size() const noexcept {
		return container.max_size();
	}

	// Extra functions on top of the regular map interface
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
	
	const_iterator lower_bound(key_type const & key) const {
		return std::lower_bound(begin(), end(), key, key_value_compare{key_comp()});
	}
	iterator lower_bound(key_type const & key) {
		return std::lower_bound(begin(), end(), key, key_value_compare{key_comp()});
	}
	const_iterator upper_bound(key_type const & key) const {
		return std::upper_bound(begin(), end(), key, key_value_compare{key_comp()});
	}
	iterator upper_bound(key_type const & key) {
		return std::upper_bound(begin(), end(), key, key_value_compare{key_comp()});
	}
	std::pair<const_iterator, const_iterator> equal_range(key_type const & key) const {
		return std::equal_range(begin(), end(), key, key_value_compare{key_comp()});
	}
	std::pair<iterator, iterator> equal_range(key_type const & key) {
		return std::equal_range(begin(), end(), key, key_value_compare{key_comp()});
	}
	const_iterator find(key_type const & key) const {
		auto const it = lower_bound(key);
		if (it == end()) {
			return end();
		}
		return equals(it->first, key) ? it : end();
	}
	iterator find(key_type const & key) {
		auto const it = lower_bound(key);
		if (it == end()) {
			return end();
		}
		return equals(it->first, key) ? it : end();
	}
	
	size_type count(key_type const & key) const {
		auto const range = equal_range(key);
		return static_cast<size_type>(std::distance(range.first, range.second));
	}
	
	mapped_type const & at(key_type const & key) const {
		auto const it = find(key);
		if (it == end()) {
			throw std::out_of_range{"Key not found"};
		}
		return it->second;
	}
	mapped_type & at(key_type const & key) {
		auto const it = find(key);
		if (it == end()) {
			throw std::out_of_range{"Key not found"};
		}
		return it->second;
	}
	mapped_type & operator[](key_type const & key) {
		return emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple()).first->second;
	}
	mapped_type & operator[](key_type && key) {
		return emplace(std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple()).first->second;
	}
	
	// Unlike in std::map, insert / emplace can only provide a time complexity
	// that matches an insert into the underlying container, which is to say,
	// linear. An insertion implies shifting all of the elements.
	//
	// Moreover, emplace cannot in general provide the guarantee of no copying
	// or moving. It can only provide the weaker guarantee of no copying or
	// moving of the mapped_type. If the underlying container is a
	// moving_vector, however, then we can perform the operation with no moving
	// of the value_type. In many cases, however, we will have to copy / move
	// the key_type, because we have to construct the key to determine whether
	// we should insert it.
	template<typename... Args>
	std::pair<iterator, bool> emplace(Args && ... args) {
		auto const search_strategy = [&](key_type const & key) {
			return this->upper_bound(key);
		};
		return emplace_search(search_strategy, std::forward<Args>(args)...);
	}
	template<typename... Args>
	iterator emplace_hint(const_iterator hint, Args && ... args) {
		auto const search_strategy = [&](key_type const & key) {
			bool const correct_greater = this->key_comp(key, *hint);
			bool const correct_less = this->key_comp(*std::prev(hint), key);
			bool const correct_hint = correct_greater and correct_less;
			return correct_hint ? hint : this->upper_bound(key);
		};
		return emplace_search(search_strategy, std::forward<Args>(args)...).first;
	}

	template<typename P>
	std::pair<iterator, bool> insert(P && value) {
		return emplace(std::forward<P>(value));
	}
	template<typename P>
	iterator insert(const_iterator const hint, P && value) {
		return emplace_hint(hint, std::forward<P>(value));
	}
	template<typename InputIterator>
	void insert(InputIterator first, InputIterator const last) {
		// Because my underlying container is expected to be contiguous storage,
		// it's best to do a batch insert and then just sort it all. However,
		// because I know that the first section of the final range is already
		// sorted, it's probably better to just sort the new elements then do a
		// merge sort on both ranges, rather than calling std::sort on the
		// entire container.
		auto const midpoint = static_cast<typename container_type::indirect_iterator>(container.insert(container.end(), first, last));
		std::sort(midpoint, container.indirect_end(), indirect_compare{value_comp()});
		std::inplace_merge(container.indirect_begin(), midpoint, container.indirect_end(), indirect_compare{value_comp()});
	}
	void insert(std::initializer_list<value_type> init) {
		insert(std::begin(init), std::end(init));
	}
	
	// These maintain the relative order of all elements in the container, so I
	// don't have to worry about re-sorting
	iterator erase(const_iterator const it) {
		return container.erase(it);
	}
	iterator erase(const_iterator const first, const_iterator const last) {
		return container.erase(first, last);
	}
	size_type erase(key_type const & key) {
		auto const range = equal_range(key);
		if (range.first == end()) {
			return 0;
		}
		erase(range.first, range.second);
		return std::distance(range.first, range.second);
	}
	
	void swap(flat_map & other) noexcept {
		container.swap(other.container);
	}

	friend bool operator==(flat_map const & lhs, flat_map const & rhs) noexcept {
		return lhs.container == rhs.container;
	}
	friend bool operator<(flat_map const & lhs, flat_map const & rhs) noexcept {
		return lhs.container < rhs.container;
	}

private:
	constexpr bool equals(key_type const & lhs, key_type const & rhs) const {
		return !key_comp()(lhs, rhs) and !key_comp()(rhs, lhs);
	}
	class key_value_compare {
	public:
		constexpr key_value_compare(key_compare const & compare):
			m_compare(compare) {
		}
		constexpr bool operator()(key_type const & key, value_type const & value) const {
			return m_compare(key, value.first);
		}
		constexpr bool operator()(value_type const & value, key_type const & key) const {
			return m_compare(value.first, key);
		}
	private:
		key_compare const & m_compare;
	};
	class indirect_compare {
	public:
		constexpr indirect_compare(value_compare const & compare):
			m_compare(compare) {
		}
		constexpr bool operator()(typename container_type::indirect_iterator::value_type const & lhs, typename container_type::indirect_iterator::value_type const & rhs) const {
			return m_compare(*lhs, *rhs);
		}	
	private:
		value_compare const & m_compare;
	};

	// It is safe to bind the reference to the object that is being moved in any
	// of these calls to emplace_key because the call to std::move does not
	// actually move anything, it just converts it to an rvalue reference. The
	// object isn't actually moved from until later on in the function.
	//
	// Search represents a function that finds where to insert
	template<typename Search>
	std::pair<iterator, bool> emplace_search(Search const search) {
		// key_type must be default initialized in case it's something like int
		key_type key{};
		return emplace_key(search, key, std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple());
	}
	template<typename Search, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, key_type const & key, ConvertToMapped && mapped) {
		return emplace_key(search, key, key, std::forward<ConvertToMapped>(mapped));
	}
	template<typename Search, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, key_type && key, ConvertToMapped && mapped) {
		return emplace_key(search, key, std::move(key), std::forward<ConvertToMapped>(mapped));
	}
	template<typename Search, typename ConvertToKey, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, ConvertToKey && convert_to_key, ConvertToMapped && mapped) {
		key_type key(convert_to_key);
		return emplace_key(search, key, std::move(key), std::forward<ConvertToMapped>(mapped));
	}
	// These pair constructors could possibly be better written to not construct
	// key twice if ConvertToKey is not the same type as key_type.
	template<typename Search, typename ConvertToKey, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, std::pair<ConvertToKey, ConvertToMapped> && value) {
		return emplace_key(search, value.first, std::move(value));
	}
	template<typename Search, typename ConvertToKey, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, std::pair<ConvertToKey, ConvertToMapped> const & value) {
		return emplace_key(search, value.first, value);
	}
	class make_key {
	public:
		template<typename... Args>
		constexpr key_type operator()(Args && ... args) {
			return key_type(std::forward<Args>(args)...);
		}
	};
	template<typename Search, typename KeyTuple, typename MappedTuple>
	std::pair<iterator, bool> emplace_search(Search const search, std::piecewise_construct_t, KeyTuple && key_tuple, MappedTuple && mapped_tuple) {
		key_type key(apply(make_key{}, std::forward<KeyTuple>(key_tuple)));
		return emplace_key(search, key, std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward<MappedTuple>(mapped_tuple));
	}

	// args contains everything needed to construct value_type, including the
	// key. It is provided here as the first argument just to make things easier
	template<typename Search, typename... Args>
	std::pair<iterator, bool> emplace_key(Search const search, key_type const & key, Args && ... args) {
		auto const it = search(key);
		// If the element is inserted, the size must be one greater
		auto const prior_size = size();
		return std::make_pair(container.emplace(it, std::forward<Args>(args)...), size() > prior_size);
	}

	container_type container;
};

template<typename Key, typename T, typename Compare, typename Allocator, template<typename, typename> class Container>
void swap(flat_map<Key, T, Compare, Allocator, Container> & lhs, flat_map<Key, T, Compare, Allocator, Container> & rhs) noexcept {
	lhs.swap(rhs);
}

template<typename Key, typename T, typename Compare, typename Allocator, template<typename, typename> class Container>
bool operator!=(flat_map<Key, T, Compare, Allocator, Container> const & lhs, flat_map<Key, T, Compare, Allocator, Container> const & rhs) noexcept {
	return !(lhs == rhs);
}
template<typename Key, typename T, typename Compare, typename Allocator, template<typename, typename> class Container>
bool operator>(flat_map<Key, T, Compare, Allocator, Container> const & lhs, flat_map<Key, T, Compare, Allocator, Container> const & rhs) noexcept {
	return rhs < lhs;
}
template<typename Key, typename T, typename Compare, typename Allocator, template<typename, typename> class Container>
bool operator<=(flat_map<Key, T, Compare, Allocator, Container> const & lhs, flat_map<Key, T, Compare, Allocator, Container> const & rhs) noexcept {
	return !(lhs > rhs);
}
template<typename Key, typename T, typename Compare, typename Allocator, template<typename, typename> class Container>
bool operator>=(flat_map<Key, T, Compare, Allocator, Container> const & lhs, flat_map<Key, T, Compare, Allocator, Container> const & rhs) noexcept {
	return !(lhs < rhs);
}

template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<std::pair<Key const, T>>>
using unstable_flat_map = flat_map<Key, T, Compare, Allocator, std::vector>;

template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<std::pair<Key const, T>>>
using stable_flat_map = flat_map<Key, T, Compare, Allocator, moving_vector>;


}	// namespace smart_pointer
#endif	// VALUE_PTR_FLAT_MAP_HPP_
