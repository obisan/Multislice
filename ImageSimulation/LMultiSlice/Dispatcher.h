#pragma once

class Dispatcher {
private:
	typedef struct Command {
		char fileNameInput[256];
		char fileNameOutput[256];
		char potentialDirectory[256];
		int nx;
		int ny;
		int numberSlices;
		float bindim;
		float radius;
		float dpa;

		float aperture;
		float cs;
		float defocus; // nm
		float keV;
	};

	bool	isFileExist(const char *fname);
	bool	isDirectoryExist(const char *dirname);
	int		parseCommand(const char* fileNameXML, Command& command);

protected:
	Command command;

public:
	Dispatcher(void);
	~Dispatcher(void);

	int		Run(const char* fileNameXML);
private:

};