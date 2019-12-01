// The MIT License (MIT)
//
// Copyright (c) 2016-2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <fstream>
#include <iostream>
#include <iterator>

//#include <daw/daw_benchmark.h>

#include "daw/temp_file.h"

namespace daw {
	template<typename B>
	constexpr void expecting( B &&b ) {
		if( not static_cast<bool>( b ) ) {
			std::abort( );
		}
	}

} // namespace daw

void shared_temp_file_test_001( ) {
	daw::fs_impl::fs::path p;
	{
		daw::shared_temp_file tmp;
		daw::expecting( tmp );
		tmp.secure_create_file( );
		std::cout << "Temp file: " << *tmp << std::endl;
		std::ofstream out_file{tmp->string( ), std::ios::trunc};
		out_file << "Test failed\n";
		out_file.close( );
		p = *tmp;
		daw::expecting( exists( p ) );
	}
	daw::expecting( !exists( p ) );
}

void shared_temp_file_test_002( ) {
	daw::fs_impl::fs::path p;
	{
		daw::shared_temp_file tmp;
		daw::expecting( tmp );
		std::cout << "Temp file: " << *tmp << std::endl;
		tmp.secure_create_file( );
		std::ofstream out_file{tmp->string( ), std::ios::trunc};
		out_file << "Test passed\n";
		out_file.close( );
		p = tmp.disconnect( );
	}
	daw::expecting( exists( p ) );
	remove( p );
	daw::expecting( !exists( p ) );
}

void shared_temp_file_test_003( ) {
	daw::fs_impl::fs::path p;
	{
		daw::shared_temp_file tmp;
		daw::expecting( tmp );
		std::cout << "Temp file: " << *tmp << std::endl;
		auto out_file = tmp.secure_create_stream( );
		out_file << "Test passed\n";
		out_file->close( );
		p = *tmp;
		daw::expecting( exists( p ) );
	}
	daw::expecting( !exists( p ) );
}

void shared_temp_file_test_004( ) {
	daw::fs_impl::fs::path p{"./"};
	{
		daw::shared_temp_file tmp{p};
		daw::expecting( tmp );
		std::cout << "Temp file: " << *tmp << std::endl;
		auto out_file = tmp.secure_create_stream( );
		out_file << "Test passed\n";
		out_file->close( );
		p = *tmp;
		daw::expecting( exists( p ) );
	}
	daw::expecting( !exists( p ) );
}

int main( ) {
	shared_temp_file_test_001( );
	shared_temp_file_test_002( );
	shared_temp_file_test_003( );
	shared_temp_file_test_004( );
}
