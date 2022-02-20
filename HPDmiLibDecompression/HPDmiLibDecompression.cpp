// HPDmiLibDecompression.cpp
// Decompressor tool for dmi.lib
// Sources: https://stackoverflow.com/questions/15916695/can-anyone-give-me-example-code-of-dupenv-s/15916732 (for dupenv usage)
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h> 
#include <string>
#include <vector>
#include <windows.h>
#include <fstream>

int main(int argc, char *argv[])
{
	if (argc == 2) {
		std::string Password = "yynufrQ"; //Note: this is the password used for the dmi.lib decompression found in CPC_DMI
		char* buf = nullptr; //Assign and buf char parameter
		size_t sz = 0; //Size 
		if (_dupenv_s(&buf, &sz, "HPDMILIBTOOL") == 0 && buf != nullptr) //Check that the %HPDMILIBTOOL% value exists
		{
			std::string DmiLibPath(argv[1]); //Convert the dmi.lib path to the 
			system(("%HPDMILIBTOOL%\\7z e " + DmiLibPath + " -p" + Password).c_str());
			//File check: https://www.tutorialspoint.com/the-best-way-to-check-if-a-file-exists-using-standard-c-cplusplus
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			std::ifstream ifile;
			SetConsoleTextAttribute(hConsole, 0x0A);
			printf("Program has successfully decompressed the DMI.lib tool for HPFByteDMITool!\n");
			SetConsoleTextAttribute(hConsole, 0x07);
			free(buf);
		}
		else {
			printf("EnvVarName = FAIL\n");
		}
	}
	else {
		printf("Usage of program: %s <path to dmi.lib>\n", argv[0]);
	}
	
}