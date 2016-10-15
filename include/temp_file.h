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
#include <memory>

namespace daw {
	namespace impl {
		struct scoped_temp_file;
	}

	/// @brief Constructs a unique temp file path that can be copied and moved.  It has the same
	///			semantics as a shared_ptr and when the last copy goes out of scope the file, if
	///			it exists is deleted.
	class temp_file {
		std::shared_ptr<impl::scoped_temp_file> m_path;
		boost::filesystem::path & get( );
		boost::filesystem::path const & get( ) const;
	public:
		temp_file( );
		temp_file( temp_file const & ) = default;
		temp_file( temp_file && ) = default;
		temp_file & operator=( temp_file const & ) = default;
		temp_file & operator=( temp_file && ) = default;
		~temp_file( );

		boost::filesystem::path & operator*( );
		boost::filesystem::path const & operator*( ) const;
		boost::filesystem::path const * operator->( ) const;
		boost::filesystem::path * operator->( );
		boost::filesystem::path disconnect( );

		explicit operator bool( ) const;
		bool empty( ) const;
	};	// temp_file
}    // namespace daw

