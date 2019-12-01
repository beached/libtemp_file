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

#pragma once

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <exception>
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility>

#ifndef NO_STD_FS
#include <filesystem>
#endif

#ifdef WIN32
#include <io.h>
#define DAW_FILE_OPEN _open
#define DAW_FILE_CLOSE _close
#else
#define DAW_FILE_OPEN open
#define DAW_FILE_CLOSE close
#endif

namespace daw {
	namespace fs_impl {
#ifndef NO_STD_FS
		namespace fs = std::filesystem;
#else
		namespace fs = boost::filesystem;
#endif

		[[nodiscard]] constexpr auto sec_perm( ) {
#ifdef WIN32
			return _S_IREAD | _S_IWRITE;
#else
			return S_IRUSR | S_IWUSR;
#endif
		}

		template<typename Path>
		[[nodiscard]] Path generate_temp_file_path( Path const &temp_folder ) {
			boost::filesystem::path result =
			  boost::filesystem::unique_path( ).replace_extension( ".tmp" );
			if constexpr( std::is_same_v<fs::path, boost::filesystem::path> ) {
				return temp_folder / result;
			} else {
				return temp_folder / fs::path( result.native( ) );
			}
		}

		template<typename Path>
		[[nodiscard]] Path generate_temp_file_path( ) {
			return generate_temp_file_path( fs::temp_directory_path( ) );
		}

	} // namespace fs_impl

	using fd_stream = std::unique_ptr<
	  boost::iostreams::stream<boost::iostreams::file_descriptor>>;

	/// @brief Constructs a unique temp file path that can be moved.  It has the
	/// same
	///			semantics as a unique_ptr and when it goes out of scope the file, if
	///			it exists is deleted.
	template<typename Path>
	struct unique_temp_file {
		using path_type = Path;

	private:
		path_type m_path = fs_impl::generate_temp_file_path<path_type>( );

	public:
		unique_temp_file( ) = default;

		unique_temp_file( path_type p )
		  : m_path( is_directory( p )
		              ? fs_impl::generate_temp_file_path<path_type>( p )
		              : std::move( p ) ) {}

		~unique_temp_file( ) {
			try {
				remove( );
			} catch( std::exception const & ) {
				// TODO don't do this
			}
		}

		unique_temp_file( unique_temp_file &&other ) noexcept
		  : m_path( other.disconnect( ) ) {}

		unique_temp_file &operator=( unique_temp_file &&rhs ) noexcept {
			if( this != &rhs ) {
				m_path = rhs.disconnect( );
			}
			return *this;
		}

		// Move only type
		unique_temp_file( unique_temp_file const & ) = delete;
		unique_temp_file &operator=( unique_temp_file const & ) = delete;

		/// @brief Get file path
		[[nodiscard]] path_type const &operator*( ) const noexcept {
			return m_path;
		}

		/// @brief access path members
		[[nodiscard]] path_type const *operator->( ) const noexcept {
			return &m_path;
		}

		///	@brief Return path object and do not delete it on exit

		[[nodiscard]] path_type disconnect( ) {
			return std::exchange( m_path, path_type{} );
		}

		/// @brief Remove file and make path empty
		void remove( ) {
			if( not empty( ) ) {
				auto tmp = std::exchange( m_path, fs_impl::fs::path{} );
				fs_impl::fs::remove( tmp );
			}
		}

		/// @brief Is a non-empty path stored
		[[nodiscard]] explicit operator bool( ) const noexcept {
			return not empty( );
		}

		[[nodiscard]] explicit operator path_type const &( ) const noexcept {
			return m_path;
		}

		/// @brief Has object been removed
		[[nodiscard]] bool empty( ) const noexcept {
			return m_path.empty( );
		}

		/// @brief string representation of the tempory file name/path
		[[nodiscard]] auto native( ) const noexcept {
			return m_path.native( );
		}

		/// @brief return a file descriptor for a file created with exclusive RW
		/// access and 00600 permissions.  Caller must close descriptor
		[[nodiscard]] int secure_create_fd( ) const {
			if( empty( ) ) {
				throw std::runtime_error{"Attempt to create a file from empty path"};
			}
			auto fd = DAW_FILE_OPEN( native( ).c_str( ), O_CREAT | O_RDWR | O_EXCL,
			                         fs_impl::sec_perm( ) );
			if( fd < 0 ) {
				throw std::runtime_error{"Could not create temp file"};
			} else if( not exists( m_path ) ) {
				throw std::runtime_error{"Failed to create temp file"};
			} else if( not is_regular_file( m_path ) ) {
				throw std::runtime_error(
				  "Temp file was not a regular file.  This should never happen as the "
				  "file was to be uniquely named" );
			}
			return fd;
		}

		/// @brief create file with 00600 permissions
		void secure_create_file( ) const {
			DAW_FILE_CLOSE( secure_create_fd( ) );
		}

		using stream = boost::iostreams::stream<boost::iostreams::file_descriptor>;
		/// @brief return a stream with exclusive RW access and 00600 permissions.
		/// File will close when result goes out of scope
		[[nodiscard]] fd_stream secure_create_stream( ) const {
			return std::make_unique<stream>(
			  secure_create_fd( ),
			  boost::iostreams::file_descriptor_flags::close_handle );
		}
	}; // unique_temp_file

	template<typename Path>
	unique_temp_file( Path )->unique_temp_file<Path>;

	unique_temp_file( )->unique_temp_file<fs_impl::fs::path>;

	template<typename Path>
	bool operator==( unique_temp_file<Path> const &lhs,
	                 unique_temp_file<Path> const &rhs ) noexcept {
		return *lhs == *rhs;
	}

	template<typename Path>
	bool operator!=( unique_temp_file<Path> const &lhs,
	                 unique_temp_file<Path> const &rhs ) noexcept {
		return *lhs != *rhs;
	}

	template<typename Path>
	bool operator<( unique_temp_file<Path> const &lhs,
	                unique_temp_file<Path> const &rhs ) noexcept {
		return *lhs < *rhs;
	}

	template<typename Path>
	bool operator>( unique_temp_file<Path> const &lhs,
	                unique_temp_file<Path> const &rhs ) noexcept {
		return *lhs > *rhs;
	}

	template<typename Path>
	bool operator<=( unique_temp_file<Path> const &lhs,
	                 unique_temp_file<Path> const &rhs ) noexcept {
		return *lhs <= *rhs;
	}

	template<typename Path>
	bool operator>=( unique_temp_file<Path> const &lhs,
	                 unique_temp_file<Path> const &rhs ) noexcept {
		return *lhs >= *rhs;
	}

	/// @brief Constructs a shareable temp file path that can be copied and moved.
	/// It has the same
	///			semantics as a shared_ptr and when the last copy goes out of scope the
	/// file, if 			it exists is deleted.
	template<typename Path = fs_impl::fs::path>
	struct shared_temp_file {
		using path_type = Path;

	private:
		std::shared_ptr<unique_temp_file<path_type>> m_path =
		  std::make_shared<unique_temp_file<path_type>>( );

	public:
		/// @brief Create a non-predicable file name in the temp folder
		shared_temp_file( ) = default;

		/// @brief Attach an existing path to delete on scope exit, or if a folder
		/// is passed use that as temporary folder with a random file name
		shared_temp_file( path_type p )
		  : m_path(
		      std::make_shared<unique_temp_file<path_type>>( std::move( p ) ) ) {}

		shared_temp_file( unique_temp_file<path_type> &&tmp )
		  : m_path( std::make_shared<unique_temp_file>( tmp.disconnect( ) ) ) {}

		shared_temp_file &operator=( unique_temp_file<path_type> &&rhs ) {
			m_path.reset( );
			m_path = std::make_shared<unique_temp_file>( rhs.disconnect( ) );
			return *this;
		}

		/// @brief Get file path
		[[nodiscard]] path_type const &operator*( ) const noexcept {
			return m_path->operator*( );
		}

		/// @brief access path members
		[[nodiscard]] path_type const *operator->( ) const noexcept {
			return m_path->operator->( );
		}

		///	@brief Return path object and do not delete it on exit
		[[nodiscard]] path_type disconnect( ) {
			return m_path->disconnect( );
		}

		/// @brief Is a non-empty path stored
		[[nodiscard]] explicit operator bool( ) const noexcept {
			return m_path->operator bool( );
		}

		/// @brief Has object been removed
		[[nodiscard]] bool empty( ) const noexcept {
			return not m_path or m_path->empty( );
		}

		/// @brief string representation of the tempory file name/path
		[[nodiscard]] auto native( ) const noexcept {
			if( m_path ) {
				return m_path->native( );
			}
			return typename path_type::string_type{};
		}

		/// @brief return a file descriptor for a file created with exclusive RW
		/// access and 00600 permissions.  Caller must close descriptor
		[[nodiscard]] int secure_create_fd( ) const {
			if( empty( ) ) {
				throw std::runtime_error{"Attempt to create a file from empty path"};
			}
			return m_path->secure_create_fd( );
		}

		/// @brief create file with 00600 permissions
		void secure_create_file( ) const {
			if( empty( ) ) {
				throw std::runtime_error{"Attempt to create a file from empty path"};
			}
			m_path->secure_create_file( );
		}

		/// @brief return a stream with exclusive RW access and 00600 permissions.
		/// File will close when result goes out of scope
		[[nodiscard]] fd_stream secure_create_stream( ) const {
			return m_path->secure_create_stream( );
		}
	}; // shared_temp_file

	template<typename Path>
	shared_temp_file( Path )->shared_temp_file<Path>;

	shared_temp_file( )->shared_temp_file<fs_impl::fs::path>;

	template<typename Path>
	bool operator==( shared_temp_file<Path> const &lhs,
	                 shared_temp_file<Path> const &rhs ) noexcept {
		if( lhs ) {
			if( rhs ) {
				return *lhs == *rhs;
			}
		}
		return false;
	}

	template<typename Path>
	bool operator!=( shared_temp_file<Path> const &lhs,
	                 shared_temp_file<Path> const &rhs ) noexcept {
		if( lhs ) {
			if( rhs ) {
				return *lhs != *rhs;
			}
		}
		return true;
	}

	template<typename Path>
	bool operator<( shared_temp_file<Path> const &lhs,
	                shared_temp_file<Path> const &rhs ) noexcept {
		if( rhs ) {
			if( lhs ) {
				return *lhs < *rhs;
			}
			return true;
		}
		return false;
	}

	template<typename Path>
	bool operator>( shared_temp_file<Path> const &lhs,
	                shared_temp_file<Path> const &rhs ) noexcept {
		if( lhs ) {
			if( rhs ) {
				return *lhs > *rhs;
			}
			return true;
		}
		return false;
	}

	template<typename Path>
	bool operator<=( shared_temp_file<Path> const &lhs,
	                 shared_temp_file<Path> const &rhs ) noexcept {
		if( lhs ) {
			if( rhs ) {
				return *lhs <= *rhs;
			}
			return false;
		}
		return true;
	}

	template<typename Path>
	bool operator>=( shared_temp_file<Path> const &lhs,
	                 shared_temp_file<Path> const &rhs ) noexcept {
		if( rhs ) {
			if( lhs ) {
				return *lhs >= *rhs;
			}
			return false;
		}
		return true;
	}
} // namespace daw

template<typename T>
daw::fd_stream &operator<<( daw::fd_stream &os, T const &value ) {
	*os << value;
	return os;
}

template<typename T>
daw::fd_stream &operator>>( daw::fd_stream &is, T &value ) {
	*is >> value;
	return is;
}

#undef DAW_FILE_OPEN
#undef DAW_FILE_CLOSE
