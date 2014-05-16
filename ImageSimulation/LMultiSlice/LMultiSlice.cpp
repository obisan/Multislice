// LMultiSlice.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Dispatcher.h"

int main(int argc, char *argv[]) {
	char fileNameXML[256];
	if(argc == 1) {
		std::cin >> fileNameXML;
	}
	if(argc > 1) {
		strcpy(fileNameXML, argv[1]);
	}

	Dispatcher *task = new Dispatcher();
	task->Run(fileNameXML);
	delete task;
	return 0;
}