cmake_minimum_required( VERSION 2.8.12 )

project( libdelete_on_exit_prj )

include( ExternalProject )

find_package( Boost 1.58.0 COMPONENTS system filesystem iostreams unit_test_framework REQUIRED )

add_definitions( -DBOOST_TEST_DYN_LINK ) 

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -Weverything -Wno-c++98-compat -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)		
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -Wall -Wno-deprecated-declarations)
	endif()
endif()

set( HEADER_FOLDER "include" )
include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/delete_on_exit.h
)

set( SOURCE_FILES
	delete_on_exit.cpp
 )

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_library( delete_on_exit ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( delete_on_exit ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

add_executable( delete_on_exit_test_bin delete_on_exit_test.cpp ${HEADER_FILES} )
target_link_libraries( delete_on_exit_test_bin delete_on_exit ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
add_test( delete_on_exit_test, delete_on_exit_test_bin )

install( TARGETS delete_on_exit DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw )
