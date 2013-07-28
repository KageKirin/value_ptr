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

#include "moving_vector.hpp"
#include <algorithm>
#include <cassert>

using namespace smart_pointer;

int main(int argc, char ** argv) {
	assert(moving_vector<int>({1, 2, 2, 3}) == moving_vector<int>({1, 2, 2, 3}));
	moving_vector<int> v({2, 5, 6, 2, -3});
	assert(std::accumulate(v.begin(), v.end(), 0) == 2 + 5 + 6 + 2 - 3);
	std::sort(v.begin(), v.end());
	assert(v == moving_vector<int>({-3, 2, 2, 5, 6}));
}
