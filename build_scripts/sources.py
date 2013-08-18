# List of sources
# Copyright (C) 2013 David Stone
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

# Example of use:
#
# example_sources = prepend_dir('cool', ['alright.cpp', 'awesome.cpp'])
# example_sources += prepend_dir('yeah', ['no.cpp', 'maybe.cpp'])
# example_defines = []
# example_libraries = ['boost_filesystem']
#
# example = ('example_program_name', example_sources, example_defines, example_libraries)
# base_sources = [example]
#
# It is already assumed that the sources are in the 'source/' folder; it does
# not need to be specified.

value_ptr_tests = ('test', ['value_ptr.cpp'], [], [])
forward_list_tests = ('forward_list', ['value_forward_list.cpp'], ['USE_SYSTEM_FORWARD_LIST=false'], [])
forward_list_std_tests = ('forward_list_std', ['value_forward_list.cpp'], ['USE_SYSTEM_FORWARD_LIST=true'], [])
moving_vector_tests = ('moving_vector', ['moving_vector.cpp'], [], [])
stable_flat_map_tests = ('flat_map_stable', ['flat_map.cpp'], ['USE_STABLE_FLAT_MAP'], [])
unstable_flat_map_tests = ('flat_map_unstable', ['flat_map.cpp'], ['USE_UNSTABLE_FLAT_MAP'], [])
flat_map_std_tests = ('flat_map_std', ['flat_map.cpp'], ['USE_SYSTEM_MAP'], [])

def reserve_options(container):
	# reserve doesn't work properly in gcc 4.7
	return ['NO_RESERVE']
	if container != 'deque' and container != 'list':
		return ['RESERVE', 'NO_RESERVE']
	else:
		return ['NO_RESERVE']

performance_sources = ['performance_test_sequence.cpp']
container_types = ['deque', 'list', 'moving_vector', 'vector']
# Don't keep compiling all of these other versions that I rarely use
# array_sizes = ['1', '10', '40', '50', '60', '100', '200', '500', '1000']
array_sizes = ['100']
performance_tests = [('performance_' + container + '_' + reserve + '_' + size, performance_sources, [container.upper(), reserve, 'ARRAY_SIZE=' + size], []) for container in container_types for size in array_sizes for reserve in reserve_options(container)]


base_sources = [value_ptr_tests, forward_list_tests, forward_list_std_tests, moving_vector_tests, flat_map_std_tests, unstable_flat_map_tests, stable_flat_map_tests] + performance_tests
