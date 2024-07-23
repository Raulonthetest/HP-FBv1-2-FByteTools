// HPDmiLibDecompression.cpp
// Decompressor tool for dmi.lib
#include <iostream>
#include <fstream>
#include <windows.h>


void PrintColor(HANDLE hConsole, int color, const char* format, ...) {
	va_list args;
	va_start(args, format);
	SetConsoleTextAttribute(hConsole, color);
	vprintf(format, args);
	SetConsoleTextAttribute(hConsole, 7); //Reset
	va_end(args);
}

//Extraction of directory from file name: https://stackoverflow.com/questions/8518743/get-directory-from-file-path-c

std::string ZipGetFirstFileName(std::string Zip) {
	std::ifstream ifile;
	ifile.open(Zip);
	WORD filename_len = 0;
	if (ifile) {
		//https://users.cs.jmu.edu/buchhofp/forensics/formats/pkzip.html
		//Skip PK\3\4+Ver+Flags+Comp+ModT+ModD+CRC+CSize+USize
		ifile.seekg(26);
		ifile.read((char*)&filename_len, sizeof(WORD)); //FN length is 16-bit
		//Skip extra field
		ifile.seekg(30); 
		std::string Output(filename_len, ' ');
		ifile.read(&Output[0], filename_len);
		ifile.close();
		return Output;
	}
}

std::string PathFromFullPath(std::string filename) {
	std::string directory;
	const size_t last_slash_idx = filename.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		directory = filename.substr(0, last_slash_idx + 1); //MODIFICATION: Add a backward slash
	}
	return directory;
}

int main(int argc, char *argv[])
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (argc == 2) {
		std::string Password = "yynufrQ"; //Note: this is the password used for the dmi.lib decompression found in CPC_DMI
		char* buf = nullptr; //Assign and buf char parameter
		size_t sz = 0; //Size 
		//dupenv example: https://stackoverflow.com/questions/15916695/can-anyone-give-me-example-code-of-dupenv-s/15916732
		//Note: might be modified
		if (_dupenv_s(&buf, &sz, "HPDMILIBTOOL") == 0 && buf != nullptr) //Check that the %HPDMILIBTOOL% value exists
		{
			//Yes, env value set.
			std::string DmiLibPath(argv[1]); //Convert the dmi.lib path to a string
			system(("%HPDMILIBTOOL%\\7z e " + DmiLibPath + " -p" + Password + " -o" + PathFromFullPath(DmiLibPath) + " >nul").c_str()); //Extract
			std::string CpcDmiPath = PathFromFullPath(DmiLibPath) + ZipGetFirstFileName(DmiLibPath);
			//Checking file: https://www.tutorialspoint.com/the-best-way-to-check-if-a-file-exists-using-standard-c-cplusplus
			if (ZipGetFirstFileName(DmiLibPath) != "cpc_dmi.ini") 
				PrintColor(hConsole, FOREGROUND_INTENSITY | 6, "Output filename is not cpc_dmi.ini\nPlease rename the file %s to cpc_dmi.ini\n", ZipGetFirstFileName(DmiLibPath).c_str());
			std::ifstream ifile;
			ifile.open(CpcDmiPath);
			if (ifile) {
				PrintColor(hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN, "Program has successfully decompressed the DMI.lib for HPFByteDMITool!\n");
				ifile.close();
				return true;
			}
			else {
				//Signal an alarm to user to set the value
				PrintColor(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED, "File cannot be opened\nProgram has failed decompressing the DMI.lib for HPFByteDMITool!\n");
				ifile.close();
				return false;
			}
			ifile.close();
		}
		else {
			//Signal an alarm to user to set the value
			PrintColor(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED, "HPDMILIBTOOL environment value is undefined!\n");
			printf("Please define it using set as follows:\nset HPDMILIBTOOL=<7-zip install path>\nExample: set HPDMILIBTOOL=\"C:\\Program Files\\7-Zip\"\n"); //How?
		}
		free(buf); //Clean out the environment value char
	}
	else {
		//Print usage
		printf("Usage of program: %s <path to dmi.lib>\n", argv[0]);
	}
	
}
