// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define BOOST_TEST_MODULE glean_test 

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <iterator>

#include "delete_on_exit.h"

BOOST_AUTO_TEST_CASE( delete_on_exit_test_002 ) {
	daw::delete_on_exit tmp;
	BOOST_REQUIRE( tmp );
	tmp.secure_create_file( );
	std::cout << "Temp file: " << *tmp << std::endl;
	std::ofstream out_file{ tmp->string( ), std::ios::trunc };
	out_file << "Test failed\n";
	out_file.close( );
}

BOOST_AUTO_TEST_CASE( delete_on_exit_test_003 ) {
	boost::filesystem::path p;
	{
		daw::delete_on_exit tmp;
		BOOST_REQUIRE( tmp );
		std::cout << "Temp file: " << *tmp << std::endl;
		tmp.secure_create_file( );
		std::ofstream out_file{ tmp->string( ), std::ios::trunc };
		out_file << "Test passed\n";
		out_file.close( );
		p = tmp.disconnect( );
	}
	BOOST_REQUIRE( exists( p ) );
	remove( p );
	BOOST_REQUIRE( !exists( p ) );
}


BOOST_AUTO_TEST_CASE( delete_on_exit_test_005 ) {
	daw::delete_on_exit tmp;
	BOOST_REQUIRE( tmp );
	std::cout << "Temp file: " << *tmp << std::endl;
	auto out_file = tmp.secure_create_stream( );
	out_file << "Test passed\n";
	out_file->close( );
}


