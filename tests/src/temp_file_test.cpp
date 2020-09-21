// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/libtemp_file
//

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
