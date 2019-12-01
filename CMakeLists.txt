cmake_minimum_required(VERSION 3.10)

project(libtemp_file_prj CXX)

include("${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(Boost 1.62 COMPONENTS system iostreams filesystem REQUIRED)

if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" )
    message( "Currently disabling std::filesystem support" )
    add_compile_definitions( NO_STD_FS )
    find_package(Filesystem REQUIRED)
endif( )

enable_testing()


set(HEADER_FOLDER "include")
set(SOURCE_FOLDER "src")
set(TEST_FOLDER "tests")

include_directories(${HEADER_FOLDER})

set(HEADER_FILES
        ${HEADER_FOLDER}/daw/temp_file.h
        )

set(CMAKE_CTEST_COMMAND ctest -V)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

option(GLEAN_INSTALL_ROOT "")
if (EXISTS "${GLEAN_INSTALL_ROOT}")
    message("using glean dependencies")
    message("installing to ${GLEAN_INSTALL_ROOT}")

    include_directories(SYSTEM "${GLEAN_INSTALL_ROOT}/include")
    link_directories("${GLEAN_INSTALL_ROOT}/lib")
endif ()

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(temp_file_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/temp_file_test.cpp)
target_link_libraries(temp_file_test_bin ${FILEYSTEM_LIBRARIES} ${Boost_LIBRARIES})
add_test(temp_file_test temp_file_test_bin)
add_dependencies(check temp_file_test_bin)

install(DIRECTORY ${HEADER_FOLDER}/ DESTINATION include)

