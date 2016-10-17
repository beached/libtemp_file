# libtemp_file
A simple interface to create temp files and ensure their removal at program exit.

The class has the same semantics as a shared_ptr and will delete the file when the last copy is destructed.
The class holds a shared_ptr containing boost::filesystem::path and the methods of path can be access via operator->( ) or (*path).method
```
{
	auto tmp = daw::unique_temp_file{ };
	auto out_file = tmp.create_secure_stream( );
	if( out_file ) {
		out_file << "Will delete" << std::endl;
	}
	out_file.close( );
}
```
The create_secure_stream( ) method will create a new file with exclusive access and with read and write access.  The permissions will be 00600 so that the current user is the only id with access.
To prevent the file from being deleted, you can call disconnect( ) and it will return the path object and no longer delete the file on destruction
