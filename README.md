# libdelete_on_exit
A simple interface to create temp files and ensure their removal at program exit.

The class has the same semantics as a shared_ptr and will delete the file when the last copy is destructed.
The class holds a shared_ptr containing boost::filesystem::path and the methods of path can be access via operator->( ) or (*path).method
```
{
	auto tmp = daw::delete_on_exit{ };
	if( !exits( *tmp ) ) {
		std::ofstream out_file{ tmp.string( ) };
		out_file << "Will delete" << std::endl;
	}
}
```
To prevent the file from being deleted, you can call disconnect( ) and it will return the path object and no longer delete the file on destruction
