# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/libtemp_file
#

add_executable(temp_file_test_bin src/temp_file_test.cpp)
target_link_libraries(temp_file_test_bin daw::libtemp_file )
add_test(temp_file_test temp_file_test_bin)



