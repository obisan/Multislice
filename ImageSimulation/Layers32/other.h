struct common_datas
{
	int i1,i2,i3,i4,i5;
	double d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15,d16,d17,d18,d19,d20,d21,d22,d23,d24,d25,d26,d27,d28,d29,d30,d31,d32,d33;
};
struct Complex {
	float re;
	float im;
};
#define M_PI   3.1415926535898
#define M_PI_2 1.5707963267949
#define M_SQRT2 1.4142
#define complex(x, y, z)	{(z).re = (x); (z).im = (y);}
#define cadd(x, y, z)		{(z).re = (x).re + (y).re;  (z).im = (x).im + (y).im;}
#define csub(x, y, z)		{(z).re = (x).re - (y).re;  (z).im = (x).im - (y).im;}
#define cmul(x, y, z)		{(z).re = (x).re*(y).re - (x).im*(y).im; \
									(z).im = (x).im*(y).re + (x).re*(y).im;}
#define cdiv(x,y,z)			{(z).im = (y).re*(y).re+(y).im*(y).im;\
									(z).re = ((x).re*(y).re+(x).im*(y).im)/(z).im;\
									(z).im = ((x).im*(y).re-(x).re*(y).im)/(z).im;}
#define cconj(x,y) 			{(y).re = (x).re; (y).im = -(x).im;}
#define cexp(fi,z) 			{(z).re = (float)cos((fi)); (z).im = (float)sin((fi));}
#define cmod(z,d) 			(d) = hypot((z).re, (z).im)
#define cpha(z,f) 			(f) = atan2((z).im,(z).re)

#define IMAGE 0
#define SPECTR 1

///////////