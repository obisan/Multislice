// MRC.cpp: определяет экспортированные функции для приложения DLL.
// 
// 
// #include "stdafx.h"
// #include "MRC.h"
// 
// 
// Пример экспортированной переменной
// MRC_API int nMRC=0;
// 
// Пример экспортированной функции.
// MRC_API int fnMRC(void)
// {
// 	return 42;
// }
// 
// Конструктор для экспортированного класса.
// см. определение класса в MRC.h
// CMRC::CMRC()
// {
// 	return;
// }

#include "stdafx.h"
#include "MRC.h"
#include "memalloc.h"

namespace ImageSpace {
	MRC::MRC(void) {
		int nx = 0;
		int ny = 0;
		int nz = 0;

		int mode = 4;

		int nxstart = 0;
		int nystart = 0;
		int nzstart = 0;

		int mx = 512;
		int my = 512;
		int mz = 4;

		float xlen = 10.0f;
		float ylen = 10.0f;
		float zlen = 10.0f;

		float alpha = 90.0f;
		float beta = 90.0f;
		float gamma = 90.0f;

		int mapc = 1;
		int mapr = 2;
		int maps = 3;

		float amin = 1.0f;
		float amax = 3.0f;
		float amean = 2.0f;

		int ispq = 0;
		int next = 0;

		short creatid = 0;
		char extra_data[30]; memset(extra_data, 0, 30 * sizeof(char));

		short nint = 0;
		short nreal = 0;

		char extra_data_2[20];	memset(extra_data_2, 0, 20 * sizeof(char));

		int imodStamp = 0;
		int imodFlag = 0;

		short idtype = 1;
		short lens = 0;
		short nd1 = 0;
		short nd2 = 0;
		short vd1 = 0;
		short vd2 = 0;

		float tiltangles[6];
		tiltangles[0] = 2.35098870164457501593747307444E-38;
		tiltangles[1] = 0;
		tiltangles[2] = 0;
		tiltangles[3] = 0;
		tiltangles[4] = 0;
		tiltangles[5] = 0;

		float xorg = 0.0f;
		float yorg = 0.0f;
		float zorg = 0.0f;

		char cmap[4]; memset(cmap, 0, 4 * sizeof(char));
		char stamp[4]; memset(stamp, 0, 4 * sizeof(char));

		float rms = 0.0f;

		int nlabl = 0;
		char vlabl[10][80]; memset(vlabl, 0, 10 * 80 * sizeof(char));

		char *pImage = nullptr;

	}

	MRC::MRC(
		const char* filename,
		int nx, int ny, int nz, 
		int mode, 
		int nxstart, int nystart, int nzstart, 
		int mx, int my, int mz,
		float xlen, float ylen, float zlen,
		float alpha, float beta, float gamma,
		int mapc, int mapr, int maps,
		float amin,	float amax,	float amean,
		int ispq, int next,
		short creatid, const char* extra_data,
		short nint,	short nreal, const char* extra_data_2,
		int imodStamp, int imodFlag,
		short idtype, short lens, short nd1, short nd2, short vd1, short vd2,
		const float tiltangles[6],
		float xorg,	float yorg,	float zorg,
		const char* cmap, const char* stamp,
		float rms,
		int nlabl,
		const char* vlabl,
		char* pData
		) {

			this->nx = nx;
			this->ny = ny;
			this->nz = nz;

			this->mode = mode;

			this->nxStart = nxstart;
			this->nyStart = nystart;
			this->nzStart = nzstart;

			this->mx = mx;
			this->my = my;
			this->mz = mz;

			this->xlen = xlen;
			this->ylen = ylen;
			this->zlen = zlen;

			this->alpha = alpha;
			this->beta = beta;
			this->gamma = gamma;

			this->mapc = mapc;
			this->mapr = mapr;
			this->maps = maps;

			this->amin = amin;
			this->amax = amax;
			this->amean = amean;

			this->ispq = ispq;
			this->next = next;

			this->creatid = creatid;
			memcpy(this->extra_data, extra_data, 30 * sizeof(char));

			this->nint = nint;
			this->nreal = nreal;
			memcpy(this->extra_data_2, extra_data_2, 20 * sizeof(char));

			this->imodStamp = imodStamp;
			this->imodFlag = imodFlag;

			this->idtype = idtype;
			this->lens = lens;
			this->nd1 = nd1;
			this->nd2 = nd2;
			this->vd1 = vd1;
			this->vd2 = vd2;

			this->tiltangles[0] = tiltangles[0];
			this->tiltangles[1] = tiltangles[1];
			this->tiltangles[2] = tiltangles[2];
			this->tiltangles[3] = tiltangles[3];
			this->tiltangles[4] = tiltangles[4];
			this->tiltangles[5] = tiltangles[5];

			this->xorg = xorg;
			this->yorg = yorg;
			this->zorg = zorg;

			memcpy(this->cmap, cmap, 4 * sizeof(char));
			memcpy(this->stamp, stamp, 4 * sizeof(char));

			this->rms = rms;

			this->nlabl = nlabl;
			memcpy(this->vlabl, vlabl, 10 * 80 * sizeof(char));

			this->pData = pData;
	}

	MRC::~MRC(void) {
		if(pData != nullptr) 
			pData = nullptr;
	}

	int MRC::save(const char* filename) {
		char *filenamesave_ext = new char[ strlen(filename) + 8 ];
		strcpy(filenamesave_ext, filename);
		strcat(filenamesave_ext,".mrc");

		std::ofstream fout(filenamesave_ext, std::ios::binary);

		fout.write(reinterpret_cast<const char*> (&nx), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&ny), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&nz), sizeof(int));

		fout.write(reinterpret_cast<const char*> (&mode), sizeof(int));

		fout.write(reinterpret_cast<const char*> (&nxStart), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&nyStart), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&nzStart), sizeof(int));

		fout.write(reinterpret_cast<const char*> (&mx), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&my), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&mz), sizeof(int));

		fout.write(reinterpret_cast<const char*> (&xlen), sizeof(float));
		fout.write(reinterpret_cast<const char*> (&ylen), sizeof(float));
		fout.write(reinterpret_cast<const char*> (&zlen), sizeof(float));

		fout.write(reinterpret_cast<const char*> (&alpha), sizeof(float));
		fout.write(reinterpret_cast<const char*> (&beta), sizeof(float));
		fout.write(reinterpret_cast<const char*> (&gamma), sizeof(float));

		fout.write(reinterpret_cast<const char*> (&mapc), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&mapr), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&maps), sizeof(int));

		fout.write(reinterpret_cast<const char*> (&amin), sizeof(float));
		fout.write(reinterpret_cast<const char*> (&amax), sizeof(float));
		fout.write(reinterpret_cast<const char*> (&amean), sizeof(float));

		fout.write(reinterpret_cast<const char*> (&ispq), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&next), sizeof(int));

		fout.write(reinterpret_cast<const char*> (&creatid), sizeof(short));
		fout.write(reinterpret_cast<const char*> (&extra_data), 30 * sizeof(char)); // !!! char 30

		fout.write(reinterpret_cast<const char*> (&nint), sizeof(short));
		fout.write(reinterpret_cast<const char*> (&nreal), sizeof(short));

		fout.write(reinterpret_cast<const char*> (&extra_data_2), 20 * sizeof(char)); // !!! char 20

		fout.write(reinterpret_cast<const char*> (&imodStamp), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&imodFlag), sizeof(int));

		fout.write(reinterpret_cast<const char*> (&idtype), sizeof(short));
		fout.write(reinterpret_cast<const char*> (&lens), sizeof(short));
		fout.write(reinterpret_cast<const char*> (&nd1), sizeof(short));
		fout.write(reinterpret_cast<const char*> (&nd2), sizeof(short));
		fout.write(reinterpret_cast<const char*> (&vd1), sizeof(short));
		fout.write(reinterpret_cast<const char*> (&vd2), sizeof(short));

		fout.write(reinterpret_cast<const char*> (&tiltangles), 6 * sizeof(float));

		fout.write(reinterpret_cast<const char*> (&xorg), sizeof(float));
		fout.write(reinterpret_cast<const char*> (&yorg), sizeof(float));
		fout.write(reinterpret_cast<const char*> (&zorg), sizeof(float));

		fout.write(reinterpret_cast<const char*> (&cmap), 4 * sizeof(char));
		fout.write(reinterpret_cast<const char*> (&stamp), 4 * sizeof(char));

		fout.write(reinterpret_cast<const char*> (&rms), sizeof(float));

		fout.write(reinterpret_cast<const char*> (&nlabl), sizeof(int));
		fout.write(reinterpret_cast<const char*> (&vlabl), 10 * 80 * sizeof(char));

		int nChannel = nChannels(mode);

		void *oData = nullptr;
				
		switch(mode) {
		case 0:
			oData = (unsigned char*) malloc1D(nx * ny * nz * nChannel, sizeof(char), "Output data unsigned char");
			break;
		case 1:
			oData = (short*) malloc1D(nx * ny * nz * nChannel, sizeof(short), "Output data signed short");
			break;
		case 2:
			oData = (float*) malloc1D(nx * ny * nz * nChannel, sizeof(float), "Output data float");
			break;
		case 3:
			oData = (short*) malloc1D(nx * ny * nz * nChannel, sizeof(short), "Output data short * 2");
			break;
		case 4:
			oData = (float*) malloc1D(nx * ny * nz * nChannel, sizeof(float), "Output data float * 2");
			break;
		case 6:
			oData = (unsigned short*) malloc1D(nx * ny * nz * nChannel, sizeof(short), "Output data unsigned short");
			break;
		case 16:
			oData = (unsigned char*) malloc1D(nx * ny * nz * nChannel, sizeof(char), "Output data unsigned char * 3");
		default:
			oData = nullptr;
			break;
		}

		for(size_t k = 0; k < nz; k++) {
			for(size_t i = 0; i < ny; i++) {
				for(size_t j = 0; j < nx; j++) {
					for(size_t l = 0; l < nChannel; l++) {
						switch(mode) {
						case 0:		// unsigned char
						case 16:
							((unsigned char*) oData)[ nChannel * (nx * ny * k + nx * i + j) + l ] = ( (double*) pData)[ nChannel * (nx * ny * k + nx * i + j) + l ];
							break;
						case 1:		// short
						case 3:
							((short*) oData)[ nChannel * (nx * ny * k + nx * i + j) + l ] = ( (double*) pData)[ nChannel * (nx * ny * k + nx * i + j) + l ];
							break;
						case 2:		// float
						case 4:
							((float*) oData)[ nChannel * (nx * ny * k + nx * i + j) + l ] = ( (double*) pData)[ nChannel * (nx * ny * k + nx * i + j) + l ];
							break;
						case 6:		// unsigned short
							((unsigned short*) oData)[ nChannel * (nx * ny * k + nx * i + j) + l ] = ( (double*) pData)[ nChannel * (nx * ny * k + nx * i + j) + l ];
							break;
						default:
							break;
						}
						
					}
				}
			}
		}

		fout.write(reinterpret_cast<const char*> (oData), this->nx * this->ny * this->nz * sizeof(float) * nChannel);
		delete[] oData;
		
		fout.close();
		
		delete[] filenamesave_ext;

		return 0;
	}

}