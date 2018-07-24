cmake_minimum_required( VERSION 2.8.12 )

project( libtemp_file_prj )

find_package( Boost 1.58.0 COMPONENTS system filesystem iostreams unit_test_framework REQUIRED )

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK ) 

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 /std:c++latest ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -march=native -pthread -Weverything -Wno-c++98-compat -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -march=native -pthread -Wall -Wno-deprecated-declarations)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	endif( )
endif( )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/temp_file.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/temp_file.cpp
)

#set( CMAKE_CTEST_COMMAND ctest -V )
add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_library( temp_file ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

add_executable( temp_file_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/temp_file_test.cpp )
target_link_libraries( temp_file_test_bin temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
add_test( temp_file_test, temp_file_test_bin )
add_dependencies( check temp_file_test_bin )

install( TARGETS temp_file DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw )
