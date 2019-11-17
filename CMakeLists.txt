cmake_minimum_required( VERSION 2.8.12 )

project( libtemp_file_prj )

find_package( Boost 1.62 COMPONENTS system iostreams filesystem REQUIRED )

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK ) 

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/daw/temp_file.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/temp_file.cpp
)

#set( CMAKE_CTEST_COMMAND ctest -V )
add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )

option( GLEAN_INSTALL_ROOT "" )
if( EXISTS "${GLEAN_INSTALL_ROOT}" ) 
	message( "using glean dependencies" )
	message( "installing to ${GLEAN_INSTALL_ROOT}" )

	include_directories( SYSTEM "${GLEAN_INSTALL_ROOT}/include" )
	link_directories( "${GLEAN_INSTALL_ROOT}/lib" )
endif( )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_library( temp_file ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

add_executable( temp_file_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/temp_file_test.cpp )
target_link_libraries( temp_file_test_bin temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
add_test( temp_file_test, temp_file_test_bin )
add_dependencies( check temp_file_test_bin )

install( TARGETS temp_file DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include )
