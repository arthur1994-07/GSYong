
#ifndef _mrcFileSystem_h_
#define _mrcFileSystem_h_

#include "mrcConfig.h"

#include <boost/filesystem.hpp>


namespace mrc
{

	namespace filesystem
	{
	

#ifdef _UNICODE	
		typedef boost::filesystem::wpath Path;	
		//	typedef boost::filesystem::basic_path< String, wpath_traits > FilePath;
		typedef boost::filesystem::wdirectory_iterator DirectoryIterator;

#else
		typedef boost::filesystem::path Path;
		typedef boost::filesystem::directory_iterator DirectoryIterator;

#endif
		typedef boost::filesystem::basic_directory_iterator< Path > DirectoryIterator;

		inline time_t last_write_time(const Path& path) 
		{
			return boost::filesystem::last_write_time(path); 
		}

		typedef boost::filesystem::file_status file_status;

		inline Path initial_path() { return boost::filesystem::initial_path<Path>(); }
		inline bool exists(const file_status& status) { return boost::filesystem::exists(status); }
		inline bool is_directory(const file_status& status) { return boost::filesystem::is_directory(status); }
		inline bool is_directory(const Path& path) { return boost::filesystem::is_directory(path); }
		inline bool is_regular_file(const file_status& status) { return boost::filesystem::is_regular_file(status); }

	}
 
}

#endif


