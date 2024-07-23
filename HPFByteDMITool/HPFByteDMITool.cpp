// HPFByteDMITool.cpp 
// Main code for the tool itself.
#include <iostream>
#include <windows.h>
#include <comdef.h>
#include <vector>
#include <sstream>

typedef std::pair<std::string, std::string> NameValuePair;

void Ini2Pair(const std::string& iniFilePath, const std::string& sectionName,
	std::vector<NameValuePair>& nameValuePairs) {
	const int bufferSize = 10000;
	char buffer[bufferSize] = "";

	int charsRead = 0;

	charsRead = GetPrivateProfileSectionA(sectionName.c_str(), buffer, bufferSize, iniFilePath.c_str());
	// if there isn't enough space, returns bufferSize - 2

	// if we got some data...
	if ((0 < charsRead) && ((bufferSize - 2) > charsRead)) {
		// walk the buffer extracting values

		// start at the beginning (const to remind us not to
		// change the contents of the buffer)
		const char* pSubstr = buffer;

		// while we have non-empty substrings...
		while ('\0' != *pSubstr) {
			// length of key-value pair substring
			size_t substrLen = strlen(pSubstr);

			// split substring on '=' char
			const char* pos = strchr(pSubstr, '=');
			if (NULL != pos) {
				// todo: remove "magic number" for buffer size 
				char name[256] = "";
				char value[256] = "";

				// if you're not using VC++ you'll prob. need to replace
				// _countof(name) with sizeof(name)/sizeof(char) and
				// similarly for value. Plus replace strncpy_s with plain
				// old strncpy.
				strncpy_s(name, _countof(name), pSubstr, pos - pSubstr);
				strncpy_s(value, _countof(value), pos + 1, substrLen - (pos - pSubstr));

				nameValuePairs.push_back(NameValuePair(name, value));
			}

			// jump over the current substring plus its null
			pSubstr += (substrLen + 1);
		}
	}
}

void dumpNameValuePairs(const std::vector<NameValuePair>& nameValuePairs) {
	typedef std::vector<NameValuePair>::size_type size_type;

	const size_type count = nameValuePairs.size();

	for (size_type index = 0; count > index; ++index) {
		const NameValuePair nameValuePair = nameValuePairs[index];
	}

	std::cout << std::endl;
}


int main(int argc, char *argv[])
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (argc >= 3) {
		std::string iniFilePath = argv[1];
		std::vector<NameValuePair> nameValuePairs;
		Ini2Pair(iniFilePath, "Options", nameValuePairs);
		dumpNameValuePairs(nameValuePairs);

		if (strcmp(argv[2], "-t11") == 0) {
			if (argc >= 3) {
				std::string dmi11;
				for (int g = 3; g <= argc - 1; g++) {
					dmi11 += argv[g];
				}
				std::stringstream ss(dmi11); //convert my_string into string stream

				std::vector<std::string> tokens;
				std::string temp_str;

				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | 6); //Yellow and bold
				printf("Note:\nIf the PC was preloaded with Windows XP, there will be a 9-digit configuration value (e.g. \"0nx141100\") and a dot preceding this conversion.\nTo readd it, write after \"FeatureByte=\" the first 7 digits and last 2 digits of the SMBIOS Type 1 System Version and a dot before you write the converted value\n");
				SetConsoleTextAttribute(hConsole, 7); //Reset

				while (getline(ss, temp_str, ';')) { //use comma as delim for cutting string
					tokens.push_back(temp_str);
				}
				for (int i = 0; (unsigned) i < tokens.size(); i++) {
					//If it is starting with "bid", then separate the equals sign
					if (tokens[i].substr(0, 3) == "bid") {
						std::cout << "BID=" << tokens[i].substr(4, tokens[i].length() - 4) << "\nFeatureByte=";
					}
					for (int g = 0; (unsigned) g < nameValuePairs.size(); g++) {
						const NameValuePair nameValuePair = nameValuePairs[g];
						if (tokens[i] == nameValuePair.second) {
							std::cout << nameValuePair.first;
						}
						
					}
					if (i > 0 && i % 2 == 0) {
						std::cout << " ";
					}
					if (tokens[i].substr(0, 1) == ".") {
						std::cout << tokens[i] << "\n";
					}
				}
			}
			else {
				//Signal an alarm to user to type them!
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED); //Red and bold
				printf("No SMBIOS Type 11 values are typed\n"); //Failed
				SetConsoleTextAttribute(hConsole, 7); //Reset
				return false;
			}
		}
		if (strcmp(argv[2], "-fbyte") == 0) {
			if (argc >= 3) {
				std::string fbyte;
				std::string t11;
				for (int g = 3; g <= argc - 1; g++) {
					fbyte += argv[g];
				}
				std::stringstream ss(fbyte); //convert my_string into string stream
				std::vector<std::string> tokens;
				std::string temp_str;

				while (getline(ss, temp_str, '.')) { //use comma as delim for cutting string
					tokens.push_back(temp_str);
				}
				for (int i = 0; (unsigned)i < tokens.size(); i++) {
					if (tokens.size() == 3) {
						if (i == 0) {
							std::cout << "SMBIOS Type 1 System Version: " << tokens[i].substr(0, 7) << "XX101XXXXX" << tokens[i].substr(7, 2) << "\n";
						}
						else {
							if (i >= 1) {
								t11 += tokens[i];
							}
						}
					}
					else {
						t11 += tokens[i];
					}
				}
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | 6); //Yellow and bold
				printf("Note:\nThere is no Build ID, so you have to add it and a semicolon (e.g. \"bid=83NAv6PrA1;\") preceding this conversion.\n");
				SetConsoleTextAttribute(hConsole, 7); //Reset

				for (int a = 0; (unsigned)a < t11.size(); a = a + 2) {
					std::string pair = t11.substr(a, 2);
					if (a == t11.size() - 2) {
						std::cout << "." << pair << ";##";
					}
					for (int g = 0; (unsigned)g < nameValuePairs.size(); g++) {
						const NameValuePair nameValuePair = nameValuePairs[g];
						if (pair == nameValuePair.first) {
							std::cout << nameValuePair.second << ";";
						}
					}
				}
			}
			else {
				//Signal an alarm to user to type them!
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED); //Red and bold
				printf("No FeatureByte sticker values are typed\n"); //Failed
				SetConsoleTextAttribute(hConsole, 7); //Reset
				return false;
			}
		}
	}
	else {
		//Print usage
		printf("Usage of program: %s <path to cpc_dmi.ini> <type> <values>\n", argv[0]);
		printf("Where:\ntype = -t11 (Type11 -> FByte), -fbyte (FByte -> Type11)\nvalues = the values\n");
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | 6); //Yellow and bold
		printf("Note:\nWhen using -fbyte option, please type the values after the \'FeatureByte=\' from the sticker\n");
		printf("Example: FeatureByte=");
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN); //Green and bold
		printf("0n4141100....\n");
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | 6); //Yellow and bold
		printf("Where the green text is the value that needs to be typed.\n");
		SetConsoleTextAttribute(hConsole, 7); //Reset
	}	
}