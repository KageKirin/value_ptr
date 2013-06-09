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

#include "value_forward_list.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <forward_list>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>


using namespace smart_pointer;
namespace {

void test_semantics() {
	forward_list<int> fl;
	fl.emplace_front(5);
	assert(fl.front() == 5);
	fl.emplace_front(2);
	assert(fl.front() == 2);
	fl.assign({2, 1, 3});
	assert(!fl.empty());
	fl.reverse();
	assert(fl == forward_list<int>({ 3, 1, 2 }));
	fl.sort();
	assert(fl == forward_list<int>({ 1, 2, 3 }));
}

template<typename T>
using list_type = typename std::conditional<USE_SYSTEM_FORWARD_LIST, std::forward_list<T>, forward_list<T>>::type;

template<typename T>
void test_performance(std::size_t const loop_count) {
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_int_distribution<T> distribution;

	auto const start = std::chrono::high_resolution_clock::now();
	list_type<T> fl;
	for (std::size_t n = 0; n != loop_count; ++n) {
		fl.emplace_front(distribution(engine));
	}
	auto const constructed = std::chrono::high_resolution_clock::now();
	fl.sort();
	auto const sorted = std::chrono::high_resolution_clock::now();
	fl.reverse();
	auto const reversed = std::chrono::high_resolution_clock::now();
	typedef std::chrono::microseconds unit;
	std::cout << "Construction time: " << std::chrono::duration_cast<unit>(constructed - start).count() << '\n';
	std::cout << "Sorting time: " << std::chrono::duration_cast<unit>(sorted - constructed).count() << '\n';
	std::cout << "Reversing time: " << std::chrono::duration_cast<unit>(reversed - sorted).count() << '\n';
}

}	// namespace

int main(int argc, char ** argv) {
	test_semantics();
	auto const loop_count = (argc == 1) ? 1 : std::stoull(argv[1]);
	test_performance<std::size_t>(loop_count);
}










