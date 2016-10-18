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

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <memory>

namespace daw {
	using fd_stream = std::unique_ptr<boost::iostreams::stream<boost::iostreams::file_descriptor>>;

	/// @brief Constructs a unique temp file path that can be moved.  It has the same
	///			semantics as a unique_ptr and when it goes out of scope the file, if
	///			it exists is deleted.
	class unique_temp_file {
		boost::filesystem::path m_path;
	public:
		unique_temp_file( );
		unique_temp_file( boost::filesystem::path p );
		~unique_temp_file( );

		unique_temp_file( unique_temp_file const & ) = delete;
		unique_temp_file( unique_temp_file && ) = default;
		unique_temp_file & operator=( unique_temp_file const & ) = delete;
		unique_temp_file & operator=( unique_temp_file && ) = default;

		/// @brief Get file path
		boost::filesystem::path const & operator*( ) const;

		/// @brief access path members
		boost::filesystem::path const * operator->( ) const;
		
		///	@brief Return path object and do not delete it on exit
		boost::filesystem::path disconnect( );

		/// @brief Remove file and make path empty
		void remove( );

		/// @brief Is a non-empty path stored
		explicit operator bool( ) const;

		explicit operator boost::filesystem::path const & ( ) const;
		explicit operator boost::filesystem::path & ( );

		/// @brief Has object been removed
		bool empty( ) const;

		/// @brief string representation of the tempory file name/path
		std::string string( ) const;

		/// @brief return a file descriptor for a file created with exclusive RW access and 00600 permissions.  Caller must close descriptor
		int secure_create_fd( ) const;

		/// @brief create file with 00600 permissions
		void secure_create_file( ) const;

		using stream = boost::iostreams::stream<boost::iostreams::file_descriptor>; 
		/// @brief return a stream with exclusive RW access and 00600 permissions.  File will close when result goes out of scope 
		fd_stream secure_create_stream( ) const;

		friend bool operator==( unique_temp_file const & lhs, unique_temp_file const & rhs );
		friend bool operator!=( unique_temp_file const & lhs, unique_temp_file const & rhs );
		friend bool operator<( unique_temp_file const & lhs, unique_temp_file const & rhs );
		friend bool operator>( unique_temp_file const & lhs, unique_temp_file const & rhs );
		friend bool operator<=( unique_temp_file const & lhs, unique_temp_file const & rhs );
		friend bool operator>=( unique_temp_file const & lhs, unique_temp_file const & rhs );
	};	// unique_temp_file

	/// @brief Constructs a shareable temp file path that can be copied and moved.  It has the same
	///			semantics as a shared_ptr and when the last copy goes out of scope the file, if
	///			it exists is deleted.
	class shared_temp_file {
		std::shared_ptr<unique_temp_file> m_path;
	public:
		/// @brief Create a non-predicable file name in the temp folder
		shared_temp_file( );

		/// @brief Attach an existing path to delete on scope exit, or if a folder is passed use that as temporary folder with a random file name
		shared_temp_file( boost::filesystem::path p );
		explicit shared_temp_file( unique_temp_file && tmp );
		shared_temp_file & operator=( unique_temp_file && rhs );

		shared_temp_file( shared_temp_file const & ) = default;
		shared_temp_file( shared_temp_file && ) = default;
		shared_temp_file & operator=( shared_temp_file const & ) = default;
		shared_temp_file & operator=( shared_temp_file && ) = default;
		~shared_temp_file( );

		/// @brief Get file path
		boost::filesystem::path const & operator*( ) const;

		/// @brief access path members
		boost::filesystem::path const * operator->( ) const;
		
		///	@brief Return path object and do not delete it on exit
		boost::filesystem::path disconnect( );

		/// @brief Is a non-empty path stored
		explicit operator bool( ) const;

		/// @brief Has object been removed
		bool empty( ) const;

		/// @brief string representation of the tempory file name/path
		std::string string( ) const;

		/// @brief return a file descriptor for a file created with exclusive RW access and 00600 permissions.  Caller must close descriptor
		int secure_create_fd( ) const;

		/// @brief create file with 00600 permissions
		void secure_create_file( ) const;

		/// @brief return a stream with exclusive RW access and 00600 permissions.  File will close when result goes out of scope 
		fd_stream secure_create_stream( ) const;

		friend bool operator==( shared_temp_file const & lhs, shared_temp_file const & rhs );
		friend bool operator!=( shared_temp_file const & lhs, shared_temp_file const & rhs );
		friend bool operator<( shared_temp_file const & lhs, shared_temp_file const & rhs );
		friend bool operator>( shared_temp_file const & lhs, shared_temp_file const & rhs );
		friend bool operator<=( shared_temp_file const & lhs, shared_temp_file const & rhs );
		friend bool operator>=( shared_temp_file const & lhs, shared_temp_file const & rhs );
	};	// shared_temp_file
}    // namespace daw

template<typename T>
daw::fd_stream & operator<<( daw::fd_stream & os, T const & value ) {
	*os << value;
	return os;
}

template<typename T>
daw::fd_stream & operator>>( daw::fd_stream & is, T & value ) {
	*is >> value;
	return is;
}

