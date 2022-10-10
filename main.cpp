#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <cstring>
#include <map>

namespace fs = std::filesystem;
using namespace std;

vector<string> files;
vector<string> directories;
vector<string> tags;

// snake_case for variables, PascalCase for functions

int PushBackUnique(vector<string>& vector, string element) { // İlk argüman olarak vektörü, ikinci argüman olarak eklenecek elemanı alır. Vektör string tipindedir.
	bool exists = 0;

	for (string& i : vector) {
		if (i == element) exists = 1;
	}

	if (!exists) vector.push_back(element);

	return 0;
}

int DirectoryScanner(string path) {

	for (const auto& entry : fs::directory_iterator(path))
	{
		
		//cout << entry.path() << endl;
		if (entry.path().string().find(".") != string::npos) {
			
			if (entry.path().string().find(".txt") != string::npos) {
				PushBackUnique(files, entry.path().string());
			}
		}
		else { 
			PushBackUnique(directories, entry.path().string());
		}
	}

	return 0;
}

string ReadFile(string path) {
	
	ifstream file;

	file.open(path);
	string content = "";

	string templine;

	if (file.is_open()) {
		while (getline(file, templine)) {
				content += templine + "\n";
		}
	}
	
	return content;
}

string ConvertTagnameToFile(string tagname) {
	map<char, char> table;
	table['ö'] = 'o';
	table['ü'] = 'u';
	table['ğ'] = 'g';
	table['ş'] = 's';
	table['ç'] = 'c';
	table['ı'] = 'i';

	table['Ö'] = 'O';
	table['Ü'] = 'U';
	table['Ğ'] = 'G';
	table['Ş'] = 'S';
	table['Ç'] = 'C';
	table['İ'] = 'I';
	
	table[' '] = '_';

	map<char, char> :: iterator iter;

	string tagname_converted = "";
	
	for (char& character: tagname) {
		if (!(table.count(character))) tagname_converted += character;
		else tagname_converted += table[character];
	}

	/*
	
	for (char & character : tagname) {
		for (auto element:table) {
		if (element.first != character) {
			tagname_converted += character;
			break;
		}
		else {
			tagname_converted += element.second;
			
		}
		}
	}
	*/
	
	/*
	for (auto element:table) {

		replace(tagname_converted.begin(), tagname_converted.end(), element.first, element.second);

	}
	*/
	
	
	return tagname_converted;
}

vector<string> FindOrphanTags () {

	vector<string> orphantags;

	for (string& filename : files) {

		for (string& tagname : tags) {

			vector<string> splitted;

			char chararray[filename.length()+1];
			strcpy(chararray, filename.c_str());
			
			char* token = strtok(chararray, "/");

			while (token != NULL) {
				string stringtoken;
				stringtoken = token;
				splitted.push_back(stringtoken);
				token = strtok(NULL, "/");
			}
			
			vector<string> filetxts;

			int counter = 0;

			for (string& filetxt : splitted) {
				counter++;
				counter %= 3;
				if (counter == 0) filetxts.push_back(filetxt);
			}

			vector<string> basenames;

			for (string& filetxt: filetxts) {
				char chararray[filetxt.length()+1];
				strcpy(chararray, filetxt.c_str());
			
				char* token = strtok(chararray, ".");
				string tokenstring = token;
				basenames.push_back(tokenstring);
			}

			vector <string> convertednames;

			for (string& basename: basenames) {
				//cout << basename << endl;
				string tagname_converted = ConvertTagnameToFile(basename);
				cout << "Basename len:" << basename.length() << endl;
				cout << "Converted tagname:" << tagname_converted << endl;
				convertednames.push_back(tagname_converted);
				cout << "Converted tagname len:" << tagname_converted.length() << endl;
				
			}

			for (string& convertedname: convertednames) {

				for (string& convertedname: convertednames) {

				if (convertedname != tagname) {
					cout << tagname << "is an orphan tag!" << endl;
				}

				}
			}
			
			
		}
	}

	return orphantags;
}
// tagfinder regex \[{2}[\w| ]*\]{2}

vector <string> FindTags(string content, bool global_vector = true) {

	// string.find() ile [[ aranır, ardından indisi ondan sonraki ]] aranır, [['ın bulunduğu karakterden itibaren okunmaya başlanır, 
	// arada [ veya ] varsa arama iptal edilir, bir sonraki [[ aranır

	vector<string> local_tags_vector;

	if (content == "") return local_tags_vector;

	vector<int> startindexes;
	vector<int> endindexes;

	int tagstartindex = 0;
	int tagendindex = tagstartindex + 2;
	
	while (tagstartindex < content.length()) {
	tagstartindex = content.find("[[", tagstartindex+2);
	tagendindex = content.find("]]", tagstartindex+2);
	startindexes.push_back(tagstartindex);
	endindexes.push_back(tagendindex);
	}

	for(int i=0; i != startindexes.size()-1; ++i) {

		bool badtag = false;
		string tagstring = "";

		tagstring = content.substr(startindexes[i]+2, endindexes[i]-startindexes[i]-2); // 2 sayıları [[ ve ]] karakterlerinin bulunmaması içindir. Bulunması gerekirse sayıların kaldırılması yeterlidir.
		
		for (int j=0;j<tagstring.length();j++) {

			if ((tagstring[j] == ']') or (tagstring[j] == '[')) {

				cout << "Bad tag!" << endl;
				badtag = true;
			}
		}

		if (!(badtag)) {

		cout << "TagString:" << tagstring << endl;
		if (global_vector) PushBackUnique(tags, tagstring);
		else PushBackUnique(local_tags_vector, tagstring);
		}
	}

	return local_tags_vector;
}

vector<int> LineLengthsOfFile(string filepath) {
	vector <int> linelengths;
	string line;
	ifstream file(filepath);

    if(file.is_open()) {

        while(!file.eof()) {

            getline(file,line);
			//cout << line.length() << endl;
            linelengths.push_back(line.length());
        }

        file.close();
    }

	return linelengths;
}

vector<int> FindLinesOfCharPositions(vector<int> linelengths, vector<int> charpositions) {
	vector <int> lines;
	//cout << "Char positions size" << charpositions.size() << endl;

	for (int& pos : charpositions) {

		cout << "Starting new line lookup" << endl;
		int val = pos; 
		int linecount = 1;

		for (int& length : linelengths) {

			//cout << "val is: " << val << ", deducting " << length << " from it" << endl;
			val -= length;

			if (val < 0) {

				lines.push_back(linecount);
				break;
			}

			else {
				linecount++;
			}
		}
	}
	return lines;
}

int FileLineCounter(string filepath) {
	int count = 0;
	string line;
	ifstream file(filepath);

    if(file.is_open()) {

        while(!file.eof()) {

            getline(file,line);
            count++;
        }
        file.close();
    }
	return count - 1;
}

vector<string> SearchForWord(string word) {
	bool istag = false;
	
	for (string& tag : tags) {
		if (word == tag) {
			istag = true;
			cout << word << " is tag!" << endl;
		}
	}
	
	for (string& file : files) {
		vector<int> foundpositions;
		int pos = 0;
		//cout << "Reading " << file << endl;
		string content = ReadFile(file);

		
		while (pos < content.length() && (content.find(word) != string::npos)) {
			pos = content.find(word, pos+1);
			if (pos != -1) foundpositions.push_back(pos);
		}

		// The comment lines below are for debugging, can be ignored or deleted in the future
		
		//if (*(foundpositions.end()) == -1) foundpositions.pop_back(); // -1'i yok eder.

		//cout << "There are " << FileLineCounter(file) << " lines in file " << file << endl;
		//cout << " found positions size is " << foundpositions.size() << " for word " << word << " for file " << file << endl;
		/*
		for (int& pos : foundpositions) {
			cout << "Foundpos " << pos << endl; 
		} */
		
		vector<int> lines = FindLinesOfCharPositions(LineLengthsOfFile(file), foundpositions);

		//cout << "lines size: " << lines.size() << endl;
		
		for (int& line : lines) {
			cout << "Found word " << word << " at line " << line << " at file " << file << endl;
		}
	}
	
}

int main(int argc, char *argv[]) 
{
	DirectoryScanner("Üniversite");
	
	//cout << "Directories" << endl << endl;
	for (string& i : directories) 
	{
		//cout << i << endl;
		DirectoryScanner(i);
	}

	//cout << endl << endl << "Files" << endl << endl;
	for (string& i : files) 
	{
		//cout << i << endl;
		FindTags(ReadFile(i));
	}

	//FindTags(ReadFile("Üniversite/Dersler/Programlama_I.txt"));
	/*
	cout << endl << endl << "Tags" << endl;
	for (string& tag : tags) {
		cout << tag << endl;
	}
	*/
	
	

	//cout << ReadFile("Üniversite/Dersler/Programlama_I.txt");
	
	/*
	tags.push_back("[[Programlama I]]");

	vector<string> orphantags = FindOrphanTags();
	*/
	
	SearchForWord("Programlama I");

	return 0;
}