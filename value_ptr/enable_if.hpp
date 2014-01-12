// A slight simplification of std::enable_if, modified from
// http://flamingdangerzone.com/cxx11/2012/06/01/almost-static-if.html
// Copyright (C) 2014 David Stone
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

#ifndef VALUE_PTR_ENABLE_IF_HPP_
#define VALUE_PTR_ENABLE_IF_HPP_

#include <type_traits>

namespace smart_pointer {

#if 0

// Usage:
template<typename Blah, enable_if_t<1 + 1 == 2>...>
void f() {
}

#endif

namespace detail {

enum class enabler {};

}	// namespace detail

// This is used a workaround for clang.
constexpr detail::enabler enabler_dummy{};

template<bool condition>
using enable_if_t = typename std::enable_if<condition, detail::enabler>::type;

}	// namespace smart_pointer
#endif	// VALUE_PTR_FLAT_MAP_HPP_
