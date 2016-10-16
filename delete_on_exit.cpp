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

#include "delete_on_exit.h"

namespace daw {
	namespace impl {
		namespace {
			auto generate_temp_file_path( ) {
				return boost::filesystem::temp_directory_path( ) / boost::filesystem::unique_path( ).replace_extension( ".tmp" );
			}
		}	// namespace anonymous
		
		struct scoped_delete_on_exit {
			boost::filesystem::path path;

			scoped_delete_on_exit( ):
				path{ generate_temp_file_path( ) } { }

			scoped_delete_on_exit( boost::filesystem::path p ):
				path{ std::move( p ) } { }

			boost::filesystem::path disconnect( ) {
				return std::exchange( path, boost::filesystem::path{ } );
			}

			~scoped_delete_on_exit( ) {
				try {
					if( !path.empty( ) && exists( path ) ) {
						remove( path );
					}
				} catch( std::exception const & ) { }
			}

		};	// scoped_delete_on_exit
	}	// namespace impl

	boost::filesystem::path & delete_on_exit::get( ) {
		return m_path->path;
	}

	boost::filesystem::path const & delete_on_exit::get( ) const {
		return m_path->path;
	}

	delete_on_exit::delete_on_exit( ):
			m_path{ std::make_shared<daw::impl::scoped_delete_on_exit>( ) } { }

	
	delete_on_exit::delete_on_exit( boost::filesystem::path p ):
			m_path{ std::make_shared<daw::impl::scoped_delete_on_exit>( std::move( p ) ) } { }

	delete_on_exit::~delete_on_exit( ) { }

	boost::filesystem::path & delete_on_exit::operator*( ) {
		return get( );
	}

	boost::filesystem::path const & delete_on_exit::operator*( ) const {
		return get( );
	}

	boost::filesystem::path * delete_on_exit::operator->( ) {
		return &(get( ));
	}

	boost::filesystem::path const * delete_on_exit::operator->( ) const {
		return &(get( ));
	}

	boost::filesystem::path delete_on_exit::disconnect( ) {
		if( m_path ) {
			return m_path->disconnect( );
		}
		return boost::filesystem::path{ };
	}

	delete_on_exit::operator bool( ) const {
		return m_path && !get( ).empty( );
	}

	delete_on_exit::operator boost::filesystem::path const & ( ) const {
		return get( );
	}

	delete_on_exit::operator boost::filesystem::path & ( ) {
		return get( );
	}

	std::string delete_on_exit::string( ) const {
		if( m_path ) {
			return get( ).string( );
		}
		return std::string{ };
	}

	bool delete_on_exit::empty( ) const {
		return !m_path || get( ).empty( );
	}
}    // namespace daw

