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
	};	// hash
}

namespace daw {
	namespace impl {
		struct scoped_temp_file {
			boost::filesystem::path path;
			boost::filesystem::path disconnect( );
			scoped_temp_file( );
			~scoped_temp_file( ) noexcept;
		};	// scoped_temp_file

		namespace {
			class temp_file_holder_t {
				std::unordered_set<boost::filesystem::path> m_paths;
				std::mutex m_mutex;
				public:
				temp_file_holder_t( ) = default;
				temp_file_holder_t( temp_file_holder_t const & ) = delete;
				temp_file_holder_t( temp_file_holder_t && ) = default;
				temp_file_holder_t & operator=( temp_file_holder_t const & ) = delete;
				temp_file_holder_t & operator=( temp_file_holder_t && ) = default;

				~temp_file_holder_t( ) {
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
			};	// temp_file_holder_t

			auto & temp_files( ) {
				static impl::temp_file_holder_t result;
				return result;
			}

			boost::filesystem::path create_temp_file( ) {
				auto result = boost::filesystem::temp_directory_path( ) / boost::filesystem::unique_path( ).replace_extension( ".tmp" );
				impl::temp_files( ).insert( result );
				return result;
			}

			void delete_temp_file( boost::filesystem::path const & path ) {
				if( exists( path ) ) {
					remove( path );
					impl::temp_files( ).erase( path );
				}
			}
		}	// namespace impl
	}	// namespace anonymous

	boost::filesystem::path & temp_file::get( ) {
		return m_path->path;
	}

	boost::filesystem::path const & temp_file::get( ) const {
		return m_path->path;
	}

	temp_file::temp_file( ):
			m_path{ std::make_shared<daw::impl::scoped_temp_file>( ) } { }

	temp_file::~temp_file( ) { }

	boost::filesystem::path & temp_file::operator*( ) {
		return get( );
	}

	boost::filesystem::path const & temp_file::operator*( ) const {
		return get( );
	}

	boost::filesystem::path * temp_file::operator->( ) {
		return &(get( ));
	}

	boost::filesystem::path const * temp_file::operator->( ) const {
		return &(get( ));
	}

	boost::filesystem::path temp_file::disconnect( ) {
		if( m_path ) {
			return m_path->disconnect( );
		}
		return boost::filesystem::path{ };
	}

	temp_file::operator bool( ) const {
		return m_path && get( ) != boost::filesystem::path{ };
	}

	bool temp_file::empty( ) const {
		if( m_path ) {
			return get( ) == boost::filesystem::path{ };
		}
		return true;
	}

	namespace impl {
		scoped_temp_file::scoped_temp_file( ):
			path{ impl::create_temp_file( ) } { }

		boost::filesystem::path scoped_temp_file::disconnect( ) {
			auto result = std::exchange( path, boost::filesystem::path{ } );
			impl::temp_files( ).erase( result );
			return result;
		}

		scoped_temp_file::~scoped_temp_file( ) noexcept {
			try {
				if( path != boost::filesystem::path{ } ) {
					auto tmp = std::exchange( path, boost::filesystem::path{ } );
					impl::delete_temp_file( tmp );
				}
			} catch( std::exception const & ) { }
		}
	}
}    // namespace daw

