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

#include "value_ptr.hpp"
#include <cassert>
#include <iostream>
#include <tuple>

using namespace smart_pointer;
namespace {

class Tester {
public:
	Tester() {
		++default_constructed;
	}
	Tester(Tester const & other) {
		++copy_constructed;
	}
	Tester(Tester && other) noexcept {
		++move_constructed;
	}
	Tester & operator=(Tester const & other) {
		++copy_assigned;
		return *this;
	}
	Tester & operator=(Tester && other) noexcept {
		++move_assigned;
		return *this;
	}
	~Tester() noexcept {
		++destructed;
	}
	static std::size_t default_constructed;
	static std::size_t copy_constructed;
	static std::size_t move_constructed;
	static std::size_t copy_assigned;
	static std::size_t move_assigned;
	static std::size_t destructed;
};

std::size_t Tester::default_constructed;
std::size_t Tester::copy_constructed;
std::size_t Tester::move_constructed;
std::size_t Tester::copy_assigned;
std::size_t Tester::move_assigned;
std::size_t Tester::destructed;

static_assert(sizeof(value_ptr<Tester>) == sizeof(Tester *), "value_ptr wrong size!");

template<typename T>
class Verify {
public:
	void operator()() const {
		assert(T::default_constructed == default_constructed);
		assert(T::copy_constructed == copy_constructed);
		assert(T::move_constructed == 0);
		assert(T::copy_assigned == 0);
		assert(T::move_assigned == 0);
		assert(T::destructed == destructed);
	}
	void default_construct() {
		++default_constructed;
	}
	void copy_construct() {
		++copy_constructed;
	}
	void destruct() {
		++destructed;
	}
private:
	std::size_t default_constructed = 0;
	std::size_t copy_constructed = 0;
	std::size_t destructed = 0;
};

}	// namespace

int main() {
	Verify<Tester> verify;
	verify();
	value_ptr<Tester> a;
	verify();
	value_ptr<Tester> b(new Tester);
	verify.default_construct();
	verify();
	b.reset(nullptr);
	verify.destruct();
	verify();
}









