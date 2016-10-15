cmake_minimum_required( VERSION 2.8.12 )

project( libtemp_file_prj )

include( ExternalProject )

find_package( Boost 1.58.0 COMPONENTS system filesystem unit_test_framework REQUIRED )

add_definitions( -DBOOST_TEST_DYN_LINK ) 

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -stdlib=libc++ -ferror-limit=1 -Weverything -Wno-c++98-compat -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)		
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		#add_compile_options(-std=c++14 -Wall -Werror -fmax-errors=1 -Wno-deprecated-declarations)
		add_compile_options(-std=c++14 -Wall -Wno-deprecated-declarations)
	endif()
endif()

set( HEADER_FOLDER "include" )
include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/temp_file.h
)

set( SOURCE_FILES
	temp_file.cpp
 )

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_library( temp_file ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

add_executable( temp_file_test_bin temp_file_test.cpp ${HEADER_FILES} )
target_link_libraries( temp_file_test_bin temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
add_test( temp_file_test, temp_file_test_bin )

install( TARGETS temp_file DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw )
