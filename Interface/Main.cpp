#include <string>
#include <iostream>
#include <sstream>
int main(int argc, char* argv[])
{
	using namespace std;
	string regName = "c:\\hello\\hello.exe";
	string  batName = regName;
	if (batName.find('\\') != string::npos)
	{
		size_t pos = regName.find_last_of("\\/");
		batName = regName.substr(pos+1, regName.length() - pos);
	}

	batName = batName.substr(0, batName.find_last_of('.')) + ".bat";

	cout << batName;
	return 0;
}


