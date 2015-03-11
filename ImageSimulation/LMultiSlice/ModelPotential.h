#pragma once

#include "stdafx.h"
#include "Microscope.h"

//////////////////////////////////////////////////////////////////////////
// 64 KB = 65536 bytes
// 65536 - 9888 = 55648  // 4608 * 3 * 4 = 55296
// 55648 - 55296 = 352 lost bytes
// 352 - 112 = 240 bytes
// 240 - 128 = 112 bytes
//////////////////////////////////////////////////////////////////////////

#define ATOMS_IN_CONST_MEMORY 4608
#define ATOMS_IN_CONST_MEMORY_MULTIPLICATOR 2

#define UNROLLX		8
#define UNROLLY		1
#define BLOCKSIZEX	16
#define BLOCKSIZEY	16
#define BLOCKSIZE	 (BLOCKSIZEX*BLOCKSIZEY)

__constant__ double a_d;
__constant__ double b_d;
__constant__ double c_d;
__constant__ double bindimx_d;
__constant__ double bindimy_d;

__constant__ double dx_d;
__constant__ double dy_d;
__constant__ int binx_d;
__constant__ int biny_d;
__constant__ double radius_d;


//__constant__ int	atominfoid[ATOMS_IN_CONST_MEMORY];
//__constant__ float	atominfoxy[ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * ATOMS_IN_CONST_MEMORY];

class ModelPotential {
public:
	ModelPotential(void);
	ModelPotential(AModel::Model *model, size_t nx, size_t ny, size_t nz, double dpa, double radius);
	~ModelPotential(void);

	int		calculatePotentialGrid();
	int		savePotential(const char* filename);
	
	AModel::Model*		getModel();
	size_t	getNx();
	size_t	getNy();
	size_t	getNz();
	
	double				*potential;

private:
	AModel::Model		*model;
	size_t				nx;
	size_t				ny;
	size_t				nz;
	
	double				radius;
	double				dpa;
};

//////////////////////////////////////////////////////////////////////////
// 112 bytes
//////////////////////////////////////////////////////////////////////////

__constant__ double k0a[] = { 
	-0.57721566, 
	 0.42278420, 
	 0.23069756,
	 0.03488590, 
	 0.00262698, 
	 0.00010750, 
	 0.00000740
};

__constant__ double k0b[] = { 
	 1.25331414, 
	-0.07832358, 
	 0.02189568,
	-0.01062446, 
	 0.00587872, 
	-0.00251540, 
	 0.00053208
};

//////////////////////////////////////////////////////////////////////////
// 128 bytes
//////////////////////////////////////////////////////////////////////////

__constant__ double i0a[] = { 
	1.0, 
	3.5156229, 
	3.0899424, 
	1.2067492,
	0.2659732, 
	0.0360768, 
	0.0045813 
};
__constant__ double i0b[] = { 
	 0.39894228, 
	 0.01328592, 
	 0.00225319,
	-0.00157565, 
	 0.00916281, 
	-0.02057706, 
	 0.02635537,	
	-0.01647633, 
	 0.00392377
};

//////////////////////////////////////////////////////////////////////////
// 9888 bytes
//////////////////////////////////////////////////////////////////////////
__constant__ double FParamsDevice[] = {
	// Z=  1,  chisq=    0.170190
	3.55221981e-002,2.25354459e-001,2.62782423e-002,2.25354636e-001,
	3.52695173e-002,2.25355749e-001,6.77755867e-002,4.38850114e+000,
	3.56601775e-003,4.03881150e-001,2.76131055e-002,1.44488619e+000,

	// Z=  2,  chisq=    0.013113
	3.73696013e-002,5.36479656e-001,3.60228922e-002,5.36480729e-001,
	1.22544155e-001,5.36493547e-001,5.06400486e-003,2.70930915e-001,
	2.03620941e-002,8.51962305e-001,2.74236552e-002,2.33542971e+000,

	// Z=  3,  chisq=    0.258875
	1.07960369e-001,9.23258728e-001,1.06777482e-001,9.23233431e-001,
	7.71392003e-002,4.74123946e-002,2.75244208e-003,1.69761557e-001,
	1.08034077e+000,1.90675572e+001,3.46769903e-001,6.28398894e+000,

	// Z=  4,  chisq=    0.222811
	1.96662954e-001,1.95719345e+000,8.82912188e-002,1.00163923e-001,
	1.03185752e-001,1.00168729e-001,7.71828818e-001,9.41653240e+000,
	2.71710968e-001,3.26602502e+000,1.71561398e-003,9.36064197e-002,

	// Z=  5,  chisq=    0.180015
	1.50630127e-001,1.49860678e-001,1.78976266e-001,3.60414978e+000,
	1.54917583e-001,1.49855459e-001,1.23462541e-003,6.10670225e-002,
	1.93865663e-001,2.02637930e+000,5.14164144e-001,5.78223175e+000,

	// Z=  6,  chisq=    0.162305
	2.53148417e-001,2.08761775e-001,1.66953010e-001,5.69804854e+000,
	1.60250013e-001,2.08774166e-001,1.41633354e-001,1.34112859e+000,
	3.60244465e-001,3.81665326e+000,8.66192838e-004,4.12204523e-002,

	// Z=  7,  chisq=    0.095853
	2.81561298e-001,2.83238399e-001,1.46347552e-001,1.05047554e+001,
	2.50794513e-001,2.83236538e-001,9.21173808e-004,3.56658185e-002,
	1.07049727e-001,9.94861556e-001,2.49131328e-001,2.74883575e+000,

	// Z=  8,  chisq=    0.058116
	3.68590229e-001,3.67542561e-001,1.32896450e-001,1.93954830e+001,
	2.77101233e-001,3.67539567e-001,8.39309305e-002,7.58355003e-001,
	1.81631127e-001,2.04360334e+000,1.05289871e-003,3.12500946e-002,

	// Z=  9,  chisq=    0.037944
	3.91424182e-001,4.70917753e-001,1.27189158e-001,4.16483304e+001,
	3.65098718e-001,4.70915966e-001,1.34924127e-003,2.68370605e-002,
	1.45113404e-001,1.57347346e+000,6.96750443e-002,5.91880259e-001,

	// Z= 10,  chisq=    0.029008
	9.11673340e-002,1.99358219e+002,8.63172759e-002,1.99191327e+002,
	8.63108401e-001,5.88361692e-001,1.23641141e-001,1.23470914e+000,
	1.91465709e-003,1.96822966e-002,5.91805145e-002,4.64885011e-001,

	// Z= 11,  chisq=    0.065937
	1.36017407e-001,4.95428367e-002,1.54831417e-001,1.65397748e+001,
	7.73349869e-001,8.82918067e-001,1.27047208e-001,7.00982715e-001,
	3.79285907e-002,2.61475739e-001,9.87509284e-001,1.99616037e+001,

	// Z= 12,  chisq=    0.060723
	3.03141167e-001,8.34631821e-002,1.00200593e-001,3.05607062e+001,
	7.58779849e-001,1.63636518e+000,3.45903833e-002,2.17635235e-001,
	1.20116322e-001,5.62363305e-001,9.59830513e-001,1.20698504e+001,

	// Z= 13,  chisq=    0.066504
	7.79634560e-001,2.72405892e+000,4.26174959e-001,9.11375865e-002,
	5.72300972e-002,7.97635831e+001,3.27350675e-002,1.79698265e-001,
	7.85910770e-001,8.64611280e+000,1.13704700e-001,4.50774270e-001,

	// Z= 14,  chisq=    0.094315
	1.20120147e-001,7.06310153e+001,1.06498026e+000,1.04600373e+000,
	1.82256296e-001,8.68866872e-002,3.06076088e-002,2.14762808e-001,
	1.10867696e+000,3.69205948e+000,1.58258089e+000,9.93119843e+000,

	// Z= 15,  chisq=    0.077296
	3.09711164e-001,1.24791065e-001,1.18252680e-001,1.09178460e+002,
	1.04430670e+000,1.34549595e+000,2.66000245e-002,1.83098148e-001,
	9.43325900e-001,2.88341609e+000,1.34138576e+000,7.54960845e+000,

	// Z= 16,  chisq=    0.066482
	4.57382135e-001,1.68476043e-001,1.27262739e-001,1.93866119e+002,
	1.00335412e+000,1.74538684e+000,2.36808666e-002,1.58174585e-001,
	1.13442225e+000,5.93263595e+000,8.08008430e-001,2.31591942e+000,

	// Z= 17,  chisq=    0.233948
	3.12243550e-001,9.47996070e+000,3.83195418e-001,9.48012637e+000,
	9.41823736e-001,2.35734126e-001,5.59701797e-002,1.68708567e-001,
	1.09028166e-002,6.75771202e-002,7.79430014e-001,2.98107659e+000,

	// Z= 18,  chisq=    0.292651
	3.38555465e-001,1.30104349e+001,3.31436858e-001,1.30102849e+001,
	1.07106900e+000,2.88010245e-001,4.92223623e-002,1.36749282e-001,
	8.55589133e-003,5.18043063e-002,7.63912327e-001,2.44189141e+000,

	// Z= 19,  chisq=    0.201242
	6.89812600e-001,7.19216212e+000,1.46536847e-001,3.78571593e-002,
	9.70417012e-001,3.59137547e-001,5.45732070e-001,1.68372903e+000,
	1.74740906e+000,3.21777454e+001,2.65720079e-002,1.03365168e-001,

	// Z= 20,  chisq=    0.114232
	8.71890292e-001,5.86619912e-001,6.58352512e-001,8.00437197e+000,
	3.70818603e-001,6.13547057e-002,2.21094483e-002,9.08697675e-002,
	1.78897674e+000,2.10605140e+001,4.93215780e-001,1.33112304e+000,

	// Z= 21,  chisq=    0.074429
	9.12261547e-001,5.19023783e-001,7.11659199e-001,7.99293247e+000,
	3.70782468e-001,6.76760444e-002,1.89930612e-002,8.15872399e-002,
	1.56348982e+000,1.78791604e+001,4.07326109e-001,1.11474352e+000,

	// Z= 22,  chisq=    0.052913
	7.37291729e-001,8.32612403e+000,9.96300042e-001,4.96982599e-001,
	3.55417565e-001,7.26595855e-002,1.63350335e-002,7.31735660e-002,
	3.60199179e-001,9.57232308e-001,1.42171303e+000,1.58512114e+001,

	// Z= 23,  chisq=    0.042843
	3.40419589e-001,7.72885577e-002,7.44572000e-001,8.94326818e+000,
	1.09904818e+000,5.04948256e-001,1.42047505e-002,6.56803788e-002,
	3.27534098e-001,8.38161439e-001,1.30529927e+000,1.43590348e+001,

	// Z= 24,  chisq=    0.027798
	3.92063635e-001,7.35286531e-002,1.34668097e+000,1.12818504e+000,
	5.39341680e-001,1.14415046e+001,4.85525850e-001,1.58837898e+000,
	1.19711645e-002,5.97713788e-002,3.37954593e-001,6.74850221e-001,

	// Z= 25,  chisq=    0.037211
	7.17361123e-001,1.12697838e+001,1.32828136e+000,5.80850091e-001,
	3.31233708e-001,8.73514102e-002,2.76477361e-001,6.69130447e-001,
	1.13628473e-002,5.32732815e-002,1.08008900e+000,1.21682893e+001,

	// Z= 26,  chisq=    0.036059
	1.44902411e+000,6.38640902e-001,3.35804378e-001,9.30314230e-002,
	6.90162940e-001,1.31851881e+001,1.05054521e-002,4.82320902e-002,
	2.54242669e-001,6.06112572e-001,9.70992561e-001,1.12847296e+001,

	// Z= 27,  chisq=    0.034915
	3.39463008e-001,9.83337442e-002,1.57689974e+000,7.04328108e-001,
	6.58282438e-001,1.58689413e+001,2.34491895e-001,5.53325204e-001,
	9.93605905e-003,4.38330874e-002,8.76823538e-001,1.05638438e+001,

	// Z= 28,  chisq=    0.033444
	1.70434795e+000,7.75692056e-001,3.44977435e-001,1.03825110e-001,
	6.26884420e-001,1.95735691e+001,2.16907769e-001,5.07719273e-001,
	9.59473827e-003,3.99824503e-002,7.89344693e-001,9.91968148e+000,

	// Z= 29,  chisq=    0.010060
	1.76966715e+000,9.78202148e-001,3.43918043e-001,1.02654437e-001,
	6.45510267e-001,1.56360030e+001,2.15730439e-001,8.68921347e-001,
	7.68971207e-003,3.87041794e-002,1.79557785e-001,3.94476391e-001,

	// Z= 30,  chisq=    0.030575
	2.00111033e+000,2.19603648e+000,5.70973035e-001,1.26562618e-001,
	3.04235247e-001,4.65681839e+001,3.97986959e-001,8.69664235e-001,
	7.26712178e-003,3.30179102e-002,2.37377314e-001,3.68690692e-001,

	// Z= 31,  chisq=    0.011003
	2.04315340e+000,2.32649260e+000,6.21558623e-001,1.09358880e-001,
	3.12804866e-001,5.26676038e+001,3.31510041e-001,7.44158557e-001,
	7.03054223e-003,3.07180585e-002,2.04524025e-001,3.29853209e-001,

	// Z= 32,  chisq=    0.037491
	5.88274727e-001,1.16695039e-001,2.01158811e+000,1.39466069e+000,
	5.56077128e-001,8.12542932e+001,9.93470453e-003,2.66995907e-002,
	1.83769260e-001,3.65293264e-001,7.43395173e-001,6.91755967e+000,

	// Z= 33,  chisq=    0.040728
	7.66111954e-001,1.44710861e-001,1.97328728e+000,1.74141662e+000,
	6.67404866e-001,1.88379573e+002,1.00067871e-002,2.27061261e-002,
	1.80181966e-001,3.32551336e-001,8.23463715e-001,5.85804925e+000,

	// Z= 34,  chisq=    0.044682
	9.36712842e-001,1.75282629e-001,5.93283862e-001,1.79861777e+002,
	1.92387969e+000,2.10899623e+000,1.72794212e-001,2.99451919e-001,
	8.68719842e-003,2.16039688e-002,8.92813053e-001,4.91654242e+000,

	// Z= 35,  chisq=    0.052091
	1.86778810e+000,2.65471268e+000,4.83354630e-001,1.71232184e+002,
	1.13299751e+000,2.13736097e-001,1.68761916e-001,2.72214527e-001,
	6.99360414e-003,1.99802395e-002,9.41278062e-001,4.19231938e+000,

	// Z= 36,  chisq=    0.058423
	1.81058198e+000,3.27014311e+000,3.72947511e-001,1.22209190e+002,
	1.31922133e+000,2.54796158e-001,1.62403804e-001,2.46303004e-001,
	5.49953845e-003,1.96113077e-002,1.00104155e+000,3.60853216e+000,

	// Z= 37,  chisq=    0.127530
	2.85527482e-001,3.60932992e-002,2.44111688e+000,1.14936063e+000,
	7.81237146e-001,1.72500088e+001,2.92384015e-003,1.99524593e-002,
	1.28673293e-001,2.09675920e-001,1.59752121e+000,2.54634972e+000,

	// Z= 38,  chisq=    0.199389
	1.59192012e+000,2.23207098e-001,1.68248494e-002,1.86484813e-002,
	1.97760024e+000,6.41852247e+000,4.59884866e+000,2.52850287e+001,
	1.56342068e-003,1.60766776e-002,1.74105846e-001,2.01554275e-001,

	// Z= 39,  chisq=    0.158793
	5.66272764e-001,1.64766253e+002,6.68703227e-001,6.47658582e-002,
	2.63607208e+000,2.04221419e+000,1.08344825e-001,1.92053346e-001,
	6.41343317e-003,1.67973039e-002,9.70128007e-001,1.64599002e+000,

	// Z= 40,  chisq=    0.083769
	2.63347251e+000,2.19895309e+000,7.64408217e-001,7.68402245e-002,
	6.10028075e-001,1.99121057e+002,6.42729406e-003,1.60004229e-002,
	9.95993302e-002,1.75460429e-001,9.48199859e-001,1.54054741e+000,

	// Z= 41,  chisq=    0.025638
	1.35352192e+000,5.77442956e-001,5.65842796e-001,7.33200498e-002,
	1.86474570e+000,6.26155384e+000,2.25975616e-003,8.42151031e-003,
	1.25360769e-001,1.66326893e-001,6.93436708e-001,1.65556162e+000,

	// Z= 42,  chisq=    0.022570
	1.40409157e+000,4.89232074e-001,5.15300659e-001,7.44612001e-002,
	1.95943005e+000,6.36928856e+000,2.17040059e-003,8.00554136e-003,
	1.17234424e-001,1.54756105e-001,6.41651415e-001,1.49254426e+000,

	// Z= 43,  chisq=    0.021671
	1.52561496e+000,4.61642010e-001,4.66879943e-001,7.56100647e-002,
	1.99693982e+000,6.89361912e+000,1.88722662e-003,7.81640595e-003,
	1.10836728e-001,1.43762834e-001,6.24441882e-001,1.36894113e+000,

	// Z= 44,  chisq=    0.022180
	1.67690451e+000,4.60569157e-001,4.23656418e-001,7.68125742e-002,
	2.01399773e+000,7.75226552e+000,1.44624157e-003,7.91508879e-003,
	1.06065006e-001,1.33647930e-001,6.17227633e-001,1.26954782e+000,

	// Z= 45,  chisq=    0.020729
	2.03041693e+000,9.12446902e+000,1.80806943e+000,6.22608459e-001,
	4.56384514e-001,8.41306295e-002,6.14124238e-001,1.04929600e+000,
	4.85904931e-001,2.41429268e+000,1.07395444e-001,1.28204672e-001,

	// Z= 46,  chisq=    0.014698
	1.40024773e+000,5.69044238e-001,8.51757922e-001,2.18223200e-001,
	2.13942237e+000,9.23664366e+000,1.01584983e-001,1.20176785e-001,
	5.48186638e-001,9.70932302e-001,3.36249115e-001,2.04303364e+000,

	// Z= 47,  chisq=    0.010637
	2.06545482e+000,1.00294175e+001,1.97861371e+000,6.48653295e-001,
	4.44925769e-001,1.05034725e-001,6.05726204e-001,8.97146128e-001,
	4.79698645e-001,2.02284806e+000,9.42835014e-002,1.12449844e-001,

	// Z= 48,  chisq=    0.009474
	2.24463477e+000,1.51925685e+000,1.57427485e+000,1.29094546e+001,
	7.69489325e-001,1.26435460e-001,7.81998614e-001,1.67898091e+000,
	8.52678993e-002,1.05413559e-001,6.84506524e-001,7.81871299e-001,

	// Z= 49,  chisq=    0.006561
	1.67551123e+000,1.29455813e+001,7.79993853e-001,1.05245421e-001,
	2.22926081e+000,1.45519888e+000,6.37891580e-001,1.52411951e+000,
	8.10614604e-002,9.91121572e-002,6.43859303e-001,7.27431452e-001,

	// Z= 50,  chisq=    0.009942
	1.94582842e+000,6.82673369e-001,6.80166191e-001,1.05877615e-001,
	2.15440714e+000,1.14228691e+001,5.68265062e-001,7.36414566e-001,
	7.79170289e-002,9.29022081e-002,8.95350509e-001,1.03686032e+001,

	// Z= 51,  chisq=    0.010186
	9.68877676e-001,1.30335360e-001,2.12500803e+000,1.20977101e+001,
	1.78338958e+000,8.88156418e-001,7.35824061e-002,8.76447076e-002,
	5.64027378e-001,6.75557371e-001,9.30811770e-001,8.83064772e+000,

	// Z= 52,  chisq=    0.010763
	1.27344215e+000,1.57788933e-001,1.62764774e+000,1.21424711e+000,
	2.07302198e+000,1.28819976e+001,5.58268716e-001,6.20292928e-001,
	9.56477889e-001,7.49779327e+000,6.95991293e-002,8.28130056e-002,

	// Z= 53,  chisq=    0.011914
	1.53843386e+000,1.84170390e+000,1.94621532e+000,1.40421669e+001,
	1.58652639e+000,1.90139677e-001,6.57588256e-002,7.83316661e-002,
	5.56768791e-001,5.70375259e-001,1.04577707e+000,6.42642720e+000,

	// Z= 54,  chisq=    0.013735
	1.73100679e+000,3.20678037e+000,1.58001908e+000,1.67039219e+001,
	1.85812966e+000,2.22941867e-001,5.62237832e-001,5.23769323e-001,
	6.17088447e-002,7.41544356e-002,1.20497382e+000,5.57269861e+000,

	// Z= 55,  chisq=    0.062415
	3.04137820e+000,6.11340227e+000,1.88948758e+000,1.79102084e-001,
	3.59374559e-001,6.51499993e+001,5.95458685e-001,4.71542621e-001,
	5.27920426e-002,7.11005386e-002,4.78902428e+000,4.68489721e+001,

	// Z= 56,  chisq=    0.055088
	1.99150210e+000,2.29209324e-001,1.50114565e-001,3.51650757e-002,
	3.19191823e+000,9.32261575e+000,5.41558677e-001,4.07988823e-001,
	4.36485437e+000,2.94414103e+001,6.76182713e-002,7.30293827e-002,

	// Z= 57,  chisq=    0.045866
	2.04923426e+000,2.21830616e-001,1.47476445e-001,3.95389433e-002,
	3.23266281e+000,9.63261028e+000,5.19082726e-001,3.82888701e-001,
	6.38501535e-002,6.91797306e-002,3.92370809e+000,2.52763920e+001,

	// Z= 58,  chisq=    0.044172
	2.19356433e+000,2.61252735e-001,1.46109209e-001,3.63856298e-002,
	3.18367409e+000,1.01129195e+001,5.00456755e-001,3.69817153e-001,
	5.97513353e-002,6.60687689e-002,4.10702583e+000,2.70334856e+001,

	// Z= 59,  chisq=    0.040241
	2.28227779e+000,2.81609709e-001,1.56132264e-001,3.78658328e-002,
	3.17928914e+000,1.04954984e+001,4.83692108e-001,3.53279981e-001,
	3.91967548e+000,2.60687594e+001,5.62653317e-002,6.30212606e-002,

	// Z= 60,  chisq=    0.036942
	1.70070911e-001,3.96603263e-002,3.17547275e+000,1.08661468e+001,
	2.36614036e+000,3.03797233e-001,5.30180262e-002,6.02074933e-002,
	4.68633234e-001,3.38020625e-001,3.71629807e+000,2.52142568e+001,

	// Z= 61,  chisq=    0.034172
	3.17227226e+000,1.12242389e+001,1.85373096e-001,4.15452310e-002,
	2.44767363e+000,3.27263223e-001,4.54820811e-001,3.23834613e-001,
	4.99757326e-002,5.75952687e-002,3.51307253e+000,2.44390985e+001,

	// Z= 62,  chisq=    0.031928
	2.52353349e+000,3.55216512e-001,3.16752799e+000,1.15550895e+001,
	2.07170064e-001,4.37577914e-002,4.70571915e-002,5.52003558e-002,
	3.29394311e+000,2.38725176e+001,4.43169078e-001,3.11132415e-001,

	// Z= 63,  chisq=    0.029840
	2.19629685e-001,4.55336338e-002,3.16655284e+000,1.18902863e+001,
	2.60514922e+000,3.78726321e-001,4.43663569e-002,5.28975171e-002,
	4.30354279e-001,2.98428320e-001,3.11652720e+000,2.31564236e+001,

	// Z= 64,  chisq=    0.024918
	2.56040507e+000,4.13742703e-001,3.18033559e+000,1.22166434e+001,
	3.44447712e-001,5.89612434e-002,4.20231451e-002,5.06244123e-002,
	2.59458068e+000,1.96541978e+001,4.17975309e-001,2.84689722e-001,

	// Z= 65,  chisq=    0.026512
	2.76420840e+000,4.30791951e-001,2.48097398e-001,4.92596449e-002,
	3.16384184e+000,1.25013461e+001,4.07575165e-001,2.75744158e-001,
	2.77547386e+000,2.19804226e+001,3.93232928e-002,4.87225318e-002,

	// Z= 66,  chisq=    0.025206
	3.16299861e+000,1.27663229e+001,2.63038983e-001,5.11867634e-002,
	2.84173986e+000,4.58694334e-001,3.97231600e-001,2.65551211e-001,
	3.69364362e-002,4.68204306e-002,2.61539652e+000,2.14888978e+001,

	// Z= 67,  chisq=    0.024176
	2.91672105e+000,4.90059921e-001,3.16169839e+000,1.29749346e+001,
	2.79958522e-001,5.31105824e-002,3.45486702e-002,4.50444713e-002,
	2.45919711e+000,2.11348304e+001,3.88315334e-001,2.56384612e-001,

	// Z= 68,  chisq=    0.023210
	2.99287944e+000,5.20631672e-001,2.94524255e-001,5.50008088e-002,
	3.16133517e+000,1.31671985e+001,3.79199707e-001,2.47566628e-001,
	3.22641272e-002,4.33328235e-002,2.31907704e+000,2.07628171e+001,

	// Z= 69,  chisq=    0.022536
	3.16105163e+000,1.32842507e+001,3.11066167e-001,5.69067186e-002,
	3.06573492e+000,5.54866486e-001,3.71468784e-001,2.39694565e-001,
	2.99316352e-002,4.17152771e-002,2.18222658e+000,2.05131376e+001,

	// Z= 70,  chisq=    0.022167
	3.22683356e-001,5.83335787e-002,3.15991383e+000,1.33457200e+001,
	3.14342469e+000,5.90650443e-001,2.75822501e-002,4.01552784e-002,
	3.64345350e-001,2.32537409e-001,2.07830373e+000,2.04175608e+001,

	// Z= 71,  chisq=    0.018570
	3.11317236e+000,6.55644698e-001,3.14096616e+000,1.38243324e+001,
	4.65896574e-001,7.32484711e-002,2.61744934e-002,3.84636538e-002,
	1.77613237e+000,1.72013614e+001,3.50786133e-001,2.22653886e-001,

	// Z= 72,  chisq=    0.016212
	3.15920241e+000,1.40083544e+001,6.13291586e-001,8.84711068e-002,
	3.03905654e+000,7.09697422e-001,3.41530456e-001,2.13687522e-001,
	2.46166752e-002,3.69228699e-002,1.43601570e+000,1.50685398e+001,

	// Z= 73,  chisq=    0.014866
	7.40719823e-001,1.02352889e-001,3.22045387e+000,1.38766179e+001,
	2.93947802e+000,7.39861702e-001,2.29176351e-002,3.54950002e-002,
	3.35663858e-001,2.05499378e-001,1.14728503e+000,1.35849981e+001,

	// Z= 74,  chisq=    0.014022
	3.31943502e+000,1.35186688e+001,8.35594773e-001,1.14555762e-001,
	2.83308303e+000,7.40021820e-001,3.31641152e-001,1.97939826e-001,
	2.11358354e-002,3.41206687e-002,9.17225714e-001,1.24413751e+001,

	// Z= 75,  chisq=    0.007799
	4.17974495e+000,1.92001537e+000,1.96702250e+000,6.46154266e+001,
	1.23493875e+000,1.32861245e-001,2.26750720e-001,1.92660999e-001,
	3.11488515e-002,2.74803921e-002,6.06340966e-001,1.33511080e+000,

	// Z= 76,  chisq=    0.005462
	4.08655158e+000,1.93580909e+000,2.01164670e+000,5.56316275e+001,
	1.32570572e+000,1.46309560e-001,2.22498621e-001,1.80986240e-001,
	2.98348539e-002,2.74143576e-002,6.22736863e-001,1.35163741e+000,

	// Z= 77,  chisq=    0.011540
	2.69057080e+000,6.74459257e-001,9.38687540e-001,1.63865804e-001,
	3.62399707e+000,1.27030013e+001,2.32148900e-001,1.69780069e+000,
	1.65466836e-002,3.00517307e-002,3.16662079e-001,1.77303963e-001,

	// Z= 78,  chisq=    0.009591
	2.95194633e+000,6.35078046e-001,3.67931581e+000,1.28458980e+001,
	7.14543243e-001,1.25851086e-001,3.06958225e-001,1.70029438e-001,
	1.56846790e-002,2.87844614e-002,2.42876856e-001,1.50098529e+000,

	// Z= 79,  chisq=    0.008579
	8.70155469e-001,1.62787604e-001,3.72985450e+000,1.30045249e+001,
	2.83853565e+000,6.52969096e-001,1.48964674e-002,2.75909732e-002,
	2.98381945e-001,1.63263715e-001,2.82816664e-001,1.38490480e+000,

	// Z= 80,  chisq=    0.007102
	2.78374208e+000,8.22277972e-001,1.14750780e+000,1.72124985e-001,
	3.60939453e+000,1.42313237e+001,3.64393244e-001,1.27646677e+000,
	1.51112082e-002,2.66239468e-002,2.83023324e-001,1.55314968e-001,

	// Z= 81,  chisq=    0.006581
	8.23389649e-001,1.18852491e-001,3.74903373e+000,1.37602314e+001,
	3.05433732e+000,7.09486570e-001,1.37349782e-002,2.53665959e-002,
	2.77871511e-001,1.50085183e-001,3.11190199e-001,1.09925720e+000,

	// Z= 82,  chisq=    0.005419
	3.13057971e+000,8.80515828e-001,9.94683454e-001,1.15403295e-001,
	3.60607274e+000,1.52496358e+001,4.04639927e-001,1.05015906e+000,
	2.63994669e-001,1.42757976e-001,1.41905960e-002,2.46167794e-002,

	// Z= 83,  chisq=    0.005521
	3.30916193e+000,1.31950436e+000,3.21962651e+000,1.91084245e+001,
	1.32348123e+000,1.31336111e-001,2.44404844e-001,1.34827998e-001,
	1.58187258e-002,2.40542959e-002,5.54231182e-001,9.93158935e-001,

	// Z= 84,  chisq=    0.006520
	4.14508732e+000,2.42038875e+000,2.25541764e+000,4.36660720e+001,
	1.68019328e+000,1.53550792e-001,2.17870243e-002,2.26801429e-002,
	2.02365227e-001,1.26847183e-001,7.29107889e-001,8.89042684e-001,

	// Z= 85,  chisq=    0.023062
	4.66781768e+000,3.82705270e+000,2.03510501e+000,1.78311523e-001,
	1.85008958e+000,1.57352238e+002,7.12132748e-001,7.27284557e-001,
	2.27295682e-002,1.84141534e-002,1.97009448e-001,1.30229758e-001,

	// Z= 86,  chisq=    0.054023
	3.89505614e+000,6.53799216e-001,6.36434744e-001,1.40722385e-001,
	3.84162464e+000,3.27370250e+001,3.38439801e-002,2.45079615e-002,
	2.43383919e-001,1.08483748e-001,2.62454829e+000,6.42960455e+000,

	// Z= 87,  chisq=    0.073876
	4.34105838e+000,6.59940116e-001,3.54331622e-001,3.56576122e-002,
	3.93270123e+000,4.46643573e+001,4.16463487e-002,2.36569107e-002,
	2.35862196e-001,1.01611484e-001,1.85880940e+000,4.69955953e+000,

	// Z= 88,  chisq=    0.088402
	1.41427764e+000,1.14443692e-001,3.56956187e+000,1.00348650e+000,
	4.01764459e+000,6.91550954e+001,4.92624882e-002,2.21279787e-002,
	2.18417884e-001,9.54875086e-002,4.31039135e+000,3.75373809e+001,

	// Z= 89,  chisq=    0.078070
	2.74063627e+000,5.59126060e-001,8.95943437e-001,6.11604537e-002,
	4.72251448e+000,1.28205776e+001,3.50544083e-001,4.65896030e-001,
	2.40318502e-001,1.12634514e-001,8.78520325e-003,1.83968309e-002,

	// Z= 90,  chisq=    0.052245
	2.52915962e+000,6.26480940e-001,1.15046406e+000,7.55226690e-002,
	4.76962590e+000,1.29056834e+001,3.70677832e-001,4.51784795e-001,
	2.32505653e-001,1.08856829e-001,8.20413411e-003,1.75054624e-002,

	// Z= 91,  chisq=    0.078096
	4.65886858e+000,1.57028475e+001,8.99518681e-001,6.59463183e-002,
	3.04416947e+000,5.73552506e-001,2.31467419e-001,1.02515786e-001,
	1.14380181e-002,1.96255632e-002,2.89768099e-001,4.22860197e-001,

	// Z= 92,  chisq=    0.078033
	3.65205160e+000,1.05218997e+000,1.65106214e+000,1.33527438e-001,
	5.29454481e+000,1.75161565e+002,3.25398904e+000,2.87670052e+001,
	6.12841265e-002,1.80787924e-002,1.97275637e-001,8.08635057e-002,

	// Z= 93,  chisq=    0.078635
	3.02213585e+000,2.86453778e-001,1.58398572e-001,4.56283440e-002,
	5.59079088e+000,9.57525300e+000,1.96414795e-001,1.08847989e-001,
	5.38463183e-001,4.71094575e-001,3.70062800e+000,2.01872367e+001,

	// Z= 94,  chisq=    0.069821
	3.29502623e+000,3.14726727e-001,5.45943556e+000,1.02049081e+001,
	1.17881479e-001,3.93194841e-002,1.84735640e-001,1.04096448e-001,
	3.54674626e+000,2.20001166e+001,4.93156014e-001,4.61773334e-001,

	// Z= 95,  chisq=    0.060176
	1.29298016e-001,4.12607516e-002,3.45457005e+000,3.38325985e-001,
	5.38891000e+000,1.07432214e+001,4.68224590e-001,4.49999819e-001,
	3.37879497e+000,2.12295175e+001,1.75693071e-001,9.97603743e-002,

	// Z= 96,  chisq=    0.049793
	3.50201438e+000,3.58615471e-001,1.94608827e-001,5.17453514e-002,
	5.37581201e+000,1.11694335e+001,4.59848229e-001,4.35152516e-001,
	3.22100697e+000,1.81542878e+001,1.68993556e-001,9.58374197e-002,

	// Z= 97,  chisq=    0.043341
	3.65558776e+000,3.87348695e-001,2.17237221e-001,5.42089667e-002,
	5.30013734e+000,1.17669293e+001,4.40344108e-001,4.25932714e-001,
	3.08097579e+000,1.74939824e+001,1.60867961e-001,9.19308953e-002,

	// Z= 98,  chisq=    0.039834
	1.71517326e-001,4.76968759e-002,5.17973108e+000,1.25297385e+001,
	3.92324407e+000,4.16721851e-001,4.14210157e-001,4.22071054e-001,
	1.51883338e-001,8.80433509e-002,2.87382527e+000,1.91828893e+001,

	// Z= 99,  chisq=    0.035361
	4.07619703e+000,4.43772820e-001,1.84972341e-001,4.98301091e-002,
	5.11409422e+000,1.31822238e+001,4.01208319e-001,4.14206858e-001,
	1.44976722e-001,8.45158446e-002,2.71895770e+000,1.85660070e+001,

	// Z=100,  chisq=    0.031998
	4.23654911e+000,4.75539940e-001,5.03977186e+000,1.38945832e+001,
	1.99923109e-001,5.17835117e-002,1.38282334e-001,8.11467887e-002,
	2.57522628e+000,1.80874036e+001,3.92285044e-001,4.08933824e-001,

	// Z=101,  chisq=    0.029108
	2.13083495e-001,5.39430856e-002,4.38509996e+000,5.03937492e-001,
	4.97907797e+000,1.46085669e+001,1.32282499e-001,7.79562577e-002,
	3.84290027e-001,4.02258179e-001,2.43285905e+000,1.75305589e+001,

	// Z=102,  chisq=    0.027453
	2.20250187e-001,5.50649651e-002,4.90512985e+000,1.54143379e+001,
	4.55343811e+000,5.36738291e-001,1.26340570e-001,7.48679930e-002,
	3.79012844e-001,3.98595967e-001,2.33435639e+000,1.72207566e+001,

	// Z=103,  chisq=    0.024574
	3.14776987e-001,6.53484929e-002,4.87469941e+000,1.60770115e+001,
	4.58964523e+000,5.74426564e-001,2.29463967e+000,1.44955175e+001,
	3.77775198e-001,3.87999262e-001,1.21868192e-001,7.21097014e-002
};