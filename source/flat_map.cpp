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

#include "flat_map.hpp"
#include <algorithm>
#include <cassert>

using namespace smart_pointer;

namespace {

class Final {
public:
	constexpr Final() noexcept = default;
	Final(Final const & other) = delete;
	Final(Final && other) = delete;
	Final & operator=(Final const & other) = delete;
	Final & operator=(Final && other) = delete;
};

}	// namespace

int main(int argc, char ** argv) {
	stable_flat_map<int, int> empty;
	stable_flat_map<int, int> container({ {1, 2}, {2, 5}, {3, 3} });
	assert((container == stable_flat_map<int, int>{ {1, 2}, {2, 5}, {3, 3} }));
	container.emplace(std::make_pair(4, 4));
//	container.emplace(std::piecewise_construct, std::forward_as_tuple(5), std::forward_as_tuple(3));
//	assert(container.at(5) == 3);
	
//	stable_flat_map<int, Final> final;
//	final.emplace(std::piecewise_construct, std::forward_as_tuple(5), std::forward_as_tuple());
}
