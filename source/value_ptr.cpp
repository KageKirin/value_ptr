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
#include <iostream>

class Tester {
public:
	Tester() {
		std::cout << "Default constructed\n";
	}
	Tester(Tester const & other) {
		std::cout << "Copy constructed\n";
	}
	Tester(Tester && other) noexcept {
		std::cout << "Move constructed\n";
	}
	Tester & operator=(Tester const & other) {
		std::cout << "Copy assigned\n";
		return *this;
	}
	Tester & operator=(Tester && other) noexcept {
		std::cout << "Move assigned\n";
		return *this;
	}
	~Tester() noexcept {
		std::cout << "Destructed.\n";
	}
};

using namespace smart_pointer;

namespace {
void f(value_ptr<Tester> value) {
}
}

int main() {
	static_assert(sizeof(value_ptr<Tester>) == sizeof(Tester *), "value_ptr wrong size!");
	value_ptr<Tester[3]> first;
}
