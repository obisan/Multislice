#include "stdafx.h"
#include "ModelCoo.h"

namespace AModel {

	ModelCoo::ModelCoo(void) {
	}

	ModelCoo::ModelCoo(const char* filename) {
		read(filename);
	}

	ModelCoo::ModelCoo(const Model& matrix, const Model& insertion) : Model(matrix, insertion) {

	}

	ModelCoo::~ModelCoo(void) {
	}

	int ModelCoo::read(const char* fileName) {
		std::ifstream fin(fileName);

		if(!fin.is_open()) {
			std::cerr << "File with name " << fileName << " doesn't exists!" << std::endl;
			return -1;
		}

		char buffer[128];
		fin.getline(buffer, 128); // comment in string
		fin.getline(buffer, 128); {
			std::replace(buffer,buffer+128, ',', '.');

			char *p = buffer;
			p = strtok(buffer, " \t");
			this->setA((float) atof(p));

			p = strtok(NULL, " \t");
			this->setB((float) atof(p));

			p = strtok(NULL, " \t");
			this->setC((float) atof(p));
			
			p = strtok(NULL, " \t");
			this->setAlpha((float) atof(p));

			p = strtok(NULL, " \t");
			this->setBeta((float) atof(p));

			p = strtok(NULL, " \t");
			this->setGamma((float) atof(p));

			p = strtok(NULL, " \t");
			this->setNumberAtoms((size_t) atol(p));
			this->setNumberAtomsActive((size_t) atol(p));
		}

		size_t size = this->getNumberAtoms() ;

		Cortege *tableCell = new Cortege[size];
		for(size_t i = 0; i < size; i++) {
			char str[256], *strsplit[7];
			fin.getline(str, 256, '\n');

			strsplit[0] = strtok(str,  " \t");
			strsplit[1] = strtok(NULL, " \t");
			strsplit[2] = strtok(NULL, " \t");
			strsplit[3] = strtok(NULL, " \t");
			strsplit[4] = strtok(NULL, " \t");
			strsplit[5] = strtok(NULL, " \t");
			strsplit[6] = strtok(NULL, " \t");

			tableCell[i].element.xsCoordinate.x = (float) StringToDouble(strsplit[0]);
			tableCell[i].element.xsCoordinate.y = (float) StringToDouble(strsplit[1]);
			tableCell[i].element.xsCoordinate.z = (float) StringToDouble(strsplit[2]);

			strcpy(tableCell[i].element.Atom, getNameByNumber( atol(strsplit[3]) ) );

			tableCell[i].element.absorbtion		= (float) StringToDouble(strsplit[4]);
			tableCell[i].element.oxygenation	= (float) StringToDouble(strsplit[5]);
			tableCell[i].element.occupation		= (float) StringToDouble(strsplit[6]);

			tableCell[i].element.R_e = 0.2f;

			//////////////////////////////////////////////////////////////////////////
			tableCell[i].isInclude = true;
		}
		this->setTableCell(tableCell);

		return 0;
	}
}