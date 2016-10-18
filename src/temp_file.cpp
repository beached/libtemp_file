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

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#define fileopen _open
#define fileclose _close
#else
#define fileopen open
#define fileclose close
#endif
#include <sys/stat.h>
#include <sys/types.h>

#include "temp_file.h"




namespace daw {
	namespace {
	constexpr mode_t sec_perm( ) {
#ifdef WIN32
		return _S_IREAD | _S_IWRITE;
#else
		return	S_IRUSR | S_IWUSR; 
#endif
	}
		auto generate_temp_file_path( boost::filesystem::path temp_folder = boost::filesystem::temp_directory_path( ) ) {
			return temp_folder / boost::filesystem::unique_path( ).replace_extension( ".tmp" );
		}
	}	// namespace anonymous
		
	unique_temp_file::unique_temp_file( ):
			m_path{ generate_temp_file_path( ) } { }

	unique_temp_file::unique_temp_file( boost::filesystem::path p ):
			m_path{ is_directory( p ) ? generate_temp_file_path( p ) : std::move( p ) } { }

	boost::filesystem::path unique_temp_file::disconnect( ) {
		return std::exchange( m_path, boost::filesystem::path{ } );
	}

	unique_temp_file::~unique_temp_file( ) {
		try {
			remove( );
		} catch( std::exception const & ) { }
	}

	boost::filesystem::path const & unique_temp_file::operator*( ) const {
		return m_path;
	}

	boost::filesystem::path const * unique_temp_file::operator->( ) const {
		return &m_path;
	}

	void unique_temp_file::remove( ) {
		if( !empty( ) ) {
			auto tmp = std::exchange( m_path, boost::filesystem::path{ } );
			boost::filesystem::remove( tmp );
		}
	}

	unique_temp_file::operator bool( ) const {
		return !empty( );
	}

	std::string unique_temp_file::string( ) const {
		return m_path.string( );
	}

	bool unique_temp_file::empty( ) const {
		return m_path.empty( );
	}

	int unique_temp_file::secure_create_fd( ) const {
		if( empty( ) ) {
			throw std::runtime_error{ "Attempt to create a file from empty path" };
		}
		auto fd = fileopen( string( ).c_str( ), O_CREAT | O_RDWR | O_EXCL, sec_perm( ) );
		if( fd < 0 ) {
			throw std::runtime_error{ "Could not create temp file" };
		} else if( !exists( m_path ) ) {
			throw std::runtime_error{ "Failed to create temp file" };
		} else if( !is_regular_file( m_path ) ) {
			throw std::runtime_error( "Temp file was not a regular file.  This should never happen as the file was to be uniquely named" );
		}
		return fd;
	}

	void unique_temp_file::secure_create_file( ) const {
		fileclose( secure_create_fd( ) );
	}

	fd_stream unique_temp_file::secure_create_stream( ) const {
		return std::make_unique<stream>( secure_create_fd( ), boost::iostreams::file_descriptor_flags::close_handle );
	}

	bool operator==( unique_temp_file const & lhs, unique_temp_file const & rhs ) {
		return *lhs == *rhs;
	}

	bool operator!=( unique_temp_file const & lhs, unique_temp_file const & rhs ) {
		return *lhs != *rhs;
	}

	bool operator<( unique_temp_file const & lhs, unique_temp_file const & rhs ) {
		return *lhs < *rhs;
	}

	bool operator>( unique_temp_file const & lhs, unique_temp_file const & rhs ) {
		return *lhs > *rhs;
	}

	bool operator<=( unique_temp_file const & lhs, unique_temp_file const & rhs ) {
		return *lhs <= *rhs;
	}

	bool operator>=( unique_temp_file const & lhs, unique_temp_file const & rhs ) {
		return *lhs >= *rhs;
	}

	shared_temp_file::shared_temp_file( ):
			m_path{ std::make_shared<unique_temp_file>( ) } { }

	shared_temp_file::shared_temp_file( boost::filesystem::path p ):
			m_path{ std::make_shared<unique_temp_file>( std::move( p ) ) } { }

	shared_temp_file::shared_temp_file( unique_temp_file && tmp ):
		m_path{ std::make_shared<unique_temp_file>( std::move( tmp ) ) } { }

	shared_temp_file & shared_temp_file::operator=( unique_temp_file && rhs ) {
		m_path.reset( );
		m_path = std::make_shared<unique_temp_file>( std::move( rhs ) );	
		return *this;
	}

	shared_temp_file::~shared_temp_file( ) { }

	namespace {
		boost::filesystem::path const & get( unique_temp_file const & tmp ) {
			return *tmp;
		}
	}

	boost::filesystem::path const & shared_temp_file::operator*( ) const {
		return get( *m_path );
	}

	boost::filesystem::path const * shared_temp_file::operator->( ) const {
		return &get( *m_path );
	}

	boost::filesystem::path shared_temp_file::disconnect( ) {
		return m_path->disconnect( );
	}

	shared_temp_file::operator bool( ) const {
		return !empty( );
	}

	std::string shared_temp_file::string( ) const {
		if( m_path ) {
			return m_path->string( );
		}
		return std::string{ };
	}

	bool shared_temp_file::empty( ) const {
		return !m_path || m_path->empty( );
	}

	int shared_temp_file::secure_create_fd( ) const {
		return m_path->secure_create_fd( );
	}

	void shared_temp_file::secure_create_file( ) const {
		return m_path->secure_create_file( );
	}

	fd_stream shared_temp_file::secure_create_stream( ) const {
		return m_path->secure_create_stream( );
	}

	bool operator==( shared_temp_file const & lhs, shared_temp_file const & rhs ) {
		return *lhs == *rhs;
	}

	bool operator!=( shared_temp_file const & lhs, shared_temp_file const & rhs ) {
		return *lhs != *rhs;
	}

	bool operator<( shared_temp_file const & lhs, shared_temp_file const & rhs ) {
		return *lhs < *rhs;
	}

	bool operator>( shared_temp_file const & lhs, shared_temp_file const & rhs ) {
		return *lhs > *rhs;
	}

	bool operator<=( shared_temp_file const & lhs, shared_temp_file const & rhs ) {
		return *lhs <= *rhs;
	}

	bool operator>=( shared_temp_file const & lhs, shared_temp_file const & rhs ) {
		return *lhs >= *rhs;
	}
}    // namespace daw

