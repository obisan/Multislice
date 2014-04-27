#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

using namespace std;

#define MASK "\t ,=Zchisq"
#define MASK2 "\t "

struct rec {
	float q;
	float a1, a2, a3, b1, b2, b3, c1, c2, c3, d1, d2, d3;
};

int main() {
	fstream fin("FPARAMS.DAT");
	ofstream fout("parse.Dat");
	char s[512];

	rec A;

	for(size_t i = 0; i < 103; i++) {
		
		fin.getline(s, 512, '\n');
		char *p = strtok(s, MASK);
		p = strtok(NULL, MASK);
		A.q = atof(p);
		p = nullptr;

		//////////////////////////////////////////////////////////////////////////

		fin.getline(s, 512, '\n');
		p = strtok(s, MASK2);
		A.a1 = atof(p);
		p = strtok(NULL, MASK2);
		A.b1 = atof(p);
		p = strtok(NULL, MASK2);
		A.a2 = atof(p);
		p = strtok(NULL, MASK2);
		A.b2 = atof(p);
		p = nullptr;

		//////////////////////////////////////////////////////////////////////////

		fin.getline(s, 512, '\n');
		p = strtok(s, MASK2);
		A.a3 = atof(p);
		p = strtok(NULL, MASK2);
		A.b3 = atof(p);
		p = strtok(NULL, MASK2);
		A.c1 = atof(p);
		p = strtok(NULL, MASK2);
		A.d1 = atof(p);
		p = nullptr;

		//////////////////////////////////////////////////////////////////////////

		fin.getline(s, 512, '\n');
		p = strtok(s, MASK2);
		A.c2 = atof(p);
		p = strtok(NULL, MASK2);
		A.d2 = atof(p);
		p = strtok(NULL, MASK2);
		A.c3 = atof(p);
		p = strtok(NULL, MASK2);
		A.d3 = atof(p);
		p = nullptr;

		//////////////////////////////////////////////////////////////////////////

		// mp[1] = PotentialOrder(q, a1, a2, a3, b1, b2, b3, c1, c2, c3, d1, d2, d3);
		fout	<< "mp[" << i+1 << "] = PotentialOrder(" 
				<< std::fixed << std::setprecision(7) << A.q  << "f, "
				<< std::fixed << std::setprecision(7) << A.a1 << "f, "
				<< std::fixed << std::setprecision(7) << A.a2 << "f, "
				<< std::fixed << std::setprecision(7) << A.a3 << "f, "
				<< std::fixed << std::setprecision(7) << A.b1 << "f, "
				<< std::fixed << std::setprecision(7) << A.b2 << "f, "
				<< std::fixed << std::setprecision(7) << A.b3 << "f, "
				<< std::fixed << std::setprecision(7) << A.c1 << "f, "
				<< std::fixed << std::setprecision(7) << A.c2 << "f, "
				<< std::fixed << std::setprecision(7) << A.c3 << "f, "
				<< std::fixed << std::setprecision(7) << A.d1 << "f, "
				<< std::fixed << std::setprecision(7) << A.d2 << "f, "
				<< std::fixed << std::setprecision(7) << A.d3 << "f);" << std::endl;

		//////////////////////////////////////////////////////////////////////////
		fin.getline(s, 512, '\n');
	}



	fin.close();
	return 0;
}