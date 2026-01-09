// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/libtemp_file
//

#include "daw/temp_file.h"

#include <fstream>
#include <iostream>
#include <iterator>

namespace daw {
	template<typename B>
	constexpr void expecting( B &&b ) {
		if( not static_cast<bool>( b ) ) {
			std::abort( );
		}
	}

} // namespace daw

void shared_temp_file_test_001( ) {
	auto p = daw::fs_impl::fs::path{ };
	{
		auto tmp = daw::shared_temp_file{ };
		daw::expecting( tmp );
		tmp.secure_create_file( );
		std::cout << "Temp file: " << *tmp << std::endl;
		auto out_file = std::ofstream{ tmp->string( ), std::ios::trunc };
		out_file << "Test failed\n";
		out_file.close( );
		p = *tmp;
		daw::expecting( exists( p ) );
	}
	daw::expecting( not exists( p ) );
}

void shared_temp_file_test_002( ) {
	auto p = daw::fs_impl::fs::path{ };
	{
		auto tmp = daw::shared_temp_file{ };
		daw::expecting( tmp );
		std::cout << "Temp file: " << *tmp << std::endl;
		tmp.secure_create_file( );
		auto out_file = std::ofstream{ tmp->string( ), std::ios::trunc };
		out_file << "Test passed\n";
		out_file.close( );
		p = tmp.disconnect( );
	}
	daw::expecting( exists( p ) );
	remove( p );
	daw::expecting( not exists( p ) );
}

void shared_temp_file_test_003( ) {
	auto p = daw::fs_impl::fs::path{ };
	{
		auto tmp = daw::shared_temp_file{ };
		daw::expecting( tmp );
		std::cout << "Temp file: " << *tmp << std::endl;
		auto out_file = tmp.secure_create_stream( );
		out_file << "Test passed\n";
		out_file->close( );
		p = *tmp;
		daw::expecting( exists( p ) );
	}
	daw::expecting( not exists( p ) );
}

void shared_temp_file_test_004( ) {
	auto p = daw::fs_impl::fs::path{ "./" };
	{
		auto tmp = daw::shared_temp_file{ p };
		daw::expecting( tmp );
		std::cout << "Temp file: " << *tmp << std::endl;
		auto out_file = tmp.secure_create_stream( );
		out_file << "Test passed\n";
		out_file->close( );
		p = *tmp;
		daw::expecting( exists( p ) );
	}
	daw::expecting( not exists( p ) );
}

int main( ) {
	shared_temp_file_test_001( );
	shared_temp_file_test_002( );
	shared_temp_file_test_003( );
	shared_temp_file_test_004( );
}
