#define _CRT_SECURE_NO_WARNINGS

#include "Platform.h"
#include "La_File.h"
#include "La_Log.h"

#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <process.h>


using namespace std;



int main(int argc, char* argv[])
{
	GAMEBox gameBox;
	gameBox.create(300, 200, TEXT("Äêºó"));
	gameBox.startCommuication();
	return argc;
}


