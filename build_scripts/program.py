# Program class
# Copyright (C) 2014 David Stone
#
# This program is free software: you can redistribute it and / or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

def prepend_dir(directory, sources):
	"""Remove redundant specification of a directory for multiple sources"""
	return map(lambda source: directory + '/' + source, sources)

class Program:
	def __init__(self, name, sources, defines = [], libraries = [], include_directories = []):
		self.name = name
		self.sources = sources
		self.defines = defines
		self.libraries = libraries
		self.include_directories = include_directories

# Example of use:
#
# example_sources = prepend_dir('cool', ['alright.cpp', 'awesome.cpp'])
# example_sources += prepend_dir('yeah', ['no.cpp', 'maybe.cpp'])
# example_defines = ['NDEBUG']
# example_libraries = ['boost_filesystem']
# example_include_directories = []
#
# example = Program('example_program_name', example_sources, example_defines, example_libraries, example_include_directories)
#
# programs = [example]
