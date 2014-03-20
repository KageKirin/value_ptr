# Debugging options
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

class debug:
	__generate_debug_symbols = ['-g']
	__sanitize_options = [
		'-fsanitize=address',
#		'-fsanitize=thread',
#		'-fsanitize=undefined',
	]
	__sanitize_link_options = __sanitize_options + [
		'-static-libasan',
#		'-static-libtsan',
#		'-static-libunsan',
	]
	compile_flags = __generate_debug_symbols + __sanitize_options
	compile_flags_release = __generate_debug_symbols + __sanitize_options

	link_flags = __sanitize_link_options
	link_flags_release = __sanitize_link_options

