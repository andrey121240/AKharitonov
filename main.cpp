#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp> 

namespace fs = boost::filesystem;
std::string test;

struct Fileinfo {
	std::string path;
	std::string hash;
	int size;
	std::string flag;
};


void save2bsr(std::string filename, std::vector<Fileinfo> vec_finfo) {
	std::ofstream ofs("filename.bsr"); 
	boost::archive::text_oarchive oa(ofs);
	std::string hash = "NULL";
	for (Fileinfo it : vec_finfo) {  
		oa << it.path;
		oa << std::to_string(it.size);
		oa << hash; 
	}

	ofs.close();
}

void read() {
	std::ifstream ofs("filename.bsr");
	boost::archive::text_iarchive oa(ofs);
	std::string test;
	while (true) {
		try
		{
			oa >> test;
			std::cout << test << std::endl;
		}
		catch (boost::archive::archive_exception& ex)
		{
			break;
		}
	}
	ofs.close();
}

void get_dir_list(fs::directory_iterator iterator, std::vector<Fileinfo> * vec_finfo) {  
	Fileinfo finfo;
	for (; iterator != fs::directory_iterator(); ++iterator)
	{
		if (fs::is_directory(iterator->status())) { 
			fs::directory_iterator sub_dir(iterator->path());
			get_dir_list(sub_dir, vec_finfo);

		}
		else 
		{
			finfo.path = iterator->path().string();
			std::replace(finfo.path.begin(), finfo.path.end(), '\\', '/'); 
			finfo.size = fs::file_size(iterator->path());
			finfo.hash = "NOT YET";
			finfo.flag = 'R';
			vec_finfo->push_back(finfo);
		}

	}
}

int main() {
	std::ofstream myfile;
	std::string path, dirpath;
	std::cout << "Folder path:" << std::endl;
	std::getline(std::cin, path);
	std::vector<Fileinfo> vec_finfo;
	std::vector<Fileinfo> vec_finfo_old;
	fs::directory_iterator home_dir(path);
	get_dir_list(home_dir, &vec_finfo); 

	
	save2bsr("example.xml", vec_finfo); 
	read();
	std::cin.clear();
	fflush(stdin);
	std::cin.get();
	return 0;
}