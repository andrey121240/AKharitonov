#include <iostream>
#include <fstream>  
#include <string>
#include <boost/filesystem.hpp>
#include "keccak.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp> 


namespace fs = boost::filesystem;
std::string test;


struct Fileinfo {
	std::string path;
	std::string hash;
	int size;
	std::string flag = "NEW";
};


std::vector<Fileinfo> compare_lists(std::vector<Fileinfo> newfl, std::vector<Fileinfo> oldfl) {
	for (std::vector<Fileinfo>::iterator itnew = newfl.begin(); itnew < newfl.end(); itnew++) {
		/*Бегаем по вектору:
		//Если нашли путь и хеш совпал, то меняем флаг на UNCHANGED
		//Если нашли путь и хеш не совпал, то меняем флаг на CHANGED
		//В новом файле у всех флагов у всех структур стоит флаг NEW - если не нашли в старом, то его и оставляем
		//Всё что осталось в старом - DELETED, добавляем в newfl и возвращаем его же
		*/
		for (std::vector<Fileinfo>::iterator itold = oldfl.begin(); itold < oldfl.end(); itold++) {
			if ((itnew->path == itold->path) && (itnew->hash == itold->hash)) {
				itnew->flag = "UNCHANGED";
				oldfl.erase(itold);
				break;
			}
			if ((itnew->path == itold->path) && (itnew->hash != itold->hash)) {
				itnew->flag = "CHANGED";
				oldfl.erase(itold);
				break;
			}
		}
	}
	for (std::vector<Fileinfo>::iterator itold = oldfl.begin(); itold < oldfl.end(); itold++) {
		itold->flag = "DELETED";
		newfl.push_back(*itold);
	}
	return newfl;
}

void save2bsr(std::string filename, std::vector<Fileinfo> & vec_finfo) {
	std::ofstream ofs(filename); 
	boost::archive::text_oarchive oa(ofs);
	for (Fileinfo it : vec_finfo) { 
		const std::string path = it.path;
		oa << path;
		const std::string size = std::to_string(it.size);
		oa << size;
		const std::string hash = it.hash;
		oa << hash;
	}

	ofs.close();
}

void read4bsr(std::string filename, std::vector<Fileinfo> & vec_finfo) {
	Fileinfo it;
	std::ifstream ofs(filename);
	boost::archive::text_iarchive oa(ofs);
	std::string test;
	while (true) {
		try
		{
			oa >> test;
			it.path = test;
			oa >> test;
			std::string ass(test.begin(), test.end() - 1);
			std::cout << test;
			if (ass.size() >= 1) it.size = std::stoi( ass );
			oa >> test;
			it.hash = test;
			vec_finfo.push_back(it);
			
		}
		catch (boost::archive::archive_exception& ex)
		{
			break;
		}
	}
	ofs.close();
}




void get_dir_list(fs::directory_iterator iterator, std::vector<Fileinfo> & vec_finfo, Fileinfo & finfo, std::ifstream & ifs) {
	for (; iterator != fs::directory_iterator(); ++iterator)
	{
		if (fs::is_directory(iterator->status())) {
			fs::directory_iterator sub_dir(iterator->path());
			get_dir_list(sub_dir, vec_finfo, finfo, ifs);

		}
		else
		{

			finfo.path = iterator->path().string();
			std::replace(finfo.path.begin(), finfo.path.end(), '\\', '/');
			finfo.size = fs::file_size(iterator->path());
			ifs.open(finfo.path, std::ios_base::binary);
			std::string strifs((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));
			Keccak keccak;
			finfo.hash = keccak(strifs);
			ifs.close();
			vec_finfo.push_back(finfo);
		}

	}
}

void print_finfo_vec(std::vector<Fileinfo> vec) {
	for (Fileinfo element : vec) {
		std::cout << element.path << std::endl <<
			element.size << std::endl <<
			element.hash << std::endl <<
			element.flag << std::endl << "-------" << std::endl;
	}
}

int main() {
	std::ofstream myfile;
	std::string path, dirpath;
	Fileinfo finfo;
	std::ifstream ifs;
	std::string checkstatus;
	std::cout << "Do you wish to save filelist or check current folder with previous result?" <<
		std::endl << "(check/save or anything else for neither)" << std::endl;
	std::getline(std::cin, checkstatus);

	std::cout << "Folder path:" << std::endl;
	std::getline(std::cin, path);
	std::vector<Fileinfo> vec_finfo;
	std::vector<Fileinfo> vec_finfo_old;
	try {
		fs::directory_iterator home_dir(path);
		get_dir_list(home_dir, vec_finfo, finfo, ifs);
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cout << "INVALID PATH" << std::endl;
		checkstatus = "null";
	}
	if (checkstatus == "save") {
		save2bsr("filelist.bsr", vec_finfo);
		print_finfo_vec(vec_finfo);
	}
	if (checkstatus == "check") {
		read4bsr("filelist.bsr", vec_finfo_old);
		print_finfo_vec(compare_lists(vec_finfo, vec_finfo_old));
	}
	if ((checkstatus != "save") && (checkstatus != "check")) {
		print_finfo_vec(vec_finfo);
	}
	std::cin.get();
	return 0;
}