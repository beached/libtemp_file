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

#include "delete_on_exit.h"

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
		namespace {
			class delete_on_exit_holder_t;
			delete_on_exit_holder_t & delete_on_exits( );
			boost::filesystem::path create_delete_on_exit( );
			void delete_delete_on_exit( boost::filesystem::path const & path );
		}
		struct scoped_delete_on_exit {
			boost::filesystem::path path;


			scoped_delete_on_exit( ):
				path{ create_delete_on_exit( ) } { }

			scoped_delete_on_exit( boost::filesystem::path p );
			boost::filesystem::path disconnect( );
			~scoped_delete_on_exit( ) noexcept;
		};	// scoped_delete_on_exit

		namespace {
			class delete_on_exit_holder_t {
				std::unordered_set<boost::filesystem::path> m_paths;
				std::mutex m_mutex;
				public:
				delete_on_exit_holder_t( ) = default;
				delete_on_exit_holder_t( delete_on_exit_holder_t const & ) = delete;
				delete_on_exit_holder_t( delete_on_exit_holder_t && ) = default;
				delete_on_exit_holder_t & operator=( delete_on_exit_holder_t const & ) = delete;
				delete_on_exit_holder_t & operator=( delete_on_exit_holder_t && ) = default;

				~delete_on_exit_holder_t( ) {
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
			};	// delete_on_exit_holder_t
		}	// namespace anonymous

		scoped_delete_on_exit::scoped_delete_on_exit( boost::filesystem::path p ):
				path{ std::move( p ) } {

			delete_on_exits( ).insert( path );
		}

		boost::filesystem::path scoped_delete_on_exit::disconnect( ) {
			auto result = std::exchange( path, boost::filesystem::path{ } );
			delete_on_exits( ).erase( result );
			return result;
		}

		scoped_delete_on_exit::~scoped_delete_on_exit( ) noexcept {
			try {
				if( path != boost::filesystem::path{ } ) {
					auto tmp = std::exchange( path, boost::filesystem::path{ } );
					delete_delete_on_exit( tmp );
				}
			} catch( std::exception const & ) { }
		}


		namespace {
			impl::delete_on_exit_holder_t & delete_on_exits( ) {
				static impl::delete_on_exit_holder_t result;
				return result;
			}

			boost::filesystem::path create_delete_on_exit( ) {
				auto result = boost::filesystem::temp_directory_path( ) / boost::filesystem::unique_path( ).replace_extension( ".tmp" );
				impl::delete_on_exits( ).insert( result );
				return result;
			}

			void delete_delete_on_exit( boost::filesystem::path const & path ) {
				if( exists( path ) ) {
					remove( path );
					impl::delete_on_exits( ).erase( path );
				}
			}
		}	// namespace anonymous
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
		return m_path && get( ) != boost::filesystem::path{ };
	}

	bool delete_on_exit::empty( ) const {
		if( m_path ) {
			return get( ) == boost::filesystem::path{ };
		}
		return true;
	}
}    // namespace daw

