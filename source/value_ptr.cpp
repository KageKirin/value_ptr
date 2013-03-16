// A smart pointer with value semantics
// Copyright (C) 2013 David Stone
//
// This file is part of Technical Machine.
//
// Technical Machine is free software: you can redistribute it and / or modify
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
	std::cout << "1\n";
	value_ptr<Tester> a(new Tester);
	std::cout << "2\n";
	f(a);
	std::cout << "3\n";
	f(std::move(a));
	std::cout << "4\n";
	Tester tester1;
	std::cout << "5\n";
	value_ptr<Tester> b(tester1);
	std::cout << "6\n";
	Tester const tester2;
	std::cout << "7\n";
	value_ptr<Tester> c(tester2);
	std::cout << "8\n";
}
