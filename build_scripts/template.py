# SCons file
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

import os
import multiprocessing

from sources import source_directory, programs

SetOption('warn', 'no-duplicate-environment')

# Options to improve the default speed of SCons
SetOption('max_drift', 2)
SetOption('implicit_cache', 1)
SetOption('num_jobs', multiprocessing.cpu_count())

AddOption('--compiler', dest = 'compiler', type = 'string', action = 'store', help = 'Name of the compiler to use.')
AddOption('--compiler-command', dest = 'compiler_command', type = 'string', action = 'store', help = 'Command to launch the compiler.')

AddOption('--verbose', dest = 'verbose', action = "store_true", help = 'Print the full compiler output.')

Decider('MD5-timestamp')

SConscript('compiler_settings.py')
Import('flags', 'compiler_command', 'compiler_name')

default = DefaultEnvironment()

# This replaces the wall of text caused by compiling with max warnings turned on
# into something a little more readable.
if not GetOption('verbose'):
	default['CXXCOMSTR'] = 'Compiling $TARGET'
	default['LINKCOMSTR'] = 'Linking $TARGET'

default.Replace(CXX = compiler_command)

build_root = '../build/' + compiler_name + '/'

def setup_environment_flags(version):
	environment = default.Clone()
	environment.Append(CCFLAGS = flags['cc'][version])
	environment.Append(CXXFLAGS = flags['cxx'][version])
	environment.Append(LINKFLAGS = flags['link'][version])
	environment.Append(CPPDEFINES = flags['cpp'][version])
	return environment

debug = setup_environment_flags('debug')
release = setup_environment_flags('release')

def defines_to_string(defines):
	string = ''
	if defines != []:
		for define in defines:
			string += define
		string += '/'
	return string

def build_directory(version, defines):
	return build_root + version + '/' + defines_to_string(defines)

def generate_sources(sources, version, defines):
	temp = []
	for source in sources:
		temp += [build_directory(version, defines) + source]
	return temp

def create_program(program):
	env_name = { 'debug':debug, 'release':release }
	suffix = { 'debug':'-debug', 'release':'' }
	for version in ['debug', 'release']:
		targets = generate_sources(program.sources, version, program.defines)
		executable_name = program.name + suffix[version]
		env = env_name[version].Clone(LIBS = program.libraries, CPPDEFINES = program.defines)
		env.Append(CPPPATH = program.include_directories)
		env.VariantDir(build_directory(version, program.defines), '../' + source_directory, duplicate = 0)
		env.Program('../' + executable_name, targets)

for program in programs:
	create_program(program)
