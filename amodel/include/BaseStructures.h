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

#define SYMBOLS_IN_ATOM 4

namespace AModel {
	struct AMODEL_API BariCentric {
		double m1, m2, m3, m4;
	};
	struct AMODEL_API Abc {
		Abc() {};
		Abc(float a, float b, float c) { 
			this->a = a; 
			this->b = b; 
			this->c = c; 
		}
		float a,b,c;

		friend Abc operator*(const Abc& v, const Abc u) {
			return Abc(v.a * u.a, v.b * u.b, v.c * u.c);
		}
		friend Abc operator/(const Abc& ch, const Abc& zn) {
			return Abc(ch.a/zn.a, ch.b/zn.b, ch.c/zn.c);
		}
	};
	struct AMODEL_API ABCD {
		float A,B,C,D;
	};
	struct AMODEL_API Corners {
		Corners() {};
		Corners(float alpha, float beta, float gamma) {
			this->alpha = alpha;
			this->beta = beta;
			this->gamma = gamma;
		};
		float alpha, beta, gamma;
	};

	struct AMODEL_API XYZ {
	public:
		XYZ() {}
		XYZ(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
		XYZ(const XYZ &p) {
			this->x = p.x;
			this->y = p.y;
			this->z = p.z;
		}

		float x,y,z;

		friend XYZ operator+(XYZ& a,XYZ& b) {
			return XYZ(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		friend XYZ operator-(XYZ& a, XYZ& b) {
			return XYZ(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		XYZ operator+=(const XYZ& p) {
			this->x += p.x;
			this->y += p.y;
			this->z += p.z;
			return *this;
		}
		XYZ operator-=(const XYZ& p) {
			this->x -= p.x;
			this->y -= p.y;
			this->z -= p.z;
			return *this;
		}
		XYZ operator*=(const XYZ& p) {
			this->x *= p.x;
			this->y *= p.y;
			this->z *= p.z;
			return *this;
		}
		XYZ operator/=(const XYZ& p) {
			this->x /= p.x;
			this->y /= p.y;
			this->z /= p.z;
			return *this;
		}
		XYZ operator*= (const Abc& p) {
			this->x *= p.a;
			this->y *= p.b;
			this->z *= p.c;
			return *this;
		}
		XYZ operator/=(const Abc& p) {
			this->x /= p.a;
			this->y /= p.b;
			this->z /= p.c;
			return *this;
		}
		friend XYZ operator/(const XYZ& u, const Abc& v) {
			return XYZ( u.x/v.a, u.y/v.b, u.z/v.c );
		}
		friend XYZ operator*(const XYZ& u, const Abc& v) {
			return XYZ( u.x*v.a, u.y*v.b, u.z*v.c );
		}
		
		XYZ operator*(const float& k) {
			return XYZ( k * this->x, k * this->y, k * this->z );
		}
	};

	struct AMODEL_API Record {
		int			Atom;
		float		oxygenation;
		XYZ			xsCoordinate;
		float		R_e;
		float		occupation;
		float		absorbtion;

		friend std::istream& operator>>(std::istream& in, Record& ptrc) {
			in	>> ptrc.xsCoordinate.x >> ptrc.xsCoordinate.y >> ptrc.xsCoordinate.z
				>> ptrc.Atom >> ptrc.absorbtion >> ptrc.oxygenation >> ptrc.occupation;
			ptrc.R_e = 0.2f;
			return in;
		}
	};
	struct AMODEL_API Cortege {
		bool isInclude;
		Record element;

		bool operator<(const Cortege& a) {
			return this->element.xsCoordinate.z < a.element.xsCoordinate.z;
		}
	};

	struct AMODEL_API Color{
		Color() {};
		Color(int _r, int _g, int _b) : r(_r), g(_g), b(_b) {};
		unsigned char r, g, b;
		friend std::ifstream& operator>>(std::ifstream& in, Color& prtc)
		{
			char str[256], *strsplit[3];
			in.getline(str,256,'\n');

			strsplit[0] = strtok(str," \t");
			strsplit[1] = strtok(NULL," \t");
			strsplit[2] = strtok(NULL," \t");

			prtc.r = atoi(strsplit[0]);
			prtc.g = atoi(strsplit[1]);
			prtc.b = atoi(strsplit[2]);
			return in;
		}
	};
	struct AMODEL_API Index{
		Index() {};
		Index(unsigned int i) : index(i) {};	
		unsigned int index;
	};
}