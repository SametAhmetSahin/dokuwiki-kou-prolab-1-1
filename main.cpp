#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;
using namespace std;

vector<string> files;
vector<string> directories;

// snake_case for variables, PascalCase for functions

int DirectoryScanner(string path) {
	for (const auto& entry : fs::directory_iterator(path))
	{
		
		//cout << entry.path() << endl;
		if (entry.path().string().find(".") != string::npos) {
			bool exists = 0;
			if (entry.path().string().find(".txt") != string::npos) {
				for (string& i : files) 
				{
					if (i == entry.path().string()) exists = 1;
				}
				if (!exists) files.push_back(entry.path().string());
			}
		}
		else { 
			bool exists = 0;
			for (string& i : directories) 
			{
				if (i == entry.path().string()) exists = 1;
			}
			if (!exists) directories.push_back(entry.path().string());
				
		}
	}
	return 0;
}

int ReadFile(string path) {

}

int main(int argc, char *argv[]) 
{
	DirectoryScanner("Üniversite");
	
	cout << "Directories" << endl << endl;
	for (string& i : directories) 
	{
		cout << i << endl;
		DirectoryScanner(i);
	}

	cout << endl << endl << "Files" << endl << endl;
	for (string& i : files) 
	{
		cout << i << endl;
	}
	

	return 0;
}


