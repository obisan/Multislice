#include "StdAfx.h"
#include "Model.h"

namespace AModel {

	Model::Model(void) {
	}

	Model::Model(const char *filename) {
	}

	Model::Model(const Model& model) {
		this->a					= model.a;
		this->b					= model.b;
		this->c					= model.c;
		this->alpha				= model.alpha;
		this->beta				= model.beta;
		this->gamma				= model.gamma;
		this->numberAtoms		= model.numberAtoms;
		this->numberAtomsActive = model.numberAtomsActive;
		this->tableCell			= model.tableCell;
	}

	Model::Model(const Model& matrix, const Model& insertion) {
		this->a					= matrix.a;
		this->b					= matrix.b;
		this->c					= matrix.c;
		this->alpha				= matrix.alpha;
		this->beta				= matrix.beta;
		this->gamma				= matrix.gamma;
		this->numberAtoms		= matrix.numberAtoms + insertion.numberAtomsActive;
		this->numberAtomsActive = matrix.numberAtomsActive + insertion.numberAtomsActive;
		this->tableCell			= new Cortege[this->numberAtoms];
	}

	Model::~Model(void) {
		if( tableCell != nullptr ) {
			delete[] tableCell;
			tableCell = nullptr;
		}
	}

	float Model::getA() {
		return a;
	}
	float Model::getB() {
		return b;
	}
	float Model::getC() {
		return c;
	}
	float Model::getAlpha() {
		return alpha;
	}
	float Model::getBeta() {
		return beta;
	}
	float Model::getGamma() {
		return gamma;
	}
	size_t Model::getNumberAtoms() {
		return numberAtoms;
	}
	size_t Model::getNumberAtomsActive() {
		return numberAtomsActive;
	}
	Cortege* Model::getTableCell() {
		return tableCell;
	}
	int Model::getSpaceGroup(void) {
		return spaceGroup;
	}
	
	void Model::setA(float a) {
		this->a = a;
	}
	void Model::setB(float b) {
		this->b = b;
	}
	void Model::setC(float c) {
		this->c = c;
	}
	void Model::setAlpha(float alpha) {
		this->alpha = alpha;
	}
	void Model::setBeta(float beta) {
		this->beta = beta;
	}
	void Model::setGamma(float gamma) {
		this->gamma = gamma;
	}
	void Model::setNumberAtoms(size_t numberAtoms) {
		this->numberAtoms = numberAtoms;
	}
	void Model::setNumberAtomsActive(size_t numberAtomsActive) {
		this->numberAtomsActive = numberAtomsActive;
	}
	void Model::setTableCell(Cortege* tableCell) {
		this->tableCell = tableCell;
	}
	void Model::setSpaceGroup(int spaceGroup) {
		this->spaceGroup = spaceGroup;
	}


	int Model::writeAce(const char* filename) {
		
		char fileNameOutputWithExtention[256];
		strcpy(fileNameOutputWithExtention, filename);
		strcat(fileNameOutputWithExtention, ".ace");

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		std::ofstream fout(fileNameOutputWithExtention);
		fout.imbue(std::locale("English"));

		fout	
			<< "CaRIne Crystallography 3.0\n" 
			<< "Cell Standard ASCII File=" << fileNameOutputWithExtention << "\n"
			<< "File Version Number=1" << "\n"
			<< "\n"
			<< "Colors Refereed to: Mendeleev Table" << "\n"
			<< "\n"
			<< "----------------------"
			<< "\n"
			<< "Cell Definition" << "\n"
			<< "----------------------" << "\n"
			<< "\n"
			<< "Cell Parameters(" << "\u253C" << " and " << '\u2591' << ")" << "\n"
			<< "a=" << this->a << "\t" << "b=" << this->b << "\t" << "c=" << this->c << "\n" 
			<< "alpha=" << this->alpha << "\t" << "beta=" << this->beta << "\t" << "gamma=" << this->gamma << "\n"
			<< "\n"
			<< "System=CEL" << std::endl;
		if(1 == this->spaceGroup)
			fout << "Space Group Number=" << "not used" << "\n";
		else if( this->spaceGroup < 0 )
			fout << "Space Group Number=" << "not used" << "\n";
		else
			fout << "Space Group Number=" << spaceGroup << "\n";
		if(spaceGroup > 1)
			fout << "Only Non Equivalent Positions are listed" << "\n";

		char buffer[64];
		fout
			<< "Number of positions in Cell=" << _ltoa( (unsigned long) numberAtomsActive,buffer,10)
			<< "\n"		
			<< "                          " 
			<< "\n"
			<< "Atom	Oxi.	X	Y	Z	R(┼)	Occ." << std::endl;

		for(size_t i = 0; i < numberAtoms; i++) {
			if(tableCell[i].isInclude) {
				fout	<< tableCell[i].element.Atom << "\t"	
						<< std::fixed << std::setprecision(0) << tableCell[i].element.oxygenation << "\t"
						<< std::fixed << std::setprecision(5) << tableCell[i].element.xsCoordinate.x << "\t" 
						<< std::fixed << std::setprecision(5) << tableCell[i].element.xsCoordinate.y << "\t" 
						<< std::fixed << std::setprecision(5) << tableCell[i].element.xsCoordinate.z << "\t"
						<< std::fixed << std::setprecision(1) << tableCell[i].element.R_e << "\t"
						<< std::fixed << std::setprecision(0) << tableCell[i].element.occupation << "\n";
			}
		}

		fout.close();
		return 0;

	}
	int Model::writeCoo(const char* filename) {
		
		char fileNameOutputWithExtention[256];
		strcpy(fileNameOutputWithExtention, filename);
		strcat(fileNameOutputWithExtention, ".coo");

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		std::ofstream fout(fileNameOutputWithExtention);
		fout.imbue(std::locale("French"));
		fout.precision(7);
		fout.setf(std::ios::fixed,std::ios::floatfield);

		fout << "//file created by converter from ace CarIne "	<< fileNameOutputWithExtention << std::endl;
		fout << "   " << a		<< "   " << b		<< "   "	<< c;
		fout << "   " << alpha	<< "   " << beta	<< "   "	<< gamma;

		fout.unsetf(std::ios::showbase);
		char buffer[64];
		fout << "   " << _ltoa( (unsigned long) numberAtomsActive,buffer,10) <<std::endl;

		fout.precision(9);
		fout.setf(std::ios::fixed,std::ios::floatfield);

		//-------------------------------------------
		// Если есть файл absorbtion.txt считываем из него параметры
		// Иначе используем уже записанные в программе

		std::map<int, double> Absorb;
		std::ifstream fin("absorbtion.txt");
		if( fin.is_open() ) {
			std::pair<int,double> temp;
			std::string	NameAtom;
			int		NumAtom;
			double	dAbsorb;
			for(;!fin.eof();) {
				fin >> NameAtom >> NumAtom >> dAbsorb;
				temp = std::make_pair(NumAtom,dAbsorb);
				Absorb.insert(temp);
			}
		}
		else
			Absorb = InitAbsorb();
		fin.close();

		for(size_t i = 0; i < numberAtoms; i++)
			if(tableCell[i].isInclude) {
				fout	<< "   " << std::fixed << std::setprecision(9) << tableCell[i].element.xsCoordinate.x
					<< "   " << std::fixed << std::setprecision(9) << tableCell[i].element.xsCoordinate.y
					<< "   " << std::fixed << std::setprecision(9) << tableCell[i].element.xsCoordinate.z
					<< "   " << std::fixed << std::setprecision(0) << getNumberByName(tableCell[i].element.Atom)
					<< "   " << std::fixed << std::setprecision(3) << Absorb.find( getNumberByName(tableCell[i].element.Atom) )->second
					<< "   " << std::fixed << std::setprecision(3) << tableCell[i].element.oxygenation
					<< "   " << std::fixed << std::setprecision(4) << tableCell[i].element.occupation
					<< std::endl;
			}
		return 0;
	}
	int Model::writeXyz(const char* filename){
		char fileNameOutputWithExtention[256];
		strcpy(fileNameOutputWithExtention, filename);
		strcat(fileNameOutputWithExtention, ".xyz");

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

// 		std::ofstream fout(fileNameOutputWithExtention);
// 		fout.imbue(std::locale("French"));
// 		fout.precision(7);
// 		fout.setf(std::ios::fixed,std::ios::floatfield);
// 
// 		fout << "//file created by converter from ace CarIne "	<< fileNameOutputWithExtention << std::endl;
// 		fout << "   " << a		<< "   " << b		<< "   "	<< c;
// 		fout << "   " << alpha	<< "   " << beta	<< "   "	<< gamma;
// 
// 		fout.unsetf(std::ios::showbase);
// 		char buffer[64];
// 		fout << "   " << _ltoa( (unsigned long) numberAtomsActive,buffer,10) <<std::endl;
// 
// 		fout.precision(9);
// 		fout.setf(std::ios::fixed,std::ios::floatfield);
// 
// 		//-------------------------------------------
// 		// Если есть файл absorbtion.txt считываем из него параметры
// 		// Иначе используем уже записанные в программе
// 
// 		std::map<int, double> Absorb;
// 		std::ifstream fin("absorbtion.txt");
// 		if( fin.is_open() ) {
// 			std::pair<int,double> temp;
// 			std::string	NameAtom;
// 			int		NumAtom;
// 			double	dAbsorb;
// 			for(;!fin.eof();) {
// 				fin >> NameAtom >> NumAtom >> dAbsorb;
// 				temp = std::make_pair(NumAtom,dAbsorb);
// 				Absorb.insert(temp);
// 			}
// 		}
// 		else
// 			Absorb = InitAbsorb();
// 		fin.close();
// 
// 		for(size_t i = 0; i < numberAtoms; i++)
// 			if(tableCell[i].isInclude) {
// 				fout	<< "   " << std::fixed << std::setprecision(9) << tableCell[i].element.xsCoordinate.x
// 					<< "   " << std::fixed << std::setprecision(9) << tableCell[i].element.xsCoordinate.y
// 					<< "   " << std::fixed << std::setprecision(9) << tableCell[i].element.xsCoordinate.z
// 					<< "   " << std::fixed << std::setprecision(0) << getNumberByName(tableCell[i].element.Atom)
// 					<< "   " << std::fixed << std::setprecision(3) << Absorb.find( getNumberByName(tableCell[i].element.Atom) )->second
// 					<< "   " << std::fixed << std::setprecision(3) << tableCell[i].element.oxygenation
// 					<< "   " << std::fixed << std::setprecision(4) << tableCell[i].element.occupation
// 					<< std::endl;
// 			}
// 		

		return 0;
	}

	void Model::crop(void) {
		
		XYZ minBound(1.0f, 1.0f, 1.0f);
		XYZ maxBound(0.0f, 0.0f, 0.0f);
		for(size_t i = 0; i < numberAtoms; i++) {
			if(tableCell[i].isInclude) {
				XYZ Buffer = tableCell[i].element.xsCoordinate;
				
				if( Buffer.x > maxBound.x ) maxBound.x = Buffer.x;
				if( Buffer.y > maxBound.y ) maxBound.y = Buffer.y;
				if( Buffer.z > maxBound.z ) maxBound.z = Buffer.z;

				if( Buffer.x < minBound.x ) minBound.x = Buffer.x;
				if( Buffer.y < minBound.y ) minBound.y = Buffer.y;
				if( Buffer.z < minBound.z ) minBound.z = Buffer.z;
			}
		}

		XYZ dl( maxBound.x - minBound.x, maxBound.y - minBound.y, maxBound.z - minBound.z);

		this->a *= dl.x;
		this->b *= dl.y;
		this->c *= dl.z;

		for(size_t i = 0; i < numberAtoms; i++) {
			if(tableCell[i].isInclude) {
				tableCell[i].element.xsCoordinate -= minBound;
				tableCell[i].element.xsCoordinate /= dl;
			}
		}

	}
}