#ifndef TEM_GL_KERNEL_CU_
#define TEM_GL_KERNEL_CU_
#define uint unsigned int
#ifdef __DEVICE_EMULATION__
#define EMUSYNC __syncthreads()
#else
#define EMUSYNC
#endif
//#include <curand_kernel.h>
int * d_seed;
		unsigned int ImageOrCTF=1;
		unsigned int RealCTFOrComplexCTF=0;
		int seed;
		int NoiseLevel=20;
template<class T>
struct SharedMemory
{
    __device__ inline operator       T*()
    {
        extern __shared__ int __smem[];
        return (T*)__smem;
    }

    __device__ inline operator const T*() const
    {
        extern __shared__ int __smem[];
        return (T*)__smem;
    }
};

__device__ inline float RNG()
{   
    unsigned int m_w = threadIdx.x;
    unsigned int m_z = threadIdx.y;

    for(int i=0; i < 100; i++)
    {
        m_z = 36969 * (m_z & 65535) + (m_z >> 16);
        m_w = 18000 * (m_w & 65535) + (m_w >> 16);
}
       // cout <<(m_z << 16) + m_w << endl;  /* 32-bit result */
		return (float)(((m_z << 16) + m_w)%1000)/1000.0f ;
    //}
}
__device__ inline float RNG11(int &seed){
double const a = 16807; //ie 7**5
double const m = 2147483647; //ie 2**31-1
double const reciprocal_m = 1.0/m;
double temp = seed * a;
temp = (temp - m * floor(temp * reciprocal_m));
seed=(int)temp;
return temp;
}
int intrnd (int& seed) // 1<=seed<m
{
int const a = 16807; //ie 7**5
int const m = 2147483647; //ie 2**31-1
seed = (long(seed * a))%m;
return seed;
}
__device__ uint rgbaFloatToInt(float rgba)
{
    //rgba = __saturatef(rgba);   // clamp to [0.0, 1.0]
    //rgba.y = __saturatef(rgba.y);
    //rgba.z = __saturatef(rgba.z);
    //rgba.w = __saturatef(rgba.w);
    return (uint(rgba*255)<<24) | (uint(rgba*255)<<16) | (uint(rgba*255)<<8) | uint(rgba*255);
}
__device__ uint rgbaFloatToIntcolor(float rgba)
{
    //rgba = __saturatef(rgba);   // clamp to [0.0, 1.0]
    //rgba.y = __saturatef(rgba.y);
    //rgba.z = __saturatef(rgba.z);
    //rgba.w = __saturatef(rgba.w);
	uint color=0;
	//rgba > 0.0f ? color=(uint(0*255)<<24) | (uint(rgba*255)<<16) | (uint(0*255)<<8) | uint(0*255) : color=(uint(0*255)<<24) | (uint(0*255)<<16) | (uint(0*255)<<8) | uint(rgba*255);
	if (rgba >0.0f) {
		color=(uint(0*255)<<24) | (uint(rgba*255)<<16) | (uint(0*255)<<8) | uint(0*255);
		} else {
			color=(uint(0*255)<<24) | (uint(0*255)<<16) | (uint(0*255)<<8) | uint(-rgba*255);
		}
    return color;
}
// convert from 32-bit int to float
__device__ float rgbaIntToFloat(uint c)
{
    float rgba;
    rgba = (c & 0xff) / 255.0f;
    //rgba.y = ((c>>8) & 0xff) / 255.0f;
    //rgba.z = ((c>>16) & 0xff) / 255.0f;
    //rgba.w = ((c>>24) & 0xff) / 255.0f;
    return rgba;
}
__global__ void CreateRGBAfromFloat(uint *d_result,float *d_img, int dimx,int dimy){
	
   unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
   unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	if((xIndex < dimx) && (yIndex < dimy)){
			int index  = xIndex + dimx*yIndex;
		d_result[index]=rgbaFloatToInt(d_img[index]);

		}

	}
__global__ void CreateRGBAfromComplex(uint *d_result,cufftComplex *d_img, int dimx,int dimy,float imin, float imax, unsigned int ImageorCTF,unsigned int RealCTFOrComplexCTF,unsigned int Zoom){
	
   unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
   unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	if((xIndex < dimx/2) && (yIndex < dimy/2)){
			//int index  = xIndex + dimx*yIndex;
			int index  = xIndex*2 + dimx*yIndex*2;
		//d_result[index]=rgbaFloatToInt(d_img[index].x);
		//d_result[index]=1010526;
			//if (ImageorCTF==1) {
			//		d_result[index]=rgbaFloatToInt(__fdividef((sqrtf(d_img[index].x*d_img[index].x+d_img[index].y*d_img[index].y)-imin),(imax-imin)));
			//	} else {
					//d_result[index]=RealCTFOrComplexCTF==0?rgbaFloatToIntcolor(d_img[index].y):rgbaFloatToIntcolor(d_img[index].x);

					d_result[index]=RealCTFOrComplexCTF==0?rgbaFloatToIntcolor(__fdividef((d_img[index].y),(imax-imin))):rgbaFloatToIntcolor(__fdividef((d_img[index].x),(imax-imin)));

			//	}

		}

	}
///
__global__ void CreateRGBAfromTexture(uint *d_result, int dimx,int dimy,float imin, float imax, unsigned int ImageorCTF,unsigned int RealCTFOrComplexCTF,float Zoom,float cx,float cy,float tx,float ty){
	
   unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
   unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	if((xIndex < dimx) && (yIndex < dimy)){
			int index  = xIndex + dimx*yIndex;
			float x,y;
			x=(float)xIndex;//*__cosf(10.0f/180.0f*PI)-(float)yIndex*__sinf(10.0f/180.0f*PI);
			y=(float)yIndex;//*__cosf(10.0f/180.0f*PI)+(float)xIndex*__sinf(10.0f/180.0f*PI);
			//int index  = xIndex/Zoom + dimx*yIndex/Zoom;
		//d_result[index]=rgbaFloatToInt(d_img[index].x);
		//d_result[index]=1010526;
			x=(x-cx)/Zoom+cx+tx;
			y=(y-cy)/Zoom+cy+ty;
			while (x<=0.0f)
				x+=(float)dimx;
			while (y<=0.0f)
				y+=(float)dimy;
			while (x>=(float)dimx)
				x-=(float)dimx;
			while (y>=(float)dimy)
				y-=(float)dimy;

			if (ImageorCTF==1) {
			d_result[index]=rgbaFloatToInt(__fdividef((tex2D(tex_image_2d,x+0.5f,y+0.5f)-imin),(imax-imin)));
				} else {
					//d_result[index]=RealCTFOrComplexCTF==0?rgbaFloatToIntcolor(d_img[index].y):rgbaFloatToIntcolor(d_img[index].x);
					//d_result[index]=RealCTFOrComplexCTF==0?rgbaFloatToIntcolor(__fdividef((d_img[index].y),(imax-imin))):rgbaFloatToIntcolor(__fdividef((d_img[index].x),(imax-imin)));
					//d_result[index]=RealCTFOrComplexCTF==0?rgbaFloatToIntcolor(__fdividef(tex2D(tex_image_2d,x+0.5f,y+0.5f),(imax-imin))):rgbaFloatToIntcolor(__fdividef(tex2D(tex_image_2d,x+0.5f,y+0.5f),(imax-imin)));
					d_result[index]=rgbaFloatToIntcolor(__fdividef(tex2D(tex_image_2d,x+0.5f,y+0.5f),(imax-imin)));


				}

		}

	}
///
///
__global__ void CreateRGBAfromTextureNoise(uint *d_result, int dimx,int dimy,float imin, float imax, unsigned int ImageorCTF,unsigned int RealCTFOrComplexCTF,float Zoom,float cx,float cy,float tx,float ty){
	
   unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
   unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	if((xIndex < dimx) && (yIndex < dimy)){
			int index  = xIndex + dimx*yIndex;
			float x,y;
			x=(float)xIndex;//*__cosf(10.0f/180.0f*PI)-(float)yIndex*__sinf(10.0f/180.0f*PI);
			y=(float)yIndex;//*__cosf(10.0f/180.0f*PI)+(float)xIndex*__sinf(10.0f/180.0f*PI);
			//int index  = xIndex/Zoom + dimx*yIndex/Zoom;
		//d_result[index]=rgbaFloatToInt(d_img[index].x);
		//d_result[index]=1010526;
			x=(x-cx)/Zoom+cx+tx;
			y=(y-cy)/Zoom+cy+ty;
			while (x<=0.0f)
				x+=(float)dimx;
			while (y<=0.0f)
				y+=(float)dimy;
			while (x>=(float)dimx)
				x-=(float)dimx;
			while (y>=(float)dimy)
				y-=(float)dimy;

			if (ImageorCTF==1) {
			d_result[index]+=rgbaFloatToInt(__fdividef((tex2D(tex_image_2d,x+0.5f,y+0.5f)-imin),(imax-imin)));
				} else {
					//d_result[index]=RealCTFOrComplexCTF==0?rgbaFloatToIntcolor(d_img[index].y):rgbaFloatToIntcolor(d_img[index].x);
					//d_result[index]=RealCTFOrComplexCTF==0?rgbaFloatToIntcolor(__fdividef((d_img[index].y),(imax-imin))):rgbaFloatToIntcolor(__fdividef((d_img[index].x),(imax-imin)));
					//d_result[index]=RealCTFOrComplexCTF==0?rgbaFloatToIntcolor(__fdividef(tex2D(tex_image_2d,x+0.5f,y+0.5f),(imax-imin))):rgbaFloatToIntcolor(__fdividef(tex2D(tex_image_2d,x+0.5f,y+0.5f),(imax-imin)));
					d_result[index]+=rgbaFloatToIntcolor(__fdividef(tex2D(tex_image_2d,x+0.5f,y+0.5f),(imax-imin)));


				}

		}

	}
///
__global__ void CreateFloatfromRGBA(float *d_result,uint *d_img, int dimx,int dimy){
	
   unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
   unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	if((xIndex < dimx) && (yIndex < dimy)){
			int index  = xIndex + dimx*yIndex;
		d_result[index]=rgbaIntToFloat(d_img[index]);

		}

	}
__global__ void CopyComplexToFloat(float *d_result,cufftComplex *d_input,int dimx,int dimy,uint ImageOrCTF,uint RealCTFOrComplexCTF){
   unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
   unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	if((xIndex < dimx) && (yIndex < dimy)){
			int index  = xIndex + dimx*yIndex;
			if (ImageOrCTF==1){
				//d_result[index]=RNG11(seed);
				d_result[index]=sqrtf(d_input[index].x*d_input[index].x+d_input[index].y*d_input[index].y);
				} else {
					if (RealCTFOrComplexCTF==1){
				
						d_result[index]=d_input[index].x;

						}else{
						d_result[index]=d_input[index].y;

						}
				}

		}
	}
//__global__ void CopyComplexToFloatNoise(float *d_result,cufftComplex *d_input,int dimx,int dimy,uint ImageOrCTF,uint RealCTFOrComplexCTF){
__global__ void 	ImageAdd(cufftComplex *d_img_out,cufftComplex *d_img_in, int dimx, int dimy){

   unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
   unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	if((xIndex < dimx) && (yIndex < dimy)){
			int index  = xIndex + dimx*yIndex;
				d_img_out[index]=cuCaddf(d_img_out[index],d_img_in[index]);
				__syncthreads();
		}
	}
__global__ void AppertureAplay(cufftComplex *d_img,int dimx, int dimy,float ux, float uy, float AppertureRadius, float Ax, float Ay) {
    unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
    unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	if((xIndex < dimx) && (yIndex < dimy)){
 		float kx=((int)xIndex-dimx/2)*ux;
		float ky=((int)yIndex-dimy/2)*uy;
		float rAx=Ax*ux;
		float rAy=Ay*uy;
		float Dr2=(rAx-kx)*(rAx-kx)+(rAy-ky)*(rAy-ky);
		if ((AppertureRadius*AppertureRadius<Dr2)){
			int index  = xIndex + dimx*yIndex;
			d_img[index].x=0.0f;
			d_img[index].y=0.0f;
			} 

		}
	}
__global__ void CTF(cufftComplex *d_img,int dimx, int dimy,float Cs, float Lambda, float Df,float Delta,float ux, float uy,unsigned int ImageOrCTF,float H_Cc,float q0)
	{
    unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
    unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
   float Alpha,G0,G1,G2,G3,G4;//,Ar2;
   cuFloatComplex CTFF,Esct;
	if((xIndex < dimx) && (yIndex < dimy)){
		//float k2=__powf(((int)xIndex-dimx/2)*ux,2.0f)+__powf(((int)yIndex-dimy/2)*ux,2.0f);
		//int kx=;
		//int ky=;
		//float rAx=Ax*ux;
		//float rAy=Ay*uy;
		float kx=((int)xIndex-dimx/2)*ux;
		float ky=((int)yIndex-dimy/2)*uy;
		//float Dr2=(rAx-kx)*(rAx-kx)+(rAy-ky)*(rAy-ky);
		float k2=kx*kx+ky*ky;
		int index  = xIndex + dimx*yIndex;

		//if ((Apperture==1) && (AppertureRadius*AppertureRadius<Dr2)){
		//		d_img[index].x=0.0f;
		//		d_img[index].y=0.0f;
		//	} else {

			G0=PI*Df*Lambda ;
			G1=PI*0.5f*Cs*__powf(Lambda,3);
			G2=-0.5f*__powf(PI*Lambda*Delta,2) ;
			G3=q0*2.0*G1 ;
			G4=q0*G0 ;
			//if (Apperture==1)
			//	Ar2=AppertureRadius*AppertureRadius;
			//Alpha=PI*k2*(0.5f*Cs*__powf(Lambda,3.0f)*k2+Lambda*Df);
			//return(Math.Cos(G0*s+G1*u)*Math.Exp(G2*u-Math.Pow(G3*s*v+G4*v,2))) ;
		Alpha=(G0+G1*k2)*k2;
		//CTFS=+0.0f;//-__sinf(Alpha);
		//CTF.real=-__sinf(Alpha);
		CTFF.x=__cosf(Alpha);
		CTFF.y=-__sinf(Alpha);
		//CTFC=1.0f;//__cosf(Alpha);
		//Ec=__expf(-__powf(1.1335f*Lambda*k2*H_Cc,2.0f));
		//Esct=__expf(k2*(G2*k2-__powf(G3*k2+G4,2)));
		Esct.x=__expf(k2*(G2*k2-(G3*k2+G4)*(G3*k2+G4)));
		Esct.y=0.0f;
		//if (k2>__powf((float)min(dimx/2*ux,dimy/2*ux),2.0f)) {
		//	d_img[index].y=0.0f;
		//	d_img[index].x=0.0f;

		//	} else {
			//d_img[index].y=k2;//0.0f;
			//d_img[index].x=k2;//0.0f;

		if (ImageOrCTF==0) {
			//tmp=cuCmulf(CTF,Esct);
			//d_img[index].y=tmp.x;
			//d_img[index].x=tmp.y;
			 d_img[index]=cuCmulf(CTFF,Esct);
			//d_img[index]=(cufftComplex)((complex<float>)CTF*Esct);
			} else {
			 //d_img[index].x= (d_img[index].x*CTFC -	d_img[index].y*CTFS)*Esct;
			 //d_img[index].y= (d_img[index].y*CTFC + d_img[index].x*CTFS)*Esct;
				//d_img[index]=d_img[index]*CTF*Esct;
				//tmp=d_img[index].x,d_img[index].y);
				//tmp=tmp*CTF*Esct;
			//d_img[index].y=tmp.imag();
			//d_img[index].x=tmp.real();
			  d_img[index]=cuCmulf(d_img[index],cuCmulf(CTFF,Esct));

			}
		//}
		/*if (k2>__powf((float)min(dimx/2*ux,dimy/2*ux),2.0f)) {
			d_img[index].y=0.0f;
			d_img[index].x=0.0f;
			}*/
			}
		__syncthreads();
	//	}
	}

//
//
__global__ void
max6(unsigned int blockSize,float *g_idata, float *g_odata, unsigned int n)
{
    volatile float *sdata = SharedMemory<float>(); //объявляем разделяемую память

	// первый шаг поиска максимума читаем из глобальнйо памяти и пишем в разделяемую память
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockSize*2 + threadIdx.x;
    unsigned int gridSize = blockSize*2*gridDim.x;
    
    float mySum = -1e+30f;

	// ищем максимальный элемент в потоке, число элементов определяется количеством активных блоков (gridDim).
	// большее количество блоков приводит к меньшему числу элементов на поток
    while (i < n)
    {         
        mySum = fmaxf(g_idata[i],mySum);
        // ищем максимум среди элементов с индексом blockIdx.x*blockSize*2 + threadIdx.x и	blockIdx.x*blockSize*2 + threadIdx.x+  blockSize
		// с шагом	  gridSize
        if ( i + blockSize < n) 
            mySum = fmaxf(g_idata[i+blockSize],mySum);  
        i += gridSize;
    } 

    // каждый максимум из потока пишем в разделяемую память 
    sdata[tid] = mySum;
    __syncthreads();// ждем пока все потоки не закончат вычисления


    // ищем максимум в размедяемой памяти
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] = mySum = fmaxf(mySum,sdata[tid + 256]); } __syncthreads(); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] = mySum = fmaxf(mySum,sdata[tid + 128]); } __syncthreads(); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] = mySum = fmaxf(mySum,sdata[tid +  64]); } __syncthreads(); }
    
#ifndef __DEVICE_EMULATION__ // исполняется только в случае реального устройства, а не его эмуляции
    if (tid < 32)
#endif
    {
        if (blockSize >=  64) { sdata[tid] = mySum = fmaxf(mySum, sdata[tid + 32]); EMUSYNC; }
        if (blockSize >=  32) { sdata[tid] = mySum = fmaxf(mySum , sdata[tid + 16]); EMUSYNC; }
        if (blockSize >=  16) { sdata[tid] = mySum = fmaxf(mySum , sdata[tid +  8]); EMUSYNC; }
        if (blockSize >=   8) { sdata[tid] = mySum = fmaxf(mySum , sdata[tid +  4]); EMUSYNC; }
        if (blockSize >=   4) { sdata[tid] = mySum = fmaxf(mySum , sdata[tid +  2]); EMUSYNC; }
        if (blockSize >=   2) { sdata[tid] = mySum = fmaxf(mySum , sdata[tid +  1]); EMUSYNC; }
    }
    
    // пишем результат в глобальную память с индексом = номеру блока 
    if (tid == 0) 
        g_odata[blockIdx.x] = sdata[0];
}
//
__global__ void
max61(unsigned int blockSize,cufftComplex *g_idata, float *g_odata, unsigned int n,unsigned int ImageOrCTF,unsigned int RealCTFOrComplexCTF)
{
    // now that we are using warp-synchronous programming (below)
    // we need to declare our shared memory volatile so that the compiler
    // doesn't reorder stores to it and induce incorrect behavior.
    volatile float *sdata = SharedMemory<float>();

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockSize*2 + threadIdx.x;
    unsigned int gridSize = blockSize*2*gridDim.x;
    
    float mySum = -1e+30f;

    // we reduce multiple elements per thread.  The number is determined by the 
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {         
        //mySum = max(__fsqrt_rz(g_idata[i].x*g_idata[i].x+g_idata[i].y*g_idata[i].y),mySum);
        mySum = fmaxf(ImageOrCTF==1?__fsqrt_rz(g_idata[i].x*g_idata[i].x+g_idata[i].y*g_idata[i].y):RealCTFOrComplexCTF==0?g_idata[i].y:g_idata[i].x,mySum);

        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if ( i + blockSize < n) 
            //mySum = max(g_idata[i+blockSize],mySum);  
			mySum = fmaxf(ImageOrCTF==1?__fsqrt_rz(g_idata[i+blockSize].x*g_idata[i+blockSize].x+g_idata[i+blockSize].y*g_idata[i+blockSize].y):RealCTFOrComplexCTF==0?g_idata[i+blockSize].y:g_idata[i+blockSize].x,mySum);
            //mySum = max(__fsqrt_rz(g_idata[i+blockSize].x*g_idata[i+blockSize].x+g_idata[i+blockSize].y*g_idata[i+blockSize].y),mySum);  
        i += gridSize;
    } 

    // each thread puts its local sum into shared memory 
    sdata[tid] = mySum;
    __syncthreads();


    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] = mySum = max(mySum,sdata[tid + 256]); } __syncthreads(); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] = mySum = max(mySum,sdata[tid + 128]); } __syncthreads(); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] = mySum = max(mySum,sdata[tid +  64]); } __syncthreads(); }
    
#ifndef __DEVICE_EMULATION__
    if (tid < 32)
#endif
    {
        if (blockSize >=  64) { sdata[tid] = mySum = max(mySum, sdata[tid + 32]); EMUSYNC; }
        if (blockSize >=  32) { sdata[tid] = mySum = max(mySum , sdata[tid + 16]); EMUSYNC; }
        if (blockSize >=  16) { sdata[tid] = mySum = max(mySum , sdata[tid +  8]); EMUSYNC; }
        if (blockSize >=   8) { sdata[tid] = mySum = max(mySum , sdata[tid +  4]); EMUSYNC; }
        if (blockSize >=   4) { sdata[tid] = mySum = max(mySum , sdata[tid +  2]); EMUSYNC; }
        if (blockSize >=   2) { sdata[tid] = mySum = max(mySum , sdata[tid +  1]); EMUSYNC; }
    }
    
    // write result for this block to global mem 
    if (tid == 0) 
        g_odata[blockIdx.x] = sdata[0];
}
//
__global__ void
min6(unsigned int blockSize,float *g_idata, float *g_odata, unsigned int n)
{
    // now that we are using warp-synchronous programming (below)
    // we need to declare our shared memory volatile so that the compiler
    // doesn't reorder stores to it and induce incorrect behavior.
    volatile float *sdata = SharedMemory<float>();

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockSize*2 + threadIdx.x;
    unsigned int gridSize = blockSize*2*gridDim.x;
    
    float mySum = +1e+30f;

    // we reduce multiple elements per thread.  The number is determined by the 
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {         
        mySum = fminf(g_idata[i],mySum);
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if ( i + blockSize < n) 
            mySum = fminf(g_idata[i+blockSize],mySum);  
        i += gridSize;
    } 

    // each thread puts its local sum into shared memory 
    sdata[tid] = mySum;
    __syncthreads();


    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] = mySum = fminf(mySum,sdata[tid + 256]); } __syncthreads(); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] = mySum = fminf(mySum,sdata[tid + 128]); } __syncthreads(); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] = mySum = fminf(mySum,sdata[tid +  64]); } __syncthreads(); }
    
#ifndef __DEVICE_EMULATION__
    if (tid < 32)
#endif
    {
        if (blockSize >=  64) { sdata[tid] = mySum = fminf(mySum, sdata[tid + 32]); EMUSYNC; }
        if (blockSize >=  32) { sdata[tid] = mySum = fminf(mySum , sdata[tid + 16]); EMUSYNC; }
        if (blockSize >=  16) { sdata[tid] = mySum = fminf(mySum , sdata[tid +  8]); EMUSYNC; }
        if (blockSize >=   8) { sdata[tid] = mySum = fminf(mySum , sdata[tid +  4]); EMUSYNC; }
        if (blockSize >=   4) { sdata[tid] = mySum = fminf(mySum , sdata[tid +  2]); EMUSYNC; }
        if (blockSize >=   2) { sdata[tid] = mySum = fminf(mySum , sdata[tid +  1]); EMUSYNC; }
    }
    
    // write result for this block to global mem 
    if (tid == 0) 
        g_odata[blockIdx.x] = sdata[0];
}
//
//
__global__ void
min61(unsigned int blockSize,cufftComplex *g_idata, float *g_odata, unsigned int n,unsigned int ImageOrCTF,unsigned int RealCTFOrComplexCTF)
{
    // now that we are using warp-synchronous programming (below)
    // we need to declare our shared memory volatile so that the compiler
    // doesn't reorder stores to it and induce incorrect behavior.
    volatile float *sdata = SharedMemory<float>();

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockSize*2 + threadIdx.x;
    unsigned int gridSize = blockSize*2*gridDim.x;
    
    float mySum = +1e+30f;

    // we reduce multiple elements per thread.  The number is determined by the 
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {         
       //mySum = fminf(__fsqrt_rz(g_idata[i+blockSize].x*g_idata[i+blockSize].x+g_idata[i+blockSize].y*g_idata[i+blockSize].y),mySum);  

        mySum = fminf(ImageOrCTF==1?__fsqrt_rz(g_idata[i].x*g_idata[i].x+g_idata[i].y*g_idata[i].y):RealCTFOrComplexCTF==0?g_idata[i].y:g_idata[i].x,mySum);
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if ( i + blockSize < n) 
            //mySum = max(g_idata[i+blockSize],mySum);  
        mySum = fminf(ImageOrCTF==1?__fsqrt_rz(g_idata[i+blockSize].x*g_idata[i+blockSize].x+g_idata[i+blockSize].y*g_idata[i+blockSize].y):RealCTFOrComplexCTF==0?g_idata[i+blockSize].y:g_idata[i+blockSize].x,mySum);
            //mySum = fminf(__fsqrt_rz(g_idata[i+blockSize].x*g_idata[i+blockSize].x+g_idata[i+blockSize].y*g_idata[i+blockSize].y),mySum);  
        i += gridSize;
    } 

    // each thread puts its local sum into shared memory 
    sdata[tid] = mySum;
    __syncthreads();


    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] = mySum = fminf(mySum,sdata[tid + 256]); } __syncthreads(); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] = mySum = fminf(mySum,sdata[tid + 128]); } __syncthreads(); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] = mySum = fminf(mySum,sdata[tid +  64]); } __syncthreads(); }
    
#ifndef __DEVICE_EMULATION__
    if (tid < 32)
#endif
    {
        if (blockSize >=  64) { sdata[tid] = mySum = fminf(mySum, sdata[tid + 32]); EMUSYNC; }
        if (blockSize >=  32) { sdata[tid] = mySum = fminf(mySum , sdata[tid + 16]); EMUSYNC; }
        if (blockSize >=  16) { sdata[tid] = mySum = fminf(mySum , sdata[tid +  8]); EMUSYNC; }
        if (blockSize >=   8) { sdata[tid] = mySum = fminf(mySum , sdata[tid +  4]); EMUSYNC; }
        if (blockSize >=   4) { sdata[tid] = mySum = fminf(mySum , sdata[tid +  2]); EMUSYNC; }
        if (blockSize >=   2) { sdata[tid] = mySum = fminf(mySum , sdata[tid +  1]); EMUSYNC; }
    }
    
    // write result for this block to global mem 
    if (tid == 0) 
        g_odata[blockIdx.x] = sdata[0];
}
//
extern "C" void kernel_max(int size, int threads, int blocks, float *d_idata, float *d_odata){
   dim3 dimBlock(threads, 1, 1);
   dim3 dimGrid(blocks, 1, 1);
   int smemSize = (threads <= 32) ? 2 * threads * sizeof(float) : threads * sizeof(float);


   max6<<< dimGrid, dimBlock, smemSize >>>(threads, d_idata, d_odata, size);
   cudaThreadSynchronize();
   cudaError_t err = cudaGetLastError();
   if( cudaSuccess != err) {
	   printf("Kernel execution failed max6 tem_gl_kernel.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)err, cudaGetErrorString( err ));
	   _getch();
	   exit(-1);
   }              
     
	}
extern "C" void kernel_max1(int size, int threads, int blocks, cufftComplex *d_idata, float *d_odata){
   dim3 dimBlock(threads, 1, 1);
   dim3 dimGrid(blocks, 1, 1);
   int smemSize = (threads <= 32) ? 2 * threads * sizeof(float) : threads * sizeof(float);


   max61<<< dimGrid, dimBlock, smemSize >>>(threads, d_idata, d_odata, size,ImageOrCTF,RealCTFOrComplexCTF);
   cudaThreadSynchronize();
    //cutilCheckMsg("Kernel execution failed MAX61 tem_gl_kernel.h!\n"); 
	    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err) {
		printf("Kernel execution failed MAX61 tem_gl_kernel.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)err, cudaGetErrorString( err ));
		_getch();
		exit(-1);
	}
     
	}
//
extern "C" void kernel_min(int size, int threads, int blocks, float *d_idata, float *d_odata){
   dim3 dimBlock(threads, 1, 1);
   dim3 dimGrid(blocks, 1, 1);
   int smemSize = (threads <= 32) ? 2 * threads * sizeof(float) : threads * sizeof(float);


   min6<<< dimGrid, dimBlock, smemSize >>>(threads, d_idata, d_odata, size);
   cudaThreadSynchronize();
   cudaError_t err = cudaGetLastError();
   if( cudaSuccess != err) {
	   printf("Kernel execution failed min6 tem_gl_kernel.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)err, cudaGetErrorString( err ));
	   _getch();
	   exit(-1);
   }       
     
	}
extern "C" void kernel_min1(int size, int threads, int blocks, cufftComplex *d_idata, float *d_odata){
   dim3 dimBlock(threads, 1, 1);
   dim3 dimGrid(blocks, 1, 1);
   int smemSize = (threads <= 32) ? 2 * threads * sizeof(float) : threads * sizeof(float);


   min61<<< dimGrid, dimBlock, smemSize >>>(threads, d_idata, d_odata, size,ImageOrCTF,RealCTFOrComplexCTF);
   cudaThreadSynchronize();
   cudaError_t err = cudaGetLastError();
   if( cudaSuccess != err) {
	   printf("Kernel execution failed min61 tem_gl_kernel.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)err, cudaGetErrorString( err ));
	   _getch();
	   exit(-1);
   }           
     
	}
//
//__global__ void setup_kernel(curandState *state, unsigned long seed,int dimx, int dimy)
//{
//    int id =  (blockDim.x * blockIdx.x + threadIdx.x)*dimx+blockDim.y * blockIdx.y + threadIdx.y;
//	if (id < dimx*dimy)
//		curand_init ( seed, id, 0, &state[id] );
//} 
__global__ void AplaydZ(cufftComplex *d_img, int dimx,int dimy,float Lambda, float ux, float uy,double dZ){
	unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
	unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
	int index  = xIndex + dimx*yIndex;
	cuFloatComplex dZF;
	if((xIndex < dimx) && (yIndex < dimy)){
		//float k2=__powf(((int)xIndex-dimx/2)*ux,2.0f)+__powf(((int)yIndex-dimy/2)*ux,2.0f);
		//int kx=;
		//int ky=;
		//float rAx=Ax*ux;
		//float rAy=Ay*uy;
		float kx=((int)xIndex-dimx/2)*ux;
		float ky=((int)yIndex-dimy/2)*uy;
		//float Dr2=(rAx-kx)*(rAx-kx)+(rAy-ky)*(rAy-ky);
		float k2=kx*kx+ky*ky;
		dZF.x=__cosf(PI*Lambda*k2*dZ);
		dZF.y=-__sinf(PI*Lambda*k2*dZ);
		 d_img[index]=cuCmulf(d_img[index],dZF);
	}

}	
__global__ void ApplayNoise(cufftComplex *d_tmp_c, int dimx,int dimy,int *seed){
	  unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
	  unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
   __shared__ int data;
   // int data;
  data=0;
		int index  = xIndex + dimx*yIndex;
		int indexs  = threadIdx.x +  blockDim.x *threadIdx.y;

   	double const  a =16807;      //ie 7**5
	double  m = 2147483647; //ie 2**31-1
	double reciprocal_m = 1.0/m;
	unsigned int result = 0;
	unsigned int result1 = 0;
	/*if(data==0)
		data=seed[indexs]+index;

   while (data==(seed[indexs]+index)){
		double temp = data * a;
		result = (int) (temp - m * floor ( temp * reciprocal_m ));
		data = result;
   }
   seed[indexs]=result;
		__syncthreads();*/

	if((xIndex < dimx) && (yIndex < dimy)){

		//unsigned int
		//result = 0;
        
		data=seed[indexs]+index;
		//data =  result; //+ index;
		double temp = data * a;
		result = (int) (temp - m * floor ( temp * reciprocal_m ));
		data = result;
		//temp=sin((data-m/2)/m*100.0f);
		temp=(data-m/2)/m;
		seed[indexs]=result;
		
		data=result+index;
		//data =  result; //+ index;
		double temp1 = data * a;
		result1 = (int) (temp1 - m * floor ( temp1 * reciprocal_m ));
		data = result1;
		//temp=sin((data-m/2)/m*100.0f);
		temp1=(data-m/2)/m;
		seed[indexs]=result1;
		float s=temp1*temp1+temp*temp;
		float r1=temp*__fsqrt_rn(-2.0*__logf(s)/s);

		// temp1 = (int)r1 * a;
		//result1 = (int) (temp1 - m * floor ( temp1 * reciprocal_m ));
		//data = result1;
		////temp=sin((data-m/2)/m*100.0f);
		//temp1=(data-m/2)/m;

		d_tmp_c[index].x+=r1;

		d_tmp_c[index].y+=r1;

	}
		//__syncthreads();

}

//
__global__ void ApplayNoiseAdd(cufftComplex *d_tmp_a,cufftComplex *d_tmp_c, int dimx,int dimy,int *seed,double NoiseLevel){
	  unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
	  unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;
   __shared__ int data;
   // int data;
  data=0;
		int index  = xIndex + dimx*yIndex;
		int indexs  = threadIdx.x +  blockDim.x *threadIdx.y;

   	double const  a =16807;      //ie 7**5
	double  m = 2147483647; //ie 2**31-1
	double reciprocal_m = 1.0/m;
	unsigned int result = 0;
	unsigned int result1 = 0;
	/*if(data==0)
		data=seed[indexs]+index;

   while (data==(seed[indexs]+index)){
		double temp = data * a;
		result = (int) (temp - m * floor ( temp * reciprocal_m ));
		data = result;
   }
   seed[indexs]=result;
		__syncthreads();*/

	if((xIndex < dimx) && (yIndex < dimy)){

		//unsigned int
		//result = 0;
        
		data=seed[indexs]+index;
		//data =  result; //+ index;
		double temp = data * a;
		result = (int) (temp - m * floor ( temp * reciprocal_m ));
		data = result;
		//temp=sin((data-m/2)/m*100.0f);
		temp=(data-m/2)/m;
		seed[indexs]=result;
		
		data=result+index;
		//data =  result; //+ index;
		double temp1 = data * a;
		result1 = (int) (temp1 - m * floor ( temp1 * reciprocal_m ));
		data = result1;
		//temp=sin((data-m/2)/m*100.0f);
		temp1=(data-m/2)/m;
		seed[indexs]=result1;
		float s=temp1*temp1+temp*temp;
		float r1=temp*__fsqrt_rn(-2.0*__logf(s)/s)*NoiseLevel/100.0f;

		// temp1 = (int)r1 * a;
		//result1 = (int) (temp1 - m * floor ( temp1 * reciprocal_m ));
		//data = result1;
		////temp=sin((data-m/2)/m*100.0f);
		//temp1=(data-m/2)/m;

		d_tmp_a[index].x+=d_tmp_c[index].x+r1;

		d_tmp_a[index].y+=d_tmp_c[index].y+r1;

	}
		//__syncthreads();

}

//
#endif