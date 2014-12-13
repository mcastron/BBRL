
#include <fstream>
#include <iostream>
#include <string>

#include "Utils/UtilsLib.h"

using namespace std;
using namespace utils;
using namespace utils::compression;


// ===========================================================================
/**	
	\brief	Simple program to compress/uncompress zlib files.
	
	\author	Castronovo Michael

	\date	2014-12-13
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//	Extra functions header (see below for documentation)
// ---------------------------------------------------------------------------
string parsingErr(string);
void help();


// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	cout << "\n";
	
	//	Retrieve the request task (compress or uncompress)
	if (argc < 2)
	{
		string msg;
		msg += "No argument provided!";
		cout << parsingErr(msg) << "\n";
		
		return -1;
	}
	
	
	string mode(argv[1]);
	if (mode == "--help") { help(); }

	else if ((mode == "--compress") || (mode == "-z"))
	{		
		//	'compressionLevel'
		if ((argc < 2)
				|| (		(string(argv[2]) != "--compression_level")
					&&	(string(argv[2]) != "-cl")))
		{
			string msg;
			msg += "'--compression_level'/'-cl' token is missing!";
			cout << parsingErr(msg) << "\n";
			
			return -1;
		}
		string compressionLevelStr(argv[3]);
		
		int compressionLevel;
		if (compressionLevelStr == "Z_NO_COMPRESSION")
			compressionLevel = Z_NO_COMPRESSION;
		
		else if (compressionLevelStr == "Z_BEST_SPEED")
			compressionLevel = Z_BEST_SPEED;
		
		else if (compressionLevelStr == "Z_BEST_COMPRESSION")
			compressionLevel = Z_BEST_COMPRESSION;
		
		else if (compressionLevelStr == "Z_DEFAULT_COMPRESSION")
			compressionLevel = Z_DEFAULT_COMPRESSION;
		
		else
		{
			string msg;
			msg += "Unable to determine the compression level to use!";
			cout << parsingErr(msg) << "\n";
			
			return -1;
		}
		
		
		//	'input'		
		if (argc < 4)
		{
			string msg;
			msg += "Input file missing!";
			cout << parsingErr(msg) << "\n";
			
			return -1;
		}
		string input(argv[4]);
		
		
		//	Compress
		string output = (input + ".zz");
		
		Chrono openTime;
		cout << "\tOpen the file to compress..." << flush;
		ifstream is(input.c_str());
		istreambuf_iterator<char> eos;
		string dStr(istreambuf_iterator<char>(is), eos);
		is.close();
		cout << "done! (in " << openTime.get() << "ms)\n";
		
		Chrono compressTime;
		cout << "\tCompress the data and save them into a file..." << flush;
		ofstream os(output.c_str());
		os << compressStr(dStr, compressionLevel);
		os.close();
		cout << "done! (in " << compressTime.get() << "ms)\n\n";
			
	}
	
	else if ((mode == "--uncompress") || (mode == "-d"))
	{
		//	'input'		
		if (argc < 2)
		{
			string msg;
			msg += "Input file missing!";
			cout << parsingErr(msg) << "\n";
			
			return -1;
		}
		string input(argv[2]);
		
		
		//	Uncompress
		string ext = input.substr(input.find_last_of("."));
		if (ext != ".zz")
		{
			string msg;
			msg += "Bad format! (expected *.zz)\n";
			cout << parsingErr(msg) << "\n";
			
			return -1;
		}	
		string output = input.substr(0, input.find_last_of("."));
		
		Chrono openTime;
		cout << "\tOpen the file to uncompress..." << flush;
		ifstream is(input.c_str());
		istreambuf_iterator<char> eos;
		string cStr(istreambuf_iterator<char>(is), eos);
		is.close();
		cout << "done! (in " << openTime.get() << "ms)\n";
		
		Chrono uncompressTime;
		cout << "\tUncompress the data and save them intot a file...";
		cout << flush;
		ofstream os(output.c_str());
		os << uncompressStr(cStr);
		os.close();
		cout << "done! (in " << uncompressTime.get() << "ms)\n\n";
			
	}
	
	else
	{
		string msg;
		msg += "Unable to determine the task to perform!";
		cout << parsingErr(msg) << "\n";

		return -1;
	}
}

// ---------------------------------------------------------------------------
//	Extra functions implementation
// ---------------------------------------------------------------------------
string parsingErr(string msg)
{
	return ("[parsing error]:\t" + msg);
}


void help()
{
	ifstream is("doc/command-line manual (TinyBRL-zlib).txt");
	string line;
	cout << "\n";
	while (!is.eof()) { getline(is, line); cout << line << "\n"; }
}
