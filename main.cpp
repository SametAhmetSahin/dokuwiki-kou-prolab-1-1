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

	for (string& i : vector) { // vektördeki her eleman için
		if (i == element) exists = 1; // eğer element eleman'a eşitse vardır
	}

	if (!exists) vector.push_back(element); // eğer yoksa vektöre pushla

	return 0;
}

int DirectoryScanner(string path) { // Parametre olarak yol alır, yoldaki klasörleri ve dosyaları sırasıyla directories ve files vektörlerine pushlar 

	for (const auto& entry : fs::directory_iterator(path))
	{
		
		//cout << entry.path() << endl;
		if (entry.path().string().find(".") != string::npos) { // eğer girdide . varsa
			
			if (entry.path().string().find(".txt") != string::npos) { // eğer girdide .txt varsa
				PushBackUnique(files, entry.path().string()); // files'a o girdiyi pushla
			}
		}
		else { 
			PushBackUnique(directories, entry.path().string()); // yoksa directories'e o girdiyi pushla
		}
	}

	return 0;
}

string ReadFile(string path) { // Parametre olarak dosya yolu alır, çıktı olarak dosyanın içeriğini verir
	
	ifstream file;

	file.open(path);
	string content = "";

	string templine;

	if (file.is_open()) {
		while (getline(file, templine)) { // Dosya açık olduğu sürece ve dosyadan yeni satır alabiliyorken content'e yeni satır ve "\n" ekler
				content += templine + "\n";
		}
	}
	
	return content;
}

string U16ToString(u16string u16input) { // Parametre olarak utf16 string alır, çıktı olarak normal string verir
	wstring_convert<codecvt_utf8_utf16<char16_t, 0x10ffff, codecvt_mode::little_endian>, char16_t> u8convertor; // dönüştürücü tanımlar
    string output = u8convertor.to_bytes(u16input); // dönüştürücüyle girdiği dönüştürür
    if(u8convertor.converted() < output.size())
        throw runtime_error("incomplete conversion");
    return output;
}

u16string StringToU16(string input) { // Parametre olarak normal string alır, çıktı olarak utf string verir
	wstring_convert<codecvt_utf8_utf16<char16_t, 0x10ffff, codecvt_mode::little_endian>, char16_t> u16convertor; // dönüştürücü tanımlar
    u16string u16output = u16convertor.from_bytes(input); // dönüştürücüyle girdiği dönüştürür
	if(u16convertor.converted() < u16output.size())
        throw runtime_error("incomplete conversion");
	return u16output;
}

string ConvertTagnameToFilename(string tagname) {
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

	for (char16_t& character: u16tagname) { // kelimedeki her char için
		u16string tempstring = u""; // geçici bir string oluşturur
		tempstring += character;  // geçici stringe karakteri ekler
		if (!(table.count(tempstring)))	tagname_replaced += tempstring; // eğer geçici string tabloda yoksa (türkçe karakter değilse yani) tagname_replaced'e o stringi ekler
		else tagname_replaced += table[tempstring]; // eğer geçici string tabloda varsa (türkçe karakterse) tagname_replaced'e o stringin tablodaki halini ekler
	}

	
	return U16ToString(tagname_replaced);
}

vector<string> FindOrphanTags () { // Yetim etiketleri bulur, çıktı olarak yetim etiketlerin bulunduğu bir liste döndürür.

	vector<string> orphantags;

	vector<string> basenames;

	
	for (string& filename : files) { // her dosya için
		
		vector<string> splitted;

		char chararray[filename.length()+1];
		strcpy(chararray, filename.c_str()); // dosya ismi stringini char array'e dönüştürür
		
		char* token = strtok(chararray, "/"); // char array'i / karakterinden böler (Üniversite/Dersler/Programlama_I.txt) örneğinde dosyanın basename'ine erişmek için

		while (token != NULL) { // yeni / bulamayana kadar bölme yapmaya devam eder
			string stringtoken;
			stringtoken = token;
			splitted.push_back(stringtoken);
			token = strtok(NULL, "/");
		}
		
		vector<string> filetxts;

		int counter = 0; // sayaç tanımlar

		for (string& filetxt : splitted) { // bölünmüş arraydaki her isim için, bölünmüş array {Üniversite, Dersler, Programlama_I.txt, Üniversite, Dersler, Programlama_II.txt} gibidir
			counter++; // sayacı bir arttırır
			counter %= 3; // sayacın 3'e göre modunu alır
			if (counter == 0) filetxts.push_back(filetxt); // eğer sayaç 3 ise, yani 3 dosyada bir, bu string istediğimiz dosyadır bunu filetxts vektörüne pushla  
		}

		

		for (string& filetxt: filetxts) { // filetxts vektöründeki her biri için:
			char chararray[filetxt.length()+1];
			strcpy(chararray, filetxt.c_str()); // stringi chararray'e dönüştür
		
			char* token = strtok(chararray, "."); // .'dan böl, ilkini al (basename yani, örnek Programlama_I.txt şeklinde)
			string tokenstring = token;
			basenames.push_back(tokenstring); // basenames vektörüne basename'i pushla
		}

		
		
		
		
		
	}

	
	for (string& tagname : tags) { // her tag için
		bool orphan = 1; // yetim olduğunu varsay
		
		for (string& basename : basenames) { // her basename için
			//cout << "basename: " << basename << " converted_name: " << ConvertTagnameToFilename(tagname) << endl;
			if (basename == ConvertTagnameToFilename(tagname)) { // eğer basename tagname'in dosya ismine dönüştürülmüş haline eşitse
				orphan = 0;	// yetim değildir
			}
		}
		if (orphan) { // eğer yetimse
			//cout << tagname << " is an orphan tag! at file " << filename << endl;
			PushBackUnique(orphantags, tagname); // orphantags vektörüne pushla
		}
		
	}
	

		
	

	return orphantags;
}

vector<string> FindWantedTags () { // İstenen etiketleri bulur, çıktı olarak istenen etiketlerin bulunduğu bir liste döndürür.

	vector<string> wantedtags;

	vector<string> basenames;

	
	for (string& filename : files) { // her dosya için
		
		vector<string> splitted;

		char chararray[filename.length()+1];
		strcpy(chararray, filename.c_str()); // dosya ismi stringini char array'e dönüştürür
		
		char* token = strtok(chararray, "/"); // char array'i / karakterinden böler (Üniversite/Dersler/Programlama_I.txt) örneğinde dosyanın basename'ine erişmek için

		while (token != NULL) { // yeni / bulamayana kadar bölme yapmaya devam eder
			string stringtoken;
			stringtoken = token;
			splitted.push_back(stringtoken);
			token = strtok(NULL, "/");
		}
		
		vector<string> filetxts;

		int counter = 0; // sayaç tanımlar

		for (string& filetxt : splitted) { // bölünmüş arraydaki her isim için, bölünmüş array {Üniversite, Dersler, Programlama_I.txt, Üniversite, Dersler, Programlama_II.txt} gibidir
			counter++; // sayacı bir arttırır
			counter %= 3; // sayacın 3'e göre modunu alır
			if (counter == 0) filetxts.push_back(filetxt); // eğer sayaç 3 ise, yani 3 dosyada bir, bu string istediğimiz dosyadır bunu filetxts vektörüne pushla  
		}

		for (string& filetxt: filetxts) { // filetxts vektöründeki her biri için:
			char chararray[filetxt.length()+1];
			strcpy(chararray, filetxt.c_str()); // stringi chararray'e dönüştür
		
			char* token = strtok(chararray, "."); // .'dan böl, ilkini al (basename yani, örnek Programlama_I.txt şeklinde)
			string tokenstring = token;
			basenames.push_back(tokenstring); // basenames vektörüne basename'i pushla
		}
		
	}

	
	for (string& basename : basenames) { // her tag için
		bool wanted = 1; // yetim olduğunu varsay
		
		for (string& tagname : tags) { // her basename için
			//cout << "basename: " << basename << " converted_name: " << ConvertTagnameToFilename(tagname) << endl;
			if (basename == ConvertTagnameToFilename(tagname)) { // eğer basename tagname'in dosya ismine dönüştürülmüş haline eşitse
				wanted = 0;	// yetim değildir
			}
		}
		if (wanted) { // eğer yetimse
			//cout << tagname << " is an orphan tag! at file " << filename << endl;
			PushBackUnique(wantedtags, basename); // orphantags vektörüne pushla
		}
		
	}

	return wantedtags;
}
// tagfinder regex \[{2}[\w| ]*\]{2}

vector <string> FindTags(string content, bool global_vector = true) { // Bir metindeki etiketleri bulur, opsiyonel global vektör flagi alır, global vektör flagi doğruysa bulduğu etiketleri tags vektörüne pushlar, değilse local bir vektöre pushlar ve bunu döndürür

	// string.find() ile [[ aranır, ardından indisi ondan sonraki ]] aranır, [['ın bulunduğu karakterden itibaren okunmaya başlanır, 
	// arada [ veya ] varsa arama iptal edilir, bir sonraki [[ aranır

	vector<string> local_tags_vector;

	if (content == "") return local_tags_vector;

	vector<int> startindexes;
	vector<int> endindexes;

	int tagstartindex = 0;
	int tagendindex = tagstartindex + 2;
	
	while (tagstartindex < content.length()) { // baslangıç indeksi metin uzunluğundan kısa olduğu sürece
		tagstartindex = content.find("[[", tagstartindex+2); // başlangıç indeksi bir sonraki [[
		tagendindex = content.find("]]", tagstartindex+2); // bitiş indeksi bir sonraki ]]
		startindexes.push_back(tagstartindex); // başlangıç indekslerine başlangıç indeksini ekle
		endindexes.push_back(tagendindex); // bitiş indekslerine bitiş indeksini ekle
	}

	for(int i=0; i != startindexes.size()-1; ++i) { // başlangıç indekslerinin boyutunun bir eksiğine ulaşana kadar (neden şu an hatırlamıyorum ama son satır okumayla alakası vardı galiba)

		bool badtag = false; // düzgün etiket olduğunu varsay
		string tagstring = ""; // boş etiket tanımla

		tagstring = content.substr(startindexes[i]+2, endindexes[i]-startindexes[i]-2); // 2 sayıları [[ ve ]] karakterlerinin bulunmaması içindir. Bulunması gerekirse sayıların kaldırılması yeterlidir.
		// metinde başlangıç indeksinden başlayıp bitiş indeksi ile başlangıç indeksinin farkı kadar olan her yeri etiket olarak tanımla
		for (int j=0;j<tagstring.length();j++) { // etiket ismini dolaş

			if ((tagstring[j] == ']') or (tagstring[j] == '[')) { // içinde [ veya ] varsa etiket bozuktur

				//cout << "Bad tag!" << endl;
				badtag = true;
			}
		}

		if (!(badtag)) { // bozuk değilse

			//cout << "TagString:" << tagstring << endl;
			if (global_vector) PushBackUnique(tags, tagstring); // uygun yere pushla
			else PushBackUnique(local_tags_vector, tagstring);
		}
	}

	return local_tags_vector;
}

vector<int> LineLengthsOfFile(string filepath) { // Parametre olarak dosya yolu alır, çıktı olarak dosyalardaki satırların uzunluklarını bir vektör halinde verir.
	vector <int> linelengths;
	string line;
	ifstream file(filepath);

    if(file.is_open()) { // dosya açıkken

        while(!file.eof()) { // dosyanın sonuna ulaşana kadar

            getline(file,line); // satır oku
			//cout << line.length() << endl;
            linelengths.push_back(line.length()); // satır uzunluğunu linelengths'e pushla
        }

        file.close();
    }

	return linelengths;
}

vector<int> FindLinesOfCharPositions(vector<int> linelengths, vector<int> charpositions) { // Parametre olarak dosya uzunluklarının olduğu ve char konumlarının olduğu iki vektör alır, çıktı olarak konumların hangi satırlarda olduğunu belirten bir vektör verir
	vector <int> lines;
	//cout << "Char positions size" << charpositions.size() << endl;

	for (int& pos : charpositions) { // her pozisyon için

		//cout << "Starting new line lookup" << endl;
		int val = pos; // değer = pozisyon
		int linecount = 1; // satır sayısı 1 (0. satır olmaz 1. satır olur)

		for (int& length : linelengths) { // her uzunluk için

			//cout << "val is: " << val << ", deducting " << length << " from it" << endl;
			val -= length; // değerden uzunluğu çıkar

			if (val < 0) { // 0'dan küçükse o char o satırdadır bunu pushla

				lines.push_back(linecount);
				break;
			}

			else { // değilse satırı 1 arttır
				linecount++;
			}
		}
	}
	return lines;
}

vector<int> SearchForWordInFile(string word, string file) { // Parametre olarak kelime ve dosya alır, dosyada kelimeleri bulup konumları bir vektör olarak döndürür

	bool istag = false;
	
	for (string& tag : tags) {
		if (word == tag) {
			istag = true;
		}
	}
	
	
	vector<int> foundpositions;

	int pos = 0;
	//cout << "Reading " << file << endl;
	string content = ReadFile(file);
	
	while (pos < content.length() && (content.find(word) != string::npos)) { // konum metin uzunluğundan kısayken ve o kelime metinde hala varken
		pos = content.find(word, pos+1); // konum o kelimenin metinde konum+1. konumdan itibaren aranıp bulunduğu konumdur
		if (pos != -1) foundpositions.push_back(pos); // eğer konum -1 değilse (sonda) foundpositions vektörüne pushla
	}

	return foundpositions;
	
	
}

int WriteToFile(string filename, string content) { // Parametre olarak dosya ismi ve içeriği alır, içeriği dosyaya yazar.
	ofstream file;
	file.open(filename);

	file << content;
	file.close();

	return 0;
}

int UpdateIndexing() {
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

	return 0;
}

string FindBasePathOfFile(string filename) { // Parametre olarak dosyanın tam yolunu alır

	vector<string> splitted;
	
	char chararray[filename.length()+1];
	strcpy(chararray, filename.c_str()); // dosya ismi stringini char array'e dönüştürür
	
	char* token = strtok(chararray, "/"); // char array'i / karakterinden böler (Üniversite/Dersler/Programlama_I.txt) örneğinde dosyanın basename hariç geri kalan kısmına erişmek için

	while (token != NULL) { // yeni / bulamayana kadar bölme yapmaya devam eder
		string stringtoken;
		stringtoken = token;
		splitted.push_back(stringtoken);
		token = strtok(NULL, "/");
	}

	//cout << "Finished splitting" << endl;
	string basepath = "";

	for (int i=0; i != splitted.size() - 1; i++) { // dosya ismine kadar her şeyi basepath'e ekler
		basepath += splitted.at(i) + "/";
	}
	

	return basepath;
}

int UpdateTagInFile(string oldTag, string newTag, string file)
{
	string fileContent = ReadFile(file);

	/*while(position < fileContent.length() && fileContent.find(oldTag) != string::npos)
	{
		position = fileContent.find(oldTag, position + 1);
		if(position != -1) fileContent.replace(position, oldTag.length(), newTag);
	}*/

	//string newOldTag = 

	vector<int> positions = SearchForWordInFile("[[" + oldTag + "]]", file);

	for(int& pos : positions)
	{
		fileContent.replace(pos + 2, oldTag.length(), newTag);
	}
	
	if(positions.size() > 0) 
	{
		WriteToFile(file, fileContent);
		return 0;
	}

	return 1;
}

string CreateOrphanFile(string tag, int code)
{
	// ConvertTagnameToFilename

	/*
	Dersin Kodu   : BLM102

	Dersin Adı    : Programlama II

	Öğretim Elemanları :

	Dersin İçeriği:

	*/
	string content = "Dersin Kodu\t\t\t: BLM" + to_string(code) + "\n\nDersin Adı\t\t\t: [[" + tag + "]]\n\nÖğretim Elemanları\t: \n\nDersin İçeriği\t\t:";
	// Üniversite/YetimEtiketler

	string filePath = "Üniversite/YetimEtiketler/" + ConvertTagnameToFilename(tag) + ".txt";
	WriteToFile(filePath, content);

	return filePath;
}

int UpdateTags(string oldTag, string newTag)
{
	for(string& file : files)
	{
		if(UpdateTagInFile(oldTag, newTag, file) == 0)
			if (file == (FindBasePathOfFile(file) + ConvertTagnameToFilename(oldTag) + ".txt")) {
				//rename(file, ConvertTagnameToFilename(oldTag));
				string newFileName = FindBasePathOfFile(file) + ConvertTagnameToFilename(newTag) + ".txt";
				rename(file.c_str(), newFileName.c_str());
				//break;
			}
	}
	
	int code = 201;
	vector<string> orphans = FindOrphanTags();
	for(string& orphan : orphans)
	{
		CreateOrphanFile(orphan, code);
		code++;
	}

	return 0;
}

int DumpToFile() {

	string outputcontent = "Etiket Adı - Tekrar Sayısı\n\n";
	
	
	for (string& tag : tags) {

		int totalfindcount = 0;

		for (string& file : files) {
		vector<int> foundpositions = SearchForWordInFile(tag, file);
		//cout << tag << " " << totalfindcount << endl;
		totalfindcount += foundpositions.size();
		
		}
		
		outputcontent += tag + " " + to_string(totalfindcount) + "\n";
		


	}

	outputcontent += "\n\n------------------------\n\nYetim Etiketler\n\n";
	
	//cout << "Yetim etiketler bulunmaya başlanıyor" << endl;
	vector<string> orphantags = FindOrphanTags(); 
	
	for (string& orphantag : orphantags) {
		outputcontent += orphantag + "\n";
	}
	
	for (string& file : files) 
	{
		vector<int> foundpositions = SearchForWordInFile("Programlama I", file);
		vector<int> lines = FindLinesOfCharPositions(LineLengthsOfFile(file), foundpositions);
		if (foundpositions.size() != 0) {
		string coutstring = "Found Programlama I at lines ";
		for (int& line : lines) coutstring += to_string(line) + " ";

		//cout << coutstring << endl;
		}
	}

	

	outputcontent += "\n\n------------------------\n\nİstenen Etiketler\n\n";

	vector<string> wantedtags = FindWantedTags();

	for (string& wantedtag : wantedtags) {
		outputcontent += wantedtag + "\n";
	}
	WriteToFile("output.txt", outputcontent);
	return 0;
}
	
int DumpToFilePartTwo() {
	string outputcontent = ReadFile("output.txt");
	outputcontent += "\n\n------------------------\n\nYetim Etiketler Oluşturulduktan Sonraki Yetim Etiketler\n\n";
	vector<string> neworphantags = FindOrphanTags(); 
	
	for (string& neworphantag : neworphantags) {
		outputcontent += neworphantag + "\n";
	}
	WriteToFile("output.txt", outputcontent);
	return 0;
}


int main(int argc, char *argv[]) 
{

	setlocale(LC_ALL, "tr-TR.ISO-8859-1");
	
	UpdateIndexing();

	DumpToFile();

	UpdateTags("Matematik_I", "Matematik 10");

	UpdateIndexing();
	
	DumpToFilePartTwo();

	return 0;
}