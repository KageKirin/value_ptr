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
	using container_type = std::vector<value_ptr<T>>;
public:
	using value_type = T;
	using allocator_type = Allocator;
	using size_type = typename container_type::size_type;
	using difference_type = typename container_type::difference_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
	using pointer = typename container_type::pointer;
	using const_pointer = typename container_type::const_pointer;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;
	
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
	
	template<typename... Args>
	void emplace_back(Args && ... args) {
		container.emplace_back(make_value<T>(std::forward<Args>(args)...));
	}
private:
	container_type container;
};

}	// namespace smart_pointer
#endif	// VALUE_PTR_MOVING_VECTOR_HPP_
