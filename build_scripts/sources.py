# List of sources
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
	def __init__(self, name, sources, defines = [], libraries = []):
		self.name = name
		self.sources = sources
		self.defines = defines
		self.libraries = libraries

test = Program('test', ['class.cpp', 'comparison_operators.cpp', 'make_value.cpp', 'value_ptr.cpp'])

source_directory = 'value_ptr'

programs = [test]

include_directories = []
