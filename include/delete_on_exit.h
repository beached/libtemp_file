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
	namespace impl {
		struct scoped_delete_on_exit;
	}

	/// @brief Constructs a unique temp file path that can be copied and moved.  It has the same
	///			semantics as a shared_ptr and when the last copy goes out of scope the file, if
	///			it exists is deleted.
	class delete_on_exit {
		std::shared_ptr<impl::scoped_delete_on_exit> m_path;
		boost::filesystem::path & get( );
		boost::filesystem::path const & get( ) const;
	public:
		delete_on_exit( );
		delete_on_exit( boost::filesystem::path p );
		delete_on_exit( delete_on_exit const & ) = default;
		delete_on_exit( delete_on_exit && ) = default;
		delete_on_exit & operator=( delete_on_exit const & ) = default;
		delete_on_exit & operator=( delete_on_exit && ) = default;
		~delete_on_exit( );

		boost::filesystem::path & operator*( );
		boost::filesystem::path const & operator*( ) const;
		boost::filesystem::path const * operator->( ) const;
		boost::filesystem::path * operator->( );

		///	@brief Return path object and do not delete it on exit
		boost::filesystem::path disconnect( );

		explicit operator bool( ) const;
		explicit operator boost::filesystem::path const & ( ) const;
		explicit operator boost::filesystem::path & ( );
		bool empty( ) const;

		/// @brief string representation of the tempory file name/path
		std::string string( ) const;

		/// @brief return a file descriptor for a file created with exclusive RW access and 00600 permissions.  Caller must close descriptor
		int secure_create_fd( ) const;

		/// @brief create file with 00600 permissions
		void secure_create_file( ) const;

		using stream = boost::iostreams::stream<boost::iostreams::file_descriptor>; 
		/// @brief return a stream with exclusive RW access and 00600 permissions.  File will close when result goes out of scope 
		std::unique_ptr<stream> secure_create_stream( ) const;
	};	// delete_on_exit
}    // namespace daw

template<typename T>
std::unique_ptr<daw::delete_on_exit::stream> & operator<<( std::unique_ptr<daw::delete_on_exit::stream> & os, T const & value ) {
	*os << value;
	return os;
}

template<typename T>
std::unique_ptr<daw::delete_on_exit::stream> & operator>>( std::unique_ptr<daw::delete_on_exit::stream> & is, T & value ) {
	*is >> value;
	return is;
}

