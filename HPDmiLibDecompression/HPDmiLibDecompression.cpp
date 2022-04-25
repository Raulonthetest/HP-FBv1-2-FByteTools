// HPDmiLibDecompression.cpp
// Decompressor tool for dmi.lib
#include <iostream>
#include <fstream>
#include <windows.h>

//Extraction of directory from file name: https://stackoverflow.com/questions/8518743/get-directory-from-file-path-c

std::string PathFromFullPath(std::string filename) {
	std::string directory;
	const size_t last_slash_idx = filename.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		directory = filename.substr(0, last_slash_idx + 1); //MODIFICATION: Add a backward slash
	}
	return directory;
}

std::string ZipGetFirstFileName(std::string Zip) {
	std::ifstream ifile;
	ifile.open(Zip);
	WORD filename_len = 0;
	if (ifile) {
		ifile.seekg(26); //Here might be the file name length size
		ifile.read((char *)&filename_len, sizeof(WORD));
		ifile.seekg(30);
		std::string Output(filename_len, ' ');
		ifile.read(&Output[0], filename_len);
		return Output;
	}
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
			if (ZipGetFirstFileName(DmiLibPath) != "cpc_dmi.ini") {
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | 6); //Yellow and bold
				printf("Output filename is not cpc_dmi.ini\nPlease rename the file %s to cpc_dmi.ini\n", ZipGetFirstFileName(DmiLibPath).c_str()); //Warning
				SetConsoleTextAttribute(hConsole, 7); //Reset
			}
			std::ifstream ifile;
			ifile.open(CpcDmiPath);
			if (ifile) {
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN); //Green and bold
				printf("Program has successfully decompressed the DMI.lib for HPFByteDMITool!\n"); //Done
				SetConsoleTextAttribute(hConsole, 7); //Reset
				ifile.close();
				return true;
			}
			else {
				//Signal an alarm to user to set the value
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED); //Red and bold
				printf("File cannot be opened\nProgram has failed decompressing the DMI.lib for HPFByteDMITool!\n"); //Failed
				SetConsoleTextAttribute(hConsole, 7); //Reset
				ifile.close();
				return false;
			}
			ifile.close();
		}
		else {
			//Signal an alarm to user to set the value
			SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED); //Red and bold
			printf("HPDMILIBTOOL environment value is undefined!\n"); //Problem to resolve
			SetConsoleTextAttribute(hConsole, 7); //Reset
			printf("Please define it using set as follows:\nset HPDMILIBTOOL=<7-zip install path>\nExample: set HPDMILIBTOOL=\"C:\\Program Files\\7-Zip\"\n"); //How?
		}
		free(buf); //Clean out the environment value char
	}
	else {
		//Print usage
		printf("Usage of program: %s <path to dmi.lib>\n", argv[0]);
	}
	
}
