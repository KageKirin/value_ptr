// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "value_ptr.hpp"
#include <algorithm>
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
	Tester(Tester const &) {
		++copy_constructed;
	}
	Tester(Tester &&) noexcept {
		++move_constructed;
	}
	Tester & operator=(Tester const &) = delete;
	Tester & operator=(Tester &&) noexcept = delete;
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

std::size_t Tester::default_constructed = 0;
std::size_t Tester::copy_constructed = 0;
std::size_t Tester::move_constructed = 0;
std::size_t Tester::copy_assigned = 0;
std::size_t Tester::move_assigned = 0;
std::size_t Tester::destructed = 0;

static_assert(sizeof(value_ptr<Tester>) == sizeof(Tester *), "value_ptr wrong size!");
static_assert(sizeof(value_ptr<Tester[]>) == sizeof(Tester *), "value_ptr array wrong size!");

#define CHECK_EQUALS(condition1, condition2) do { \
	if ((condition1) != (condition2)) { \
		std::cerr << (condition1) << ", " << (condition2) << '\n'; \
		assert((condition1) == (condition2)); \
	} \
} while(false)


template<typename T>
class Verify {
public:
	void operator()() const {
		CHECK_EQUALS(T::default_constructed, default_constructed);
		CHECK_EQUALS(T::copy_constructed, copy_constructed);
		CHECK_EQUALS(T::move_constructed, move_constructed);
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
	void move_construct() {
		++move_constructed;
	}
	void destruct() {
		++destructed;
	}
private:
	std::size_t default_constructed = 0;
	std::size_t copy_constructed = 0;
	std::size_t move_constructed = 0;
	std::size_t destructed = 0;
};

class Base {};
class Derived : public Base {};

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
	auto p = value_ptr<Tester>(t);
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
	constexpr std::size_t size = 5;
	value_ptr<Tester[]> array(new Tester[size]);
	for (std::size_t n = 0; n != size; ++n) {
		verify.default_construct();
	}
	verify();
	auto moved_to = value_ptr<Tester>(std::move(t));
	verify.move_construct();
	verify();
	
	verify.destruct();	// moved_to
	verify.destruct();	// t
	verify.destruct();	// p
	for (std::size_t n = 0; n != v.size(); ++n) {
		verify.destruct();
	}
	for (std::size_t n = 0; n != size; ++n) {
		verify.destruct();
	}
	value_ptr<Base> vb(new Derived{});
	value_ptr<Base> vexplicit(value_ptr<Derived>(new Derived{}));
	value_ptr<Base> vmake(make_value<Derived>());
}

void test_assignment(Verify<Tester> & verify) {
	verify();
	value_ptr<Tester> p;
	p = make_value<Tester>();
	verify.default_construct();
	verify();
	p = Tester{};
	verify.default_construct();
	verify.destruct();
	verify.move_construct();
	verify.destruct();
	verify();
}

void test_array_semantics() {
	constexpr std::size_t size = 10;
	auto a = make_value<std::size_t[]>(size);
	std::iota(a.get(), a.get() + size, 0);
	for (size_t n = 0; n != size; ++n) {
		CHECK_EQUALS(a[n], n);
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
	test_array_semantics();

	std::vector<value_ptr<int>> temp;
	temp.emplace_back(make_value<int>(5));
	temp.emplace_back(make_value<int>(3));
	temp.emplace_back(make_value<int>(1));
	temp.emplace_back(make_value<int>(2));
	temp.emplace_back(make_value<int>(4));
	std::sort(std::begin(temp), std::end(temp), [](value_ptr<int> const & lhs, value_ptr<int> const & rhs) {
		return *lhs < *rhs;
	});
}

class VirtualBase {
public:
	virtual ~VirtualBase() = default;
};

class VirtualDerived : public VirtualBase {
};

void test_virtual_cloning() {
	value_ptr<VirtualBase> ptr(new VirtualDerived{});
	static_cast<void>(ptr);
//	value_ptr<VirtualBase> other(ptr);
//	static_cast<void>(other);
}

}	// namespace

int main() {
	Verify<Tester> verify;
	test_constructors(verify);
	verify();
	test_assignment(verify);
	test_semantics();
	test_virtual_cloning();
}
