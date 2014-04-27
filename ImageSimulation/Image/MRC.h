// Приведенный ниже блок ifdef - это стандартный метод создания макросов, упрощающий процедуру 
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа MRC_EXPORTS,
// в командной строке. Этот символ не должен быть определен в каком-либо проекте
// использующем данную DLL. Благодаря этому любой другой проект, чьи исходные файлы включают данный файл, видит 
// функции MRC_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.
#ifdef IMAGE_EXPORTS
#define IMAGE_API __declspec(dllexport)
#else
#define IMAGE_API __declspec(dllimport)
#endif

// Этот класс экспортирован из MRC.dll
// class MRC_API CMRC {
// public:
// 	CMRC(void);
// 	// TODO: Добавьте здесь свои методы.
// };
// 
// extern MRC_API int nMRC;
// 
// MRC_API int fnMRC(void);


namespace ImageSpace {
	class IMAGE_API MRC {
	public:
		MRC(void);
		MRC(
			const char* filname,
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
			);
		~MRC(void);

		int save(const char* filename);

	private:
		char filename[128];

		int nx;							//	0	0
		int ny;							//	1	4
		int nz;							//	2	8
		int mode;						//	3			0 = uchar, 1 = short, 2 = float, 3 = short*2, 4 = float*2, 6 = unsigned 16-bit integers (non-standard), 16 = unsigned char * 3 (for rgb data, non-standard)
		int nxStart;					//	4			
		int nyStart;					//	5			
		int nzStart;					//	6				
		int mx;							//	7			
		int my;							//  8			
		int mz;							//  9			
		float xlen;						// 10	40		cell dimensions in A
		float ylen;						// 11				
		float zlen;						// 12			
		float alpha;					// 13			cell angles in degrees
		float beta;						// 14			
		float gamma;					// 15			
		int mapc;						// 16			column axis
		int mapr;						// 17			row axis
		int maps;						// 18			section axis
		float amin;						// 19			minimum density value
		float amax;						// 20	80		maximum density value
		float amean;					// 22			average density value
		int ispq;						// 23			space group number
		int next;						// 24			
		short creatid;					//				
		char extra_data[30];			//				
		short nint;						//		128		
		short nreal;					//				
		char extra_data_2[20];			//				
		int imodStamp;					//				
		int imodFlag;					//				
		short idtype;					//		160		
		short lens;						//				
		short nd1;						//				
		short nd2;						//				
		short vd1;						//				
		short vd2;						//				
		float tiltangles[6];			//					
		float xorg;						//		196		phase origin in pixels FIXME: is in pixels or [L] units?
		float yorg;						//				
		float zorg;						//				
		char cmap[4];					//				
		char stamp[4];					//				
		float rms;						//				
		int nlabl;						//				number of labels used
		char vlabl[10][80];				//		224		10 80-character labels

		char *pData;					//		1024	image
		
		int nChannels(int mode) {
			if( (mode == 0) || (mode == 1) || (mode == 2) || (mode == 6) ) return 1;
			if( (mode == 3) || (mode == 4) ) return 2;
			if( (mode == 16) ) return 3;
			return 1;
		};
	};
}