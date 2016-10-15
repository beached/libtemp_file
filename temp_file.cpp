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
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_set>

#include "temp_file.h"

namespace std {
	template<>
		class hash<boost::filesystem::path> {
			std::hash<std::string> m_hash;
		public:
			auto operator()( boost::filesystem::path const & path ) const {
				return m_hash( path.string( ) );
			}
		};
}

namespace daw {
	namespace impl {
		class tmp_file_holder_t {
			std::unordered_set<boost::filesystem::path> m_paths;
			std::mutex m_mutex;
		public:
			tmp_file_holder_t( ) = default;
			tmp_file_holder_t( tmp_file_holder_t const & ) = delete;
			tmp_file_holder_t( tmp_file_holder_t && ) = default;
			tmp_file_holder_t & operator=( tmp_file_holder_t const & ) = delete;
			tmp_file_holder_t & operator=( tmp_file_holder_t && ) = default;

			~tmp_file_holder_t( ) {
				try {
					for( auto const & path : m_paths ) {
						try {
							if( exists( path ) ) {
								remove( path );
							}
						} catch( std::exception const & ex ) {
							std::cerr << "Exception while removing tmp files: " << ex.what( ) << std::endl;
						}
					}
				} catch(...) { }
			}

			void insert( boost::filesystem::path p ) {
				std::lock_guard<std::mutex> lock{ m_mutex };
				m_paths.insert( std::move( p ) );
			}

			void erase( boost::filesystem::path const & p ) {
				std::lock_guard<std::mutex> lock{ m_mutex };
				m_paths.erase( p );
			}
		};	// tmp_file_holder_t

		auto & tmp_files( ) {
			static impl::tmp_file_holder_t result;
			return result;
		}
	}	// namespace impl

	boost::filesystem::path create_tmpfile( ) {
		auto result = boost::filesystem::temp_directory_path( ) / boost::filesystem::unique_path( ).replace_extension( ".tmp" );
		impl::tmp_files( ).insert( result );
		return result;
	}

	void delete_tmpfile( boost::filesystem::path const & path ) {
		if( exists( path ) ) {
			remove( path );
			impl::tmp_files( ).erase( path );
		}
	}

	scoped_tmpfile::scoped_tmpfile( ):
			path{ create_tmpfile( ) } { }

	boost::filesystem::path scoped_tmpfile::reset( ) {
		auto result = std::exchange( path, boost::filesystem::path{ } );
		impl::tmp_files( ).erase( result );
		return result;
	}

	scoped_tmpfile::~scoped_tmpfile( ) noexcept {
		try {
			if( path != boost::filesystem::path{ } ) {
				auto tmp = std::exchange( path, boost::filesystem::path{ } );
				delete_tmpfile( tmp );
			}
		} catch( std::exception const & ) { }
	}
}    // namespace daw

