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
#include <utility>
#include <vector>

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

#define CHECK_EQUALS(condition1, condition2) do { \
	if ((condition1) != (condition2)) { \
		std::cerr << (condition1) << ", " << (condition2) << '\n'; \
		assert(condition1 == condition2); \
	} \
} while(0)


template<typename T>
class Verify {
public:
	void operator()() const {
		CHECK_EQUALS(T::default_constructed, default_constructed);
		CHECK_EQUALS(T::copy_constructed, copy_constructed);
		CHECK_EQUALS(T::move_constructed, 0);
		CHECK_EQUALS(T::copy_assigned, 0);
		CHECK_EQUALS(T::move_assigned, 0);
		CHECK_EQUALS(T::destructed, destructed);
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

void test_constructors(Verify<Tester> & verify) {
	verify();
	{
		value_ptr<Tester> p;
	}
	verify();
	{
		value_ptr<Tester> p(new Tester);
		verify.default_construct();
		verify();
		p.reset(nullptr);
		verify.destruct();
		verify();
	}
	verify();
	Tester t;
	verify.default_construct();
	value_ptr<Tester> p(t);
	verify.copy_construct();
	verify();
	std::vector<value_ptr<Tester>> v;
	v.emplace_back(std::move(p));
	verify();
	for (size_t n = 0; n != 10; ++n) {
		v.emplace_back(value_ptr<Tester>(new Tester));
		verify.default_construct();
	}
	verify();
	using std::swap;
	swap(v[1], v[2]);
	verify();
	value_ptr<Tester> d(v[3]);
	verify.copy_construct();
	verify();
	verify.destruct();	// t
	verify.destruct();	// p
	for (size_t n = 0; n != v.size(); ++n) {
		verify.destruct();
	}
}

class C {
public:
	int get() const {
		return 4;
	}
};

void test_semantics() {
	value_ptr<int> a(new int(5));
	value_ptr<int> b(new int(7));
	CHECK_EQUALS(*a + *b, 12);
	value_ptr<C> c(new C);
	CHECK_EQUALS(*a + c->get(), 9);
}

}	// namespace

int main() {
	Verify<Tester> verify;
	test_constructors(verify);
	verify();
}









