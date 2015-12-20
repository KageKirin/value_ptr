# Copyright David Stone 2015.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

from program import Program

sources = [
	'class.cpp',
	'comparison_operators.cpp',
	'default_new.cpp',
	'make_value.cpp',
	'value_ptr.cpp',
]

source_directory = 'value_ptr'

programs = [Program('test', sources)]
