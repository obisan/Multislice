// LMultiSlice.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include "Dispatcher.h"

int main(int argc, char *argv[]) {
	char fileNameXML[256];
	if(argc == 1) {
		std::cout << "Input name XML file: " << std::endl;
		std::cin >> fileNameXML;
	}
	if(argc > 1) {
		strcpy(fileNameXML, argv[1]);
	}

	Dispatcher *task = new Dispatcher();
	int errnum = task->Run(fileNameXML);
	delete task;
	return errnum;
}