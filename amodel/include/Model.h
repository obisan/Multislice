#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AMODEL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AMODEL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef AMODEL_EXPORTS
#define AMODEL_API __declspec(dllexport)
#else
#define AMODEL_API __declspec(dllimport)
#endif

#define GetCurrentDir _getcwd

#include "BaseStructures.h"

namespace AModel {
	
	class AMODEL_API Model {
	public:
		Model(void);
		Model(const char *filename);
		Model(const Model& model);
		Model(const Model& matrix, const Model& insertion);
		virtual ~Model(void);

		float			getA(void);
		float			getB(void);
		float			getC(void);
		float			getAlpha(void);
		float			getBeta(void);
		float			getGamma(void);
		size_t			getNumberAtoms(void);
		size_t			getNumberAtomsActive(void);
		Cortege*		getTableCell(void);
		int				getSpaceGroup(void);

		void			setA(float a);
		void			setB(float b);
		void			setC(float c);
		void			setAlpha(float alpha);
		void			setBeta(float beta);
		void			setGamma(float gamma);
		void			setNumberAtoms(size_t numberAtoms);
		void			setNumberAtomsActive(size_t numberAtomsActive);
		void			setTableCell(Cortege* tableCell);
		void			setSpaceGroup(int spaceGroup);
		//////////////////////////////////////////////////////////////////////////

		void			freeTable();

		virtual	int read(const char* filename) = 0;
		virtual int readhead(const char* filename) = 0;
		
		int	writeAce(const char* filename);
		int writeCoo(const char* filename);
		int writeXyz(const char* filename);

		void		crop(void);
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		static __forceinline int getNumberByName(const char *name) {
			char *table[] = {
				"H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne", 
				"Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca", 
				"Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", 
				"Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", 
				"Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", 
				"Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd", 
				"Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", 
				"Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", 
				"Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th", 
				"Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", 
				"Md", "No", "Lr", "Ku"
			};

			for(int i=0;i<103;i++)
				if( strcmp(name,table[i]) == 0 )
					return i+1;
			return -1;
		}
		__forceinline const char* getNameByNumber(int number) {
				const char *table[] = {
					"H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne", 
					"Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca", 
					"Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", 
					"Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", 
					"Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", 
					"Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd", 
					"Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", 
					"Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", 
					"Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th", 
					"Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", 
					"Md", "No", "Lr", "Ku"
				};
				return table[number - 1];
			}
		__forceinline int iFindSymbol(char *str, char sym) {
				int iByfer = 0;

				for(char *p=str; *p!='\0'; ++p,++iByfer)
					if(sym == *p)
						return iByfer;
				return -1;
			}
		__forceinline float StringToDouble(const char *_str) {
			char bstr[15] = "\0";
			strcpy(bstr,_str);
			
			if( strstr(bstr, "/") ) {
				__int64 pos = strstr(bstr, "/") - bstr;

				char chisl[15], znamenat[15];
				size_t ilen;

				ilen = strlen(_str);
				strncpy(chisl, _str, pos);
				strncpy(znamenat, _str + pos + 1, ilen - pos);
				chisl[9] = znamenat[9] = '\0';

				return	(float)(atof(chisl) / atof(znamenat));
			}

			std::replace(bstr, bstr + 15, ',', '.');
			return (float) atof(bstr);

			return 0;
		}
		
	private:
		float		a;
		float		b;
		float		c;
		float		alpha;
		float		beta;
		float		gamma;
		size_t		numberAtoms;
		size_t		numberAtomsActive;
		int			spaceGroup;
		Cortege		*tableCell;

		static std::map<int,double>	InitAbsorb() {
			std::map<int,double> mp;
			mp[1] = 0.022;		mp[2] = 0.023;		mp[3] = 0.024;		mp[4] = 0.025;		
			mp[5] = 0.026;		mp[6] = 0.027;		mp[7] = 0.028;		mp[8] = 0.029;
			mp[9] = 0.03;		mp[10] = 0.031;		mp[11] = 0.032;		mp[12] = 0.033;
			mp[13] = 0.034;		mp[14] = 0.035;		mp[15] = 0.036;		mp[16] = 0.037;
			mp[17] = 0.038;		mp[18] = 0.039;		mp[19] = 0.04;		mp[20] = 0.041;
			mp[21] = 0.042;		mp[22] = 0.043;		mp[23] = 0.044;		mp[24] = 0.045;
			mp[25] = 0.046;		mp[26] = 0.047;		mp[27] = 0.048;		mp[28] = 0.049;
			mp[29] = 0.05;		mp[30] = 0.051;		mp[31] = 0.052;		mp[32] = 0.053;
			mp[33] = 0.054;		mp[34] = 0.055;		mp[35] = 0.056;		mp[36] = 0.057;
			mp[37] = 0.058;		mp[38] = 0.059;		mp[39] = 0.06;		mp[40] = 0.061;
			mp[41] = 0.062;		mp[42] = 0.063;		mp[43] = 0.064;		mp[44] = 0.065;
			mp[45] = 0.066;		mp[46] = 0.067;		mp[47] = 0.068;		mp[48] = 0.069;
			mp[49] = 0.07;		mp[50] = 0.071;		mp[51] = 0.072;		mp[52] = 0.073;
			mp[53] = 0.074;		mp[54] = 0.075;		mp[55] = 0.076;		mp[56] = 0.077;
			mp[57] = 0.078;		mp[58] = 0.079;		mp[59] = 0.08;		mp[60] = 0.081;
			mp[61] = 0.082;		mp[62] = 0.083;		mp[63] = 0.084;		mp[64] = 0.085;
			mp[65] = 0.086;		mp[66] = 0.087;		mp[67] = 0.088;		mp[68] = 0.089;
			mp[69] = 0.09;		mp[70] = 0.091;		mp[71] = 0.092;		mp[72] = 0.093;
			mp[73] = 0.094;		mp[74] = 0.095;		mp[75] = 0.096;		mp[76] = 0.097;
			mp[77] = 0.098;		mp[78] = 0.099;		mp[79] = 0.1;		mp[80] = 0.101;
			mp[81] = 0.102;		mp[82] = 0.103;		mp[83] = 0.104;		mp[84] = 0.105;
			mp[85] = 0.106;		mp[86] = 0.107;		mp[87] = 0.108;		mp[88] = 0.109;
			mp[89] = 0.11;		mp[90] = 0.111;		mp[91] = 0.112;		mp[92] = 0.113;
			mp[93] = 0.114;		mp[94] = 0.115;		mp[95] = 0.116;		mp[96] = 0.117;
			mp[97] = 0.118;		mp[98] = 0.11;
			return mp;
		}
	};
}