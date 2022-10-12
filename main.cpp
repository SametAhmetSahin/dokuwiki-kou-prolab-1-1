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

string U16ToString(u16string u16input) {
	wstring_convert<codecvt_utf8_utf16<char16_t, 0x10ffff, codecvt_mode::little_endian>, char16_t> u8convertor;
    string output = u8convertor.to_bytes(u16input);
    if(u8convertor.converted() < output.size())
        throw runtime_error("incomplete conversion");
    return output;
}

u16string StringToU16(string input) {
	wstring_convert<codecvt_utf8_utf16<char16_t, 0x10ffff, codecvt_mode::little_endian>, char16_t> u16convertor;
    u16string u16output = u16convertor.from_bytes(input);
	if(u16convertor.converted() < u16output.size())
        throw runtime_error("incomplete conversion");
	return u16output;
}

string ConvertTagnameToFile(string tagname) {
	map<u16string, u16string> table;
	
	table[u"\u00f6"] = u"o";
	table[u"\u00fc"] = u"u";
	table[u"\u011f"] = u"g";
	table[u"\u015f"] = u"s";
	table[u"\u00e7"] = u"c";
	table[u"\u0131"] = u"i";
	
	table[u"\u00d6"] = u"O";
	table[u"\u00dc"] = u"U";
	table[u"\u011e"] = u"G";
	table[u"\u015e"] = u"S";
	table[u"\u00c7"] = u"C";
	table[u"\u0130"] = u"I";
	
	table[u"\u0020"] = u"_";

	//wstring_convert<codecvt<char16_t, char, mbstate_t>,char16_t> u16;

	//u16string u16tagname = u16.from_bytes(tagname);

	
	u16string u16tagname = StringToU16(tagname);
	
	u16string tagname_replaced = u"";

	for (char16_t& character: u16tagname) {
		u16string tempstring = u"";
		tempstring += character;
		if (!(table.count(tempstring)))	tagname_replaced += tempstring;
		else tagname_replaced += table[tempstring];
	}

	
	return U16ToString(tagname_replaced);
}

vector<string> FindOrphanTags () {

	vector<string> orphantags;

	vector <string> convertednames;

	/* 
	for (string& filename : files) {
		
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

		
		
		/*
		for (string& basename: basenames) {

			string tagname_converted = ConvertTagnameToFile(basename);

			cout << "Converted tagname: " << tagname_converted << endl;
			convertednames.push_back(tagname_converted);
			
		}
		
	}

	*/
	/*
	for (string& tagname : tags) {
		bool orphan = 1;
		
		for (string& convertedname: convertednames) {
			if (convertedname == tagname) {
				orphan = 0;		
			}
		}
		if (orphan) {
			//cout << tagname << " is an orphan tag! at file " << filename << endl;
			PushBackUnique(orphantags, tagname);
		}
		
	}
	*/

		
	

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

				//cout << "Bad tag!" << endl;
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

		//cout << "Starting new line lookup" << endl;
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

vector<string> SearchForWord(string word, int &totalfindcount) {
	
	bool istag = false;
	totalfindcount = 0;
	for (string& tag : tags) {
		if (word == tag) {
			istag = true;
			//cout << word << " is tag!" << endl;
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
			//cout << "Found word " << word << " at line " << line << " at file " << file << endl;
		}

		totalfindcount += foundpositions.size();
	}


	
	
}

int WriteToFile(string filename, string content) {
	ofstream file;
	file.open(filename);

	file << content << endl;
	file.close();
}

string ConvertCharToHex(char character) {
	std::stringstream ss;
	cout << ((int) character) + 320 << endl;
	ss << std::hex << ((int) character) + 320;
	
	string mystr = ss.str();
	while (mystr.length() < 4) {
		mystr = "0" + mystr;
	}
	mystr = mystr;
	return mystr;

}

int main(int argc, char *argv[]) 
{

	setlocale(LC_ALL, "tr-TR.ISO-8859-1");

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
	
	//cout << "Kelime - Tekrar Sayısı" << endl;
	
	string outputcontent = "Etiket Adı - Tekrar Sayısı\n\n";
	
	for (string& tag : tags) {
		int totalfindcount = 0;
		SearchForWord(tag, totalfindcount);
		//cout << tag << " " << totalfindcount << endl;
		
		

		outputcontent += tag + " " + to_string(totalfindcount) + "\n";

	}
	
	
	

	outputcontent += "\n\n------------------------\n\nYetim Etiketler\n\n";
	
	cout << "Yetim etiketler bulunmaya başlanıyor" << endl;
	vector<string> orphantags = FindOrphanTags(); 
	cout << "Yetim etiket bulma fonksiyonu bitti" << endl;
	
	/* 
	for (string& orphantag : orphantags) {
		outputcontent += orphantag;
	}
	*/
	
	
	//SearchForWord("Programlama I");

	/* 
	vector<string> orphantags = FindOrphanTags();

	for (string& i : orphantags) {
		cout << "OrphanTag: " << i << endl;
	}
	*/

	WriteToFile("output.txt", outputcontent);

	cout << ConvertTagnameToFile("Proğramlama İ") << endl;
	return 0;
}