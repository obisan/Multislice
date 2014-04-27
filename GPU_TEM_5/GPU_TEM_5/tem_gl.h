#include "tem_gl_kernel.h"
#include <sys/stat.h>
float defocusSeria[1024];
bool ImgNotWav = false;

		unsigned int RecalcuateMinMax=1;
		float img_min,img_max;

		float Cs=0.6E+06;
		float Lambda;
		float ux,uy;
		float Df;
		float H_Cc;
		float conv=0.001f;
		float tetta;
		float delta=7.0f;
		float Contrast=1.0f;
		float ImagesizeInnm=7.1f;
		float AppertureRadius=5.0f;
		unsigned int Apperture=0;
		unsigned int ImageOrDifr=1;
		unsigned int NoiseOn=0;
		unsigned int CurrentKadr=0;
		unsigned int Exposition=20;
		float Zoom=1.0f,ZoomImage=1.0f;
		int width,height;
		double dZ=0.0;
		cufftComplex* d_img = NULL,*d_tmp_c=NULL, *d_tmp_a=NULL,*d_tmp_show=NULL;
		float* h_img = NULL,*d_result_tex = NULL;
		int slice=0;
		cufftHandle plan;
		int dimx,dimy,nproj;
		float a,b,c,alpha,beta,gamma,HT,thickness;
		cudaArray* cu_array;
		cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();
float tx = 0.0f, ty = 0.0f;    // image translation
float cx, cy;                   // image centre
float Ax=0.0f,Ay=0.0f;
char filename_short[4096]="";
bool CTFoff=false;
	// curandState* devStates;
StopWatchInterface *timer = NULL;
int *h_seed;
		int BLOCK_DIM=16;

/*		mrc_header header;
		union all_mrc_ext_h ext_header;
float *ReadMRC2Float(char *fname,int &nx,int &ny,int &nz){
		unsigned char * image=0;
		short *image_short=0;
		float *image_float=0;

		image=ReadMRC(fname, header, ext_header);
		nx=header.nx;
		ny=header.ny;
		nz=header.nz;
	if (header.mode == 0) {
	//		WriteRawFileT<unsigned char>(image,fnameout,header.nx,header.ny,header.nz);
	//		WriteMRC<unsigned char>(fnamemrc,image,header,ext_header);

			image_float=(float*)malloc(nx*ny*nz*sizeof(float));
			for (int i=0;i<nx*ny*nz;i++)
				image_float[i]=(float)image[i];

		}
	if (header.mode == 1) {
			image_short=(short*)image;
	//		WriteRawFileT<short>(image_short,fnameout,header.nx,header.ny,header.nz);
	//		WriteMRC<short>(fnamemrc,image_short,header,ext_header);
			image_float=(float*)malloc(nx*ny*nz*sizeof(float));
			for (int i=0;i<nx*ny*nz;i++)
				image_float[i]=(float)image_short[i];

		}
	if (header.mode == 2) {
			image_float=(float*)image;
	//		WriteRawFileT<float>(image_float,fnameout,header.nx,header.ny,header.nz);
	//		WriteMRC<float>(fnamemrc,image_float,header,ext_header);
		}

	return image_float;
	}
	*/
cufftComplex *ReadMRC2Complex(char *fname,int &nx,int &ny,int &nz){
		unsigned char * image=0;
		short *image_short=0;
		short2 *image_short2=0;
		float *image_float=0;
		float2 *image_float2=0;
		cufftComplex *image_complex=0;
		image=ReadMRC(fname, header, ext_header);
		nx=header.nx;
		ny=header.ny;
		nz=header.nz;
	if (header.mode == 3)  {
	//		WriteRawFileT<unsigned char>(image,fnameout,header.nx,header.ny,header.nz);
	//		WriteMRC<unsigned char>(fnamemrc,image,header,ext_header);
			image_short2=(short2*)image;

			image_complex=(cufftComplex*)malloc(nx*ny*nz*sizeof(cufftComplex));
			for (int i=0;i<nx*ny*nz;i++){
				image_complex[i].x=(float)image_short2[i].x;
				image_complex[i].y=(float)image_short2[i].y;
				}

		} 
	if (header.mode == 4)  {
	//		WriteRawFileT<unsigned char>(image,fnameout,header.nx,header.ny,header.nz);
	//		WriteMRC<unsigned char>(fnamemrc,image,header,ext_header);
			image_float2=(float2*)image;

			image_complex=(cufftComplex*)malloc(nx*ny*nz*sizeof(cufftComplex));
			for (int i=0;i<nx*ny*nz;i++)
				image_complex[i]=(cufftComplex)image_float2[i];

		} 
	if (header.mode == 1) {
			image_short=(short*)image;
	//		WriteRawFileT<short>(image_short,fnameout,header.nx,header.ny,header.nz);
	//		WriteMRC<short>(fnamemrc,image_short,header,ext_header);
			image_float=(float*)malloc(nx*ny*nz*sizeof(float));
			for (int i=0;i<nx*ny*nz;i++) {
				image_complex[i].x=(float)image_short[i];
				image_complex[i].y=0.0f;
				}

		}
	if (header.mode == 2) {
			image_float=(float*)image;
	//		WriteRawFileT<float>(image_float,fnameout,header.nx,header.ny,header.nz);
	//		WriteMRC<float>(fnamemrc,image_float,header,ext_header);
				for (int i=0;i<nx*ny*nz;i++) {
				image_complex[i].x=image_float[i];
				image_complex[i].y=0.0f;
				}
		}
	if (header.mode == 0) {
	//		WriteRawFileT<unsigned char>(image,fnameout,header.nx,header.ny,header.nz);
	//		WriteMRC<unsigned char>(fnamemrc,image,header,ext_header);

			image_float=(float*)malloc(nx*ny*nz*sizeof(float));
			for (int i=0;i<nx*ny*nz;i++){
				image_complex[i].x=(float)image[i];
				image_complex[i].y=0.0f;
				}

		}
	return image_complex;
	}
cufftDoubleComplex *ReadWavAndParams(TCHAR* filename,float &a,float &b,float &c,float &alpha,float &beta,float &gamma,float &HT,int &dx,int &dy,float &thickness){
	struct stat results;
   
	if (stat(filename, &results) == 0){
        // The size of the file in bytes is in
        // results.st_size
		printf("File size %d\n",results.st_size);
		FILE *fp;
		int size = (int)results.st_size;
		unsigned char *data = (unsigned char *) malloc(size*sizeof(unsigned char));
		fopen_s(&fp,filename, "rb");
		if (!fp) {
			fprintf(stderr, "Error opening file '%s'\n", filename);
			return NULL;
		}
		size_t read = fread(data, sizeof(unsigned char),size, fp);
		printf("Read '%s', %d bytes\n", filename, read);
		fclose(fp);
		double2 *image_d_c=0;

		image_d_c=(double2*)data;


	//char *ds=0;
	  char *ds = ( char *) malloc(160*sizeof( char));
	  char ds1[11][16];// = ( char *) malloc(16*sizeof( char));
	  for(int i=0;i<160;i++){
		  if (data[size-160+i]==','){
			  ds[i]='.';
			  } else {
			  ds[i]=data[size-160+i];
			  }
		  }
	//free(data);
	//free(image_d_c);
	//for(int j=0;j<78;j++){
	//	 //ds1[j]=ds[j+3];
	////ss.data()=ds;
	//	 printf("%c",ds[j]);
	//	}
	//	 printf("\n");

	for(int j=3;j<159;j++){
		if(ds[j]==' ' && ds[j+1]==' '){
			for (int k=j;k<159;k++)
				ds[k]=ds[k+1];
			j-=1;
			}
		}
	//test = Regex.Replace(ds, @"\s{2,}", " ");
	
	//std::string ss(ds,160);
		int l=0,n1=0,n2=0;

	for(int j=0;j<160;j++){
		 //ds1[j]=ds[j+3];
	//ss.data()=ds;
		 //printf("%c",ds[j]);
		if(ds[j]==' '){
			 n1=j;
			 //printf("j=%d",j);
			 for (int k=j+1;k<159;k++)
				if(ds[k]==' '){
					 n2=k;
					// printf(" k=%d",k);
					goto stop;
					}
			stop: ;//printf("n1 %d n2 %d\n",n1,n2);
			if (n1<n2) {
				for (int m=0;m<(n2-n1);m++)
				ds1[l][m]=ds[n1+m];
				printf(" l=%d\n",l);
				l+=1;
				}
		}
		//stop: continue;
		}

		// printf("\n");
		//for (int n=0;n<11;n++)
		//	printf("\n atof= %.5f\n",atof( ds1[n] ));
	a=(float)atof( ds1[0]);
	b=(float)atof( ds1[1]);
	c=(float)atof( ds1[2]);
	alpha=(float)atof( ds1[3]);
	beta=(float)atof( ds1[4]);
	gamma=(float)atof( ds1[5]);
	HT=(float)atof( ds1[6]);
	dx=atoi( ds1[7]);
	dy=atoi( ds1[8]);
	thickness=(float)atof( ds1[9]);

	//double	value = atof( ds1 );
	//printf("\n atof= %.5f\n",value);

	/*FILE *fp1;
	fopen_s(&fp1,"c:\\image\\0_test_double_complex_1024.dat", "w+b");
    if (!fp1) {
        fprintf(stderr, "Error opening file '%s'\n", "c:\\image\\0_test_double_complex_1024.dat");
        return;
    }
	size_t write  = fwrite(image_d_c, sizeof(double2), 1024*1024, fp1);
    fclose(fp1);
    printf("Write MRC '%s', %d items \n", "c:\\image\\0_test_double_complex_1024.dat", write);*/

	//free(data);
		return image_d_c;

		}
	else {
		printf("Error get file %s info\n",filename);
		return NULL;
		}
	}
//
	cufftDoubleComplex *ReadWavAndParams2(TCHAR* filename,float &a,float &b,float &c,float &alpha,float &beta,float &gamma,float &HT,int &dx,int &dy,float &thickness){
	struct stat results;
   
	if (stat(filename, &results) == 0){
        // The size of the file in bytes is in
        // results.st_size
		printf("File size %d\n",results.st_size);
		FILE *fp;
		int size = (int)results.st_size;
		fopen_s(&fp,filename, "rb");
		if (!fp) {
			fprintf(stderr, "Error opening file '%s'\n", filename);
			return NULL;
		}
		double2 *image_d_c=0;



	//char *ds=0;
		fpos_t pos;

		pos=(fpos_t)(size-160);
		if( fsetpos( fp, &pos ) != 0 ) {
			perror( "fsetpos error" );
			return NULL;
		}


	  char *ds = ( char *) malloc(160*sizeof( char));
	  size_t read = fread(ds, sizeof(unsigned char),160, fp);
	  printf("Read '%s', %d bytes\n", filename, read);

	  char ds1[11][16];// = ( char *) malloc(16*sizeof( char));
	  for(int i=0;i<160;i++){
		  if (ds[i]==','){
			  ds[i]='.';
			  } else {
			  ds[i]=ds[i];
			  }
		  }
	//free(data);
	//free(image_d_c);
	//for(int j=0;j<78;j++){
	//	 //ds1[j]=ds[j+3];
	////ss.data()=ds;
	//	 printf("%c",ds[j]);
	//	}
	//	 printf("\n");

	for(int j=3;j<159;j++){
		if(ds[j]==' ' && ds[j+1]==' '){
			for (int k=j;k<159;k++)
				ds[k]=ds[k+1];
			j-=1;
			}
		}
	//test = Regex.Replace(ds, @"\s{2,}", " ");
	
	//std::string ss(ds,160);
		int l=0,n1=0,n2=0;

	for(int j=0;j<160;j++){
		 //ds1[j]=ds[j+3];
	//ss.data()=ds;
		 //printf("%c",ds[j]);
		if(ds[j]==' '){
			 n1=j;
			 //printf("j=%d",j);
			 for (int k=j+1;k<159;k++)
				if(ds[k]==' '){
					 n2=k;
					// printf(" k=%d",k);
					goto stop;
					}
			stop: ;//printf("n1 %d n2 %d\n",n1,n2);
			if (n1<n2) {
				for (int m=0;m<(n2-n1);m++)
				ds1[l][m]=ds[n1+m];
				printf(" l=%d\n",l);
				l+=1;
				}
		}
		//stop: continue;
		}

		// printf("\n");
		//for (int n=0;n<11;n++)
		//	printf("\n atof= %.5f\n",atof( ds1[n] ));
	a=(float)atof( ds1[0]);
	b=(float)atof( ds1[1]);
	c=(float)atof( ds1[2]);
	alpha=(float)atof( ds1[3]);
	beta=(float)atof( ds1[4]);
	gamma=(float)atof( ds1[5]);
	HT=(float)atof( ds1[6]);
	dx=atoi( ds1[7]);
	dy=atoi( ds1[8]);
	thickness=(float)atof( ds1[9]);

	//double	value = atof( ds1 );
	//printf("\n atof= %.5f\n",value);

	/*FILE *fp1;
	fopen_s(&fp1,"c:\\image\\0_test_double_complex_1024.dat", "w+b");
    if (!fp1) {
        fprintf(stderr, "Error opening file '%s'\n", "c:\\image\\0_test_double_complex_1024.dat");
        return;
    }
	size_t write  = fwrite(image_d_c, sizeof(double2), 1024*1024, fp1);
    fclose(fp1);
    printf("Write MRC '%s', %d items \n", "c:\\image\\0_test_double_complex_1024.dat", write);*/

	//free(data);
	unsigned char *data = (unsigned char *) malloc(dx*dy*sizeof(double2));
	pos=(fpos_t)(0);
	if( fsetpos( fp, &pos ) != 0 ) {
		perror( "fsetpos to 0 error" );
		return NULL;
	}
	read = fread(data, sizeof(unsigned char),dx*dy*sizeof(double2), fp);
	printf("Read '%s', %d bytes\n", filename, read);
	printf("dx=%d dy=%d\n",dx,dy);
	fclose(fp);
	image_d_c=(double2*)data;
	//WriteRawFileT<double2>(image_d_c,"d:\\test.dat",dx,dy,1);
	//if( fopen_s( &stream, "fread.out", "w+t" ) == 0 )
	//{
	//	for ( i = 0; i < 25; i++ )
	//		list[i] = (char)('z' - i);
	//	// Write 25 characters to stream 
	//	numwritten = fwrite( list, sizeof( char ), 25, stream );
	//	printf( "Wrote %d items\n", numwritten );
	//	fclose( stream );

	//}
	//else
	//	printf( "Problem opening the file\n" );





		return image_d_c;

		}
	else {
		printf("Error get file %s info\n",filename);
		return NULL;
		}
	}
	//
void SaveImage(char *fname,cufftComplex *d_image_c,int dimx, int dimy){
	cufftComplex *h_img_tmp=0;
	h_img_tmp=(cufftComplex*)malloc(dimx*dimy*sizeof(cufftComplex));
	checkCudaErrors( cudaMemcpy( h_img_tmp, d_image_c, dimx*dimy*sizeof(cufftComplex), cudaMemcpyDeviceToHost));
	union mrc_h header_out;
		memset(&header_out.buff,0,1024); 
		header_out.header.mode=4;
		header_out.header.next=0;
		header_out.header.nx=dimx;
		header_out.header.ny=dimy;
		header_out.header.nz=1;
		header_out.header.alpha=90.0f;
		header_out.header.beta=90.0f;
		header_out.header.gamma=90.0f;
		header_out.header.mx=dimx;
		header_out.header.my=dimy;
		header_out.header.mz=1;
		header_out.header.xlen=(float)dimx;
		header_out.header.ylen=(float)dimy;
		header_out.header.zlen=(float)1;
		//header_out.header.amax=imax;
		//header_out.header.amin=imin;
		//header_out.header.amean=average;
		union all_mrc_ext_h ext_header_out;
		memset(&ext_header_out.buff,0,1024*128);
		WriteMRC<cufftComplex>(fname, h_img_tmp, header_out.header , ext_header_out);
		free(h_img_tmp);
	}
void SaveImage1(char *fname,unsigned int *d_image,int dimx, int dimy){
	 int *h_img_tmp=0;
	 char3 *h_img_tmp1=0;
	h_img_tmp=( int*)malloc(dimx*dimy*sizeof( int));
	h_img_tmp1=( char3*)malloc(dimx*dimy*sizeof( char3));
	checkCudaErrors( cudaMemcpy( h_img_tmp, d_image, dimx*dimy*sizeof( int), cudaMemcpyDeviceToHost));
	replace(fname,".wav",".mrc");
	replace(fname,"Wave at","Image ");
	//char *pch;
	//pch=strrchr(fname,'.');
	//int res=(int)(pch - fname + 1);
	//fname[res]='m';
	//fname[res+1]='r';
	//fname[res+2]='c';
	////fname[0]='I';
	//fname[1]='m';
	////fname[2]='a';
	////fname[3]='g';
	////fname[4]='e';
	union mrc_h header_out;
		memset(&header_out.buff,0,1024); 
		header_out.header.mode=16;
		header_out.header.next=0;
		header_out.header.nx=dimx;
		header_out.header.ny=dimy;
		header_out.header.nz=1;
		header_out.header.alpha=90.0f;
		header_out.header.beta=90.0f;
		header_out.header.gamma=90.0f;
		header_out.header.mx=dimx;
		header_out.header.my=dimy;
		header_out.header.mz=1;
		header_out.header.xlen=(float)ImagesizeInnm*10.0f;
		header_out.header.ylen=(float)ImagesizeInnm*10.0f;
		header_out.header.zlen=(float)1;
		header_out.header.mapc=1;
		header_out.header.mapr=2;
		header_out.header.maps=3;
		//header_out.header.amax=imax;
		//header_out.header.amin=imin;
		//header_out.header.amean=average;
		header_out.header.nlabl=5;
		header_out.header.next=128*1024; 
		header_out.header.nreal=32;
		strncpy(header_out.header.labels[0],filename_short,80)  ;
		//strncpy(header_out.header.labels[1],strcat("Cs=",ftoa(Cs)),80)  ;
		//strncpy(header_out.header.labels[1],strcat("Cs=",ftoa(Cs)),80)  ;
		sprintf(header_out.header.labels[1],"Cs=%.4f mm, delta=%.4f nm",Cs/1000000.0f,delta);
		union all_mrc_ext_h ext_header_out;
		memset(&ext_header_out.buff,0,1024*128);
		ext_header_out.ext_header[0].a_tilt=0.0f;
		ext_header_out.ext_header[0].b_tilt=0.0f;
		ext_header_out.ext_header[0].x_shift=0.0f;
		ext_header_out.ext_header[0].y_shift=0.0f;
		ext_header_out.ext_header[0].defocus=Df*1e-09f;
		ext_header_out.ext_header[0].pixel_size=ImagesizeInnm/dimx*1e-09f;
		ext_header_out.ext_header[0].ht=HT*1000.0f;
		for(int i=0;i<dimx*dimy;i++){
		 h_img_tmp1[i].x = (h_img_tmp[i] & 0xff) ;
		 h_img_tmp1[i].y = ((h_img_tmp[i]>>8) & 0xff) ;
		 h_img_tmp1[i].z = ((h_img_tmp[i]>>16) & 0xff) ;
			}
		WriteMRC<char3>(fname, h_img_tmp1, header_out.header , ext_header_out);
		free(h_img_tmp);
		free(h_img_tmp1);
		//_getch();
	}
void CalculateStats(cufftComplex *d_tmp_c,int dimx,int dimy,float &imin, float &imax, unsigned int ImageOrCTF){
	cufftComplex *h_image_c=0;
	h_image_c = (cufftComplex*)malloc(dimx*dimy*sizeof(cufftComplex));
	cudaMemcpy(h_image_c,d_tmp_c,dimx*dimy*sizeof(cufftComplex),cudaMemcpyDeviceToHost);

	//float imax,imin;
		imax=numeric_limits<float>::min();
		imin=numeric_limits<float>::max();
		float val=0.0f;
		for(int j=0;j<dimy;j+=2){
			for(int k=0;k<dimx;k+=2){
					int	idx	=	j*dimx	+	k;
					if(ImageOrCTF==1) {
					val=sqrtf(powf(h_image_c[idx].x,2)+powf(h_image_c[idx].y,2));
						} else {
							val=h_image_c[idx].y;
						}
					if (val<imin)
					imin=val;
					if (val>imax)
					imax=val;

				}
			}

	
	free(h_image_c);

	}
unsigned int nextPow2( unsigned int x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

void getNumBlocksAndThreads( int n, int maxBlocks, int maxThreads, int &blocks, int &threads)
{
 
        threads = (n < maxThreads*2) ? nextPow2((n + 1)/ 2) : maxThreads;
        blocks = (n + (threads * 2 - 1)) / (threads * 2);
        blocks = MIN(maxBlocks, blocks);
}
float GPU_max(int  n,int  numThreads,int  numBlocks,int  maxThreads,int  maxBlocks,cufftComplex* d_idata,float* d_odata){

	
	float gpu_result = 0;

    gpu_result = 0;

    cudaThreadSynchronize();
        // execute the kernel
        kernel_max1(n, numThreads, numBlocks,  d_idata, d_odata);// Первый вызов вычисляет модуль комплексных величин 
		// в d_odata частичные максимумы в пределах блоков, всего значений	numBlocks
        // check if kernel execution generated an error
        //cutilCheckMsg("Kernel execution failed");
 getLastCudaError("launch_kernel failed");
            // sum partial block sums on GPU
            int s=numBlocks;
            while(s > 1) // до тех пор пока блоков больше 1, вычисляем частичные паксимумы в пределах блоков
            {
                int threads = 0, blocks = 0;
                getNumBlocksAndThreads( s, maxBlocks, maxThreads, blocks, threads);
                
                kernel_max(s, threads, blocks, d_odata, d_odata);
                    s = (s + (threads*2-1)) / (threads*2);
            }
            

        cudaThreadSynchronize();

        // copy final sum from device to host  копируем результат из устройства в память компьютера
        checkCudaErrors( cudaMemcpy( &gpu_result, d_odata, sizeof(float), cudaMemcpyDeviceToHost) );

    return gpu_result;
	}
float GPU_min(int  n,int  numThreads,int  numBlocks,int  maxThreads,int  maxBlocks,cufftComplex* d_idata,float* d_odata){

	
	float gpu_result = 0;

    gpu_result = 0;

    cudaThreadSynchronize();
        // execute the kernel
        kernel_min1(n, numThreads, numBlocks,  d_idata, d_odata);

        // check if kernel execution generated an error
        //cutilCheckMsg("Kernel execution failed");
		 getLastCudaError("launch_kernel failed");
            // sum partial block sums on GPU
            int s=numBlocks;
            while(s > 1) 
            {
                int threads = 0, blocks = 0;
                getNumBlocksAndThreads( s, maxBlocks, maxThreads, blocks, threads);
                
                kernel_min(s, threads, blocks, d_odata, d_odata);
                    s = (s + (threads*2-1)) / (threads*2);
            }
            

        cudaThreadSynchronize();
        //cutilCheckError( sdkStopTimer(&timer) );      

        // copy final sum from device to host
        checkCudaErrors( cudaMemcpy( &gpu_result, d_odata, sizeof(float), cudaMemcpyDeviceToHost) );

    return gpu_result;
	}
//void CalculateStatsGPU(cufftComplex *d_tmp_c,int dimx,int dimy,float &imin, float &imax, unsigned int ImageOrCTF){
void CalculateStatsGPU(cufftComplex *d_tmp_c,int dimx,int dimy,float &imin, float &imax){
	//if (ImageOrCTF==1){
	int maxThreads = 128;  // number of threads per block
	int maxBlocks = 32;
    int numBlocks = 0;
    int numThreads = 0;
	//int blocks;
	//int threads;
//    printf("%d elements\n", dimx*dimy);
//    printf("%d threads (max)\n", maxThreads);
   // unsigned int bytes = dimx*dimy * sizeof(float);
    getNumBlocksAndThreads(dimx*dimy, maxBlocks, maxThreads, numBlocks, numThreads);
        //threads = (dimx*dimy < maxThreads*2) ? nextPow2((dimx*dimy + 1)/ 2) : maxThreads;
        //blocks = (dimx*dimy + (threads * 2 - 1)) / (threads * 2);
        //blocks = min(maxBlocks, blocks);
//printf("numThreads=%d, numBlocks=%d, maxThreads=%d, maxBlocks=%d\n",numThreads, numBlocks, maxThreads, maxBlocks);
    float* d_odata = NULL;
    checkCudaErrors( cudaMalloc((void**) &d_odata, numBlocks*sizeof(float)) );
	cudaMemset(d_odata,0,numBlocks*sizeof(float));
	imax = GPU_max(dimx*dimy, numThreads, numBlocks, maxThreads, maxBlocks, d_tmp_c, d_odata);
	imin = GPU_min(dimx*dimy, numThreads, numBlocks, maxThreads, maxBlocks, d_tmp_c, d_odata);
	//	} else {
	//	imax=1.0f;
	//	imin=-1.0f;
	//	}
	}
extern "C" void DrawImage(cufftComplex *d_img,unsigned int *d_result,int dimx,int dimy,int slice){
	if (slice>=nproj)
		slice=0;
	if (slice<0)
		slice=nproj-1;
	//if(NoiseOn==0)
	 cudaError_t error;
	     checkCudaErrors(cudaMemset(d_result,0,dimx*dimy*sizeof(uint)));
		
		dim3 grid(dimx / BLOCK_DIM, dimy / BLOCK_DIM, 1);
		dim3 threads(BLOCK_DIM, BLOCK_DIM, 1);
		//cudaMemset(d_tmp_c,0,height*width*sizeof(cufftComplex));
		//if(NoiseOn==0) {
			 checkCudaErrors(cudaMemcpy(d_tmp_c,d_img+slice*dimx*dimy,dimx*dimy*sizeof(cufftComplex),cudaMemcpyDeviceToDevice));
			 /*if (error != cudaSuccess)
			 {
			 printf("cudaMemcpy d_tmp_c <- d_img+slice*dimx*dimy returned error code %d, line(%d)\n", error, __LINE__);
			 exit(EXIT_FAILURE);
			 }*/
		//} else {
			//ImageAdd<<<grid,threads>>>(d_tmp_c,d_img+slice*dimx*dimy,dimx,dimy);
		//}
		
	
		//cufftExecC2C( plan, d_tmp_c,d_img+slice*width*height,  CUFFT_INVERSE);
		if (ImageOrDifr==1) {
			if (!CTFoff)
				CTF<<<grid,threads>>>(d_tmp_c, dimx, dimy, Cs, Lambda,  Df,delta, ux,uy,ImageOrCTF,H_Cc,tetta);
			getLastCudaError("Error CTF");

			/*cudaThreadSynchronize();
			error = cudaGetLastError();
			if( cudaSuccess != error) {
				printf("Kernel execution failed CTF tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
				_getch();
				exit(-1);
			}         */   
			if (Apperture==1) {
				AppertureAplay<<<grid,threads>>>(d_tmp_c, dimx, dimy,ux,uy,AppertureRadius,Ax,Ay);
				getLastCudaError("Error AppertureAplay");

				/*cudaThreadSynchronize();
				error = cudaGetLastError();
				if( cudaSuccess != error) {
					printf("Kernel execution failed AppertureAplay tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
					_getch();
					exit(-1);
				}         */
			}
		if (ImageOrCTF==1) {
			if (dZ!=0.0) {
				AplaydZ<<<grid,threads>>>(d_tmp_c, dimx,dimy,Lambda,ux,uy,dZ);
				getLastCudaError("Error AplaydZ");

			}
			/*cudaThreadSynchronize();
			error = cudaGetLastError();
			if( cudaSuccess != error) {
				printf("Kernel execution failed AplaydZ tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
				_getch();
				exit(-1);
			}        */
			QuadrantTileComplex<<<grid,threads>>>(d_tmp_c,dimx,dimy);
			getLastCudaError("Error QuadrantTileComplex");

			/*cudaThreadSynchronize();
			 error = cudaGetLastError();
			if( cudaSuccess != error) {
				printf("Kernel execution failed QuadrantTileComplex tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
				_getch();
				exit(-1);
			}      */
			//checkCudaErrors(cufftExecC2C( plan, d_tmp_c,d_tmp_c,  CUFFT_INVERSE));
			//cufftResult cufft_error=
				if (cufftExecC2C( plan, d_tmp_c,d_tmp_c,  CUFFT_INVERSE) != CUFFT_SUCCESS){
					fprintf(stderr, "CUFFT error: ExecC2C Inverse failed");
					_getch();
					exit(-1);	
				}

      // int  gpuTime = (int)floor(sdkGetTimerValue(&timer)*10000.0f);
	   
		//cudaMemcpy(d_seed,&gpuTime,sizeof(int),cudaMemcpyHostToDevice);
			if(NoiseOn==1){
				//if(CurrentKadr==Exposition){

				//	cudaMemcpy(d_tmp_show,d_tmp_a,dimx*dimy*sizeof(cufftComplex),cudaMemcpyDeviceToDevice);

				//	cudaMemset(d_tmp_a,0,dimx*dimy*sizeof(cufftComplex));
				//	CurrentKadr=0;
				//}

				for(int i=0;i<BLOCK_DIM*BLOCK_DIM;i++){
					h_seed[i]=(int)floor(sdkGetTimerValue(&timer)*10000.0f);
				}
				checkCudaErrors(cudaMemcpy(d_seed,h_seed,BLOCK_DIM*BLOCK_DIM*sizeof(int),cudaMemcpyHostToDevice));
				dim3 grid1(dimx , dimy , 1);
				dim3 threads1(1, 1, 1);
				ApplayNoiseAdd<<<grid1,threads1>>>(d_tmp_a,d_tmp_c, dimx, dimy,d_seed,NoiseLevel);
				getLastCudaError("Error ApplayNoiseAdd");

				/*cudaThreadSynchronize();
				cudaError_t error = cudaGetLastError();
				if( cudaSuccess != error) {
					printf("Kernel execution failed QuadrantTileComplex tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
					_getch();
					exit(-1);
				}      */
				//ApplayNoise<<<grid1,threads1>>>(d_tmp_c, dimx, dimy,d_seed);
			//CurrentKadr++;
			} else {
					checkCudaErrors(cudaMemcpy(d_tmp_show,d_tmp_c,dimx*dimy*sizeof(cufftComplex),cudaMemcpyDeviceToDevice));

			}
			//printf("time %d %d\n",(int)time(NULL),gpuTime);
			CalculateStatsGPU(d_tmp_show,dimx,dimy,img_min, img_max);
			//setup_kernel<<<grid,threads>>>(devStates,time(NULL),dimx,dimy);
			} else {
					checkCudaErrors(cudaMemcpy(d_tmp_show,d_tmp_c,dimx*dimy*sizeof(cufftComplex),cudaMemcpyDeviceToDevice));

			CalculateStatsGPU(d_tmp_show,dimx,dimy,img_min, img_max);
			img_max*=Contrast;
			}
			} else {
					checkCudaErrors(cudaMemcpy(d_tmp_show,d_tmp_c,dimx*dimy*sizeof(cufftComplex),cudaMemcpyDeviceToDevice));

			CalculateStatsGPU(d_tmp_show,dimx,dimy,img_min, img_max);
			img_max*=Contrast;
			}
			
		//if (RecalcuateMinMax==1){
		//CalculateStats(d_tmp_c,dimx,dimy,img_min,img_max,ImageOrCTF);
		//RecalcuateMinMax=0;
		//	}
		//if (ImageOrCTF==1) {
		
			CopyComplexToFloat<<<grid,threads>>>(d_result_tex,d_tmp_show, dimx, dimy,ImageOrCTF,RealCTFOrComplexCTF);
			getLastCudaError("Error CopyComplexToFloat");
			/*cudaThreadSynchronize();

			error = cudaGetLastError();
			if( cudaSuccess != error) {
				printf("Kernel execution failed QuadrantTileComplex tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
				_getch();
				exit(-1);
			}      */
		
			checkCudaErrors( cudaMemcpyToArray( cu_array, 0, 0, d_result_tex, dimx*dimy*sizeof(float), cudaMemcpyDeviceToDevice));

	if(NoiseOn==0){
					CreateRGBAfromTexture<<<grid,threads>>>(d_result,dimx,dimy,img_min,img_max,ImageOrCTF,RealCTFOrComplexCTF,Zoom,cx,cy,tx,ty);
					getLastCudaError("Error CreateRGBAfromTexture");
					/*cudaThreadSynchronize();
					 error = cudaGetLastError();
					if( cudaSuccess != error) {
						printf("Kernel execution failed QuadrantTileComplex tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
						_getch();
						exit(-1);
					}      */
	}
	else
	{
			CreateRGBAfromTextureNoise<<<grid,threads>>>(d_result,dimx,dimy,img_min,img_max,ImageOrCTF,RealCTFOrComplexCTF,Zoom,cx,cy,tx,ty);
			getLastCudaError("Error CreateRGBAfromTextureNoise");
			/*cudaThreadSynchronize();
			 error = cudaGetLastError();
			if( cudaSuccess != error) {
				printf("Kernel execution failed QuadrantTileComplex tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
				_getch();
				exit(-1);
			}      */
	}
			//} else {
		
			//	CreateRGBAfromComplex<<<grid,threads>>>(d_result,d_tmp_c,dimx,dimy,img_min,img_max,ImageOrCTF,RealCTFOrComplexCTF,Zoom);

			//}
	}
// 
void CreateFFT(cufftComplex * d_image, int dimx, int dimy){
	int BLOCK_DIM=16;
	dim3 grid(dimx / BLOCK_DIM, dimy / BLOCK_DIM, 1);
    dim3 threads(BLOCK_DIM, BLOCK_DIM, 1);
	int device;
	 cudaGetDevice( &device );
	 if (debugON)
	 printf("Again device=%d!!!!!!!!!!!!!!!!!\n",device);
	//checkCudaErrors(cufftExecC2C(plan, d_image, d_image, CUFFT_FORWARD));
		if (cufftExecC2C(plan, d_image, d_image, CUFFT_FORWARD) != CUFFT_SUCCESS){
			fprintf(stderr, "CUFFT error: ExecC2C Forward failed");
			_getch();
			exit(-1);	
		}
	QuadrantTileComplex_TEM<<<grid,threads>>>(d_image,dimx,dimy);
	//_getch();
	cudaThreadSynchronize();
	cudaError_t error = cudaGetLastError();
	if( cudaSuccess != error) {
		printf("Kernel execution failed QuadrantTileComplex tem_gl.h! ErrNo %d, Error ='%s'\n Any key to exit.\n",(int)error, cudaGetErrorString( error ));
		_getch();
		exit(-1);
	}      
	}
//

extern "C" void DrawImageC(cufftComplex *d_img,unsigned int *d_result,int dimx,int dimy,int slice){
		cudaMemset(d_result,0,dimx*dimy*sizeof(uint));
		int BLOCK_DIM=16;
		
		dim3 grid(dimx / BLOCK_DIM, dimy / BLOCK_DIM, 1);
		dim3 threads(BLOCK_DIM, BLOCK_DIM, 1);

		cudaMemcpy(d_tmp_c,d_img+slice*dimx*dimy,dimx*dimy*sizeof(cufftComplex),cudaMemcpyDeviceToDevice);

		if (ImageOrDifr==1) {
			
			CTF<<<grid,threads>>>(d_tmp_c, dimx, dimy, Cs, Lambda,  Df,delta, ux,uy,ImageOrCTF,H_Cc,tetta);
			if (dZ!=0.0)
				AplaydZ<<<grid,threads>>>(d_tmp_c, dimx,dimy,Lambda,ux,uy,dZ);

			if (ImageOrCTF==1) {
				QuadrantTileComplex<<<grid,threads>>>(d_tmp_c,dimx,dimy);
				cufftExecC2C( plan, d_tmp_c,d_tmp_c,  CUFFT_INVERSE);
				CalculateStatsGPU(d_tmp_c,dimx,dimy,img_min, img_max);
				} else {
				CalculateStatsGPU(d_tmp_c,dimx,dimy,img_min, img_max);
				img_max*=Contrast;
				}
			} else {
				CalculateStatsGPU(d_tmp_c,dimx,dimy,img_min, img_max);
				img_max*=Contrast;
			}
			
			CopyComplexToFloat<<<grid,threads>>>(d_result_tex,d_tmp_c, dimx, dimy,ImageOrCTF,RealCTFOrComplexCTF);
		
			checkCudaErrors( cudaMemcpyToArray( cu_array, 0, 0, d_result_tex, dimx*dimy*sizeof(float), cudaMemcpyDeviceToDevice));
		
			CreateRGBAfromTexture<<<grid,threads>>>(d_result,dimx,dimy,img_min,img_max,ImageOrCTF,RealCTFOrComplexCTF,Zoom,cx,cy,tx,ty);
	
	}
		
extern "C" void CopyDImageToSeria(char3 *SeriaImage, unsigned int *d_image,int dimx,int dimy,int SeriaCounter){
	int *h_img_tmp=0;
	//char3 *h_img_tmp1=0;
	h_img_tmp=( int*)malloc(dimx*dimy*sizeof( int));
	//h_img_tmp1=( char3*)malloc(dimx*dimy*sizeof( char3));
	checkCudaErrors( cudaMemcpy( h_img_tmp, d_image, dimx*dimy*sizeof( int), cudaMemcpyDeviceToHost));
	int j;
	for(int i=0;i<dimx*dimy;i++){
		j=i+SeriaCounter*dimx*dimy;
		 SeriaImage[j].x = (h_img_tmp[i] & 0xff) ;
		 SeriaImage[j].y = ((h_img_tmp[i]>>8) & 0xff) ;
		 SeriaImage[j].z = ((h_img_tmp[i]>>16) & 0xff) ;
	}
	
	//memcpy(SeriaImage+SeriaCounter*dimx*dimy,h_img_tmp1,dimx*dimy*sizeof(char3));
   		free(h_img_tmp);

	}

void SaveImageSeria(char3 *SeriaImage, char *fname,int dimx, int dimy,int dimz){
	// int *h_img_tmp=0;
	// char3 *h_img_tmp1=0;
	//h_img_tmp=( int*)malloc(dimx*dimy*sizeof( int));
	//h_img_tmp1=( char3*)malloc(dimx*dimy*sizeof( char3));
	//checkCudaErrors( cudaMemcpy( h_img_tmp, d_image, dimx*dimy*sizeof( int), cudaMemcpyDeviceToHost));
	replace(fname,".wav",".mrc");
	replace(fname,"Wave at","ImageFS");
	//char *pch;
	//pch=strrchr(fname,'.');
	//int res=(int)(pch - fname + 1);
	//fname[res]='m';
	//fname[res+1]='r';
	//fname[res+2]='c';
	////fname[0]='I';
	//fname[1]='m';
	////fname[2]='a';
	////fname[3]='g';
	////fname[4]='e';
	union mrc_h header_out;
		memset(&header_out.buff,0,1024); 
		header_out.header.mode=16;
		header_out.header.next=128*1024;
		header_out.header.nx=dimx;
		header_out.header.ny=dimy;
		header_out.header.nz=dimz;
		header_out.header.alpha=90.0f;
		header_out.header.beta=90.0f;
		header_out.header.gamma=90.0f;
		header_out.header.mx=dimx;
		header_out.header.my=dimy;
		header_out.header.mz=dimz;
		header_out.header.xlen=(float)ImagesizeInnm*10.0f;
		header_out.header.ylen=(float)ImagesizeInnm*10.0f;
		header_out.header.zlen=30.5f;//(float)dimz;
		header_out.header.nreal=32;
		header_out.header.mapc=1;
		header_out.header.mapr=2;
		header_out.header.maps=3;
		header_out.header.zorg=30.5f;
		//header_out.header.amax=imax;
		//header_out.header.amin=imin;
		//header_out.header.amean=average;
		header_out.header.nlabl=2;
		strncpy(header_out.header.labels[0],filename_short,80)  ;
				sprintf(header_out.header.labels[1],"Cs=%.2f mm, delta=%.1f nm",Cs/1.0e06,delta);
//strncpy(header_out.header.labels[1],filename_short,80)  ;
		union all_mrc_ext_h ext_header_out;
		memset(&ext_header_out.buff,0,1024*128);
		for(int i=0;i<dimz;i++){
			ext_header_out.ext_header[i].a_tilt = 0.0f;
			ext_header_out.ext_header[i].b_tilt = 0.0f;
			ext_header_out.ext_header[i].exp_time = 1.0f;
			ext_header_out.ext_header[i].appliedDefocus = 0.0f;
			ext_header_out.ext_header[i].binning = 1.0f;
			ext_header_out.ext_header[i].ht =HT* 1000.0f;
			ext_header_out.ext_header[i].defocus=defocusSeria[i];
			ext_header_out.ext_header[i].pixel_size=ImagesizeInnm/dimx*1e-09f;
			}
		//for(int i=0;i<dimx*dimy;i++){
		// h_img_tmp1[i].x = (h_img_tmp[i] & 0xff) ;
		// h_img_tmp1[i].y = ((h_img_tmp[i]>>8) & 0xff) ;
		// h_img_tmp1[i].z = ((h_img_tmp[i]>>16) & 0xff) ;
		//	}
		WriteMRC<char3>(fname, SeriaImage, header_out.header , ext_header_out);
		//free(h_img_tmp);
		//free(h_img_tmp1);
	}
extern "C" void CopyByteImageToSeria(unsigned char *SeriaImage, unsigned int *d_image,int dimx,int dimy,int SeriaCounter){
	int *h_img_tmp=0;
	//char3 *h_img_tmp1=0;
	h_img_tmp=( int*)malloc(dimx*dimy*sizeof( int));
	//h_img_tmp1=( char3*)malloc(dimx*dimy*sizeof( char3));
	checkCudaErrors( cudaMemcpy( h_img_tmp, d_image, dimx*dimy*sizeof( int), cudaMemcpyDeviceToHost));
	int j;
	for(int i=0;i<dimx*dimy;i++){
		j=i+SeriaCounter*dimx*dimy;
		 SeriaImage[j] = (h_img_tmp[i] & 0xff) ;
		 //SeriaImage[j].y = ((h_img_tmp[i]>>8) & 0xff) ;
		 //SeriaImage[j].z = ((h_img_tmp[i]>>16) & 0xff) ;
	}
	
	//memcpy(SeriaImage+SeriaCounter*dimx*dimy,h_img_tmp1,dimx*dimy*sizeof(char3));
   		free(h_img_tmp);

	}
extern "C" void CopyFloatImageToSeria(float *SeriaImage, unsigned int *d_image,int dimx,int dimy,int SeriaCounter){
	float *h_img_tmp=0;
	//char3 *h_img_tmp1=0;
	h_img_tmp=( float*)malloc(dimx*dimy*sizeof( float));
	//h_img_tmp1=( char3*)malloc(dimx*dimy*sizeof( char3));
	checkCudaErrors( cudaMemcpy( h_img_tmp, d_result_tex, dimx*dimy*sizeof( float), cudaMemcpyDeviceToHost));
	int j;
	for(int i=0;i<dimx*dimy;i++){
		j=i+SeriaCounter*dimx*dimy;
		 SeriaImage[j] = h_img_tmp[i];
		 //SeriaImage[j].y = ((h_img_tmp[i]>>8) & 0xff) ;
		 //SeriaImage[j].z = ((h_img_tmp[i]>>16) & 0xff) ;
	}
	
	//memcpy(SeriaImage+SeriaCounter*dimx*dimy,h_img_tmp1,dimx*dimy*sizeof(char3));
   		free(h_img_tmp);

	}
void SaveByteImageSeria(unsigned char *SeriaImage, char *fname,int dimx, int dimy,int dimz){
	// int *h_img_tmp=0;
	// char3 *h_img_tmp1=0;
	//h_img_tmp=( int*)malloc(dimx*dimy*sizeof( int));
	//h_img_tmp1=( char3*)malloc(dimx*dimy*sizeof( char3));
	//checkCudaErrors( cudaMemcpy( h_img_tmp, d_image, dimx*dimy*sizeof( int), cudaMemcpyDeviceToHost));
	if (ImgNotWav) {
			replace(fname,".img",".mrc");
			replace(fname,"wave","Focal serie of wave");
			//sprintf(fname,"Focal Serie of %s",fname);
		} else {
			replace(fname,".wav",".mrc");
			replace(fname,"Wave at","ImageFS");

		}
	//char *pch;
	//pch=strrchr(fname,'.');
	//int res=(int)(pch - fname + 1);
	//fname[res]='m';
	//fname[res+1]='r';
	//fname[res+2]='c';
	////fname[0]='I';
	//fname[1]='m';
	////fname[2]='a';
	////fname[3]='g';
	////fname[4]='e';
	union mrc_h header_out;
		memset(&header_out.buff,0,1024); 
		header_out.header.mode=0;
		header_out.header.next=128*1024;
		header_out.header.nx=dimx;
		header_out.header.ny=dimy;
		header_out.header.nz=dimz;
		header_out.header.alpha=90.0f;
		header_out.header.beta=90.0f;
		header_out.header.gamma=90.0f;
		header_out.header.mx=dimx;
		header_out.header.my=dimy;
		header_out.header.mz=dimz;
		header_out.header.xlen=(float)ImagesizeInnm*10.0f;
		header_out.header.ylen=(float)ImagesizeInnm*10.0f;
		header_out.header.zlen=30.5f;//(float)dimz;
		header_out.header.nreal=32;
		header_out.header.mapc=1;
		header_out.header.mapr=2;
		header_out.header.maps=3;
		header_out.header.zorg=30.5f;
		//header_out.header.amax=imax;
		//header_out.header.amin=imin;
		//header_out.header.amean=average;
		header_out.header.nlabl=3;
		strncpy(header_out.header.labels[0],filename_short,80)  ;
				sprintf(header_out.header.labels[1],"Cs=%.2f um, delta=%.1f nm, dZ=%.1f nm.",Cs/1.0e03,delta,dZ,STRPRODUCTVER);
				sprintf(header_out.header.labels[2],"GPU TEM ver.%s %s %s (c) pust@lemoi.phys.dvgu.ru",STRPRODUCTVER,DATESTAMP,TIMESTAMP);
//strncpy(header_out.header.labels[1],filename_short,80)  ;
		union all_mrc_ext_h ext_header_out;
		memset(&ext_header_out.buff,0,1024*128);
		for(int i=0;i<dimz;i++){
			ext_header_out.ext_header[i].a_tilt = 0.0f;
			ext_header_out.ext_header[i].b_tilt = 0.0f;
			ext_header_out.ext_header[i].exp_time = 1.0f;
			ext_header_out.ext_header[i].appliedDefocus = 0.0f;
			ext_header_out.ext_header[i].binning = 1.0f;
			ext_header_out.ext_header[i].ht =HT* 1000.0f;
			ext_header_out.ext_header[i].defocus=defocusSeria[i];
			ext_header_out.ext_header[i].pixel_size=ImagesizeInnm/dimx*1e-09f;
			}
		//for(int i=0;i<dimx*dimy;i++){
		// h_img_tmp1[i].x = (h_img_tmp[i] & 0xff) ;
		// h_img_tmp1[i].y = ((h_img_tmp[i]>>8) & 0xff) ;
		// h_img_tmp1[i].z = ((h_img_tmp[i]>>16) & 0xff) ;
		//	}
		WriteMRC<unsigned char>(fname, SeriaImage, header_out.header , ext_header_out);
		//free(SeriaImage);
		//free(h_img_tmp);
		//free(h_img_tmp1);
	}
void SaveFloatImageSeria(float *SeriaImage, char *fname,int dimx, int dimy,int dimz){
	// int *h_img_tmp=0;
	// char3 *h_img_tmp1=0;
	//h_img_tmp=( int*)malloc(dimx*dimy*sizeof( int));
	//h_img_tmp1=( char3*)malloc(dimx*dimy*sizeof( char3));
	//checkCudaErrors( cudaMemcpy( h_img_tmp, d_image, dimx*dimy*sizeof( int), cudaMemcpyDeviceToHost));
	if (ImgNotWav) {
			replace(fname,".img",".mrc");
			replace(fname,"wave","f_Focal serie of wave");
			//sprintf(fname,"Focal Serie of %s",fname);
		} else {
			replace(fname,".wav",".mrc");
			replace(fname,"Wave at","f_ImageFS");

		}
	//char *pch;
	//pch=strrchr(fname,'.');
	//int res=(int)(pch - fname + 1);
	//fname[res]='m';
	//fname[res+1]='r';
	//fname[res+2]='c';
	////fname[0]='I';
	//fname[1]='m';
	////fname[2]='a';
	////fname[3]='g';
	////fname[4]='e';
	union mrc_h header_out;
		memset(&header_out.buff,0,1024); 
		header_out.header.mode=2;
		header_out.header.next=128*1024;
		header_out.header.nx=dimx;
		header_out.header.ny=dimy;
		header_out.header.nz=dimz;
		header_out.header.alpha=90.0f;
		header_out.header.beta=90.0f;
		header_out.header.gamma=90.0f;
		header_out.header.mx=dimx;
		header_out.header.my=dimy;
		header_out.header.mz=dimz;
		header_out.header.xlen=(float)ImagesizeInnm*10.0f;
		header_out.header.ylen=(float)ImagesizeInnm*10.0f;
		header_out.header.zlen=30.5f;//(float)dimz;
		header_out.header.nreal=32;
		header_out.header.mapc=1;
		header_out.header.mapr=2;
		header_out.header.maps=3;
		header_out.header.zorg=30.5f;
		//header_out.header.amax=imax;
		//header_out.header.amin=imin;
		//header_out.header.amean=average;
		header_out.header.nlabl=3;
		strncpy(header_out.header.labels[0],filename_short,80)  ;
				sprintf(header_out.header.labels[1],"Cs=%.2f um, delta=%.1f nm, dZ=%.1f nm.",Cs/1.0e03,delta,dZ,STRPRODUCTVER);
				sprintf(header_out.header.labels[2],"GPU TEM ver.%s %s %s (c) pust@lemoi.phys.dvgu.ru",STRPRODUCTVER,DATESTAMP,TIMESTAMP);
//strncpy(header_out.header.labels[1],filename_short,80)  ;
		union all_mrc_ext_h ext_header_out;
		memset(&ext_header_out.buff,0,1024*128);
		for(int i=0;i<dimz;i++){
			ext_header_out.ext_header[i].a_tilt = 0.0f;
			ext_header_out.ext_header[i].b_tilt = 0.0f;
			ext_header_out.ext_header[i].exp_time = 1.0f;
			ext_header_out.ext_header[i].appliedDefocus = 0.0f;
			ext_header_out.ext_header[i].binning = 1.0f;
			ext_header_out.ext_header[i].ht =HT* 1000.0f;
			ext_header_out.ext_header[i].defocus=defocusSeria[i];
			ext_header_out.ext_header[i].pixel_size=ImagesizeInnm/dimx*1e-09f;
			//printf("defocus=%0.3e\n",defocusSeria[i]);
			}
		//_getch();
		//for(int i=0;i<dimx*dimy;i++){
		// h_img_tmp1[i].x = (h_img_tmp[i] & 0xff) ;
		// h_img_tmp1[i].y = ((h_img_tmp[i]>>8) & 0xff) ;
		// h_img_tmp1[i].z = ((h_img_tmp[i]>>16) & 0xff) ;
		//	}
		WriteMRC<float>(fname, SeriaImage, header_out.header , ext_header_out);
		//free(SeriaImage);
		//free(h_img_tmp);
		//free(h_img_tmp1);
	}
cufftDoubleComplex *ReadImgAndParams(TCHAR* filename,float &pxa,float &pxb,int &dimx,int &dimy,float &thickness){
		  	struct stat results;
   
	if (stat(filename, &results) == 0){
			FILE *fp;
		//int size = (int)results.st_size;
		//unsigned char *data = (unsigned char *) malloc(size*sizeof(unsigned char));
		//int *HeaderSize=(int*)malloc(sizeof(int));
		//int *ParamSize=(int*)malloc(sizeof(int));
		//int *CommentSize=(int*)malloc(sizeof(int));
		//int *Nx=(int*)malloc(sizeof(int));
		//int *Ny=(int*)malloc(sizeof(int));
		//int *ComplexFlag=(int*)malloc(sizeof(int));
		//int *DataSize=(int*)malloc(sizeof(int));
		//int *Version=(int*)malloc(sizeof(int));
		//double *ThiknessDefocus=(double*)malloc(sizeof(double));
		//double *pxSizeX=(double*)malloc(sizeof(double));
		//double *pxSizeY=(double*)malloc(sizeof(double));
		int HeaderSize;
		int ParamSize;
		int CommentSize;
		int Nx;
		int Ny;
		int ComplexFlag;
		int DataSize;
		int Version;
		double ThiknessDefocus;
		double pxSizeX;
		double pxSizeY;

		//char *Comment=(char*)malloc(CommentSize[0]*sizeof(char));
		char Comment[2048];//=(char*)malloc(CommentSize[0]*sizeof(char));
		fopen_s(&fp,filename, "rb");
		if (!fp) {
			fprintf(stderr, "Error opening file '%s'\n", filename);
			return NULL;
		}
		size_t read = fread(&HeaderSize, sizeof(int),1, fp);
		printf("Read 'Header size', %d bytes\n",  HeaderSize);
		 read = fread(&ParamSize, sizeof(int),1, fp);
		printf("Read 'Param size', %d bytes\n",  ParamSize);
			 read = fread(&CommentSize, sizeof(int),1, fp);
		printf("Read 'Comment size', %d bytes\n",  CommentSize);
			 read = fread(&Nx, sizeof(int),1, fp);
		printf("Read 'Nx', %d px\n",  Nx);
			 read = fread(&Ny, sizeof(int),1, fp);
		printf("Read 'Ny', %d px\n",  Ny);
 			 read = fread(&ComplexFlag, sizeof(int),1, fp);
		printf("Read 'ComplexFlag', %d \n",  ComplexFlag);
 			 read = fread(&DataSize, sizeof(int),1, fp);
		printf("Read 'DataSize', %d \n",  DataSize);
 			 read = fread(&Version, sizeof(int),1, fp);
		printf("Read 'Version', %d \n",  Version);
 			 read = fread(&ThiknessDefocus, sizeof(double),1, fp);
		printf("Read 'ThiknessDefocus', %f \n",  ThiknessDefocus);
  			 read = fread(&pxSizeX, sizeof(double),1, fp);
		printf("Read 'pxSizeX', %f \n",  pxSizeX);
   			 read = fread(&pxSizeY, sizeof(double),1, fp);
		printf("Read 'pxSizeY', %f \n",  pxSizeY);
    			 read = fread(&Comment, sizeof(char),CommentSize, fp);
		printf("Read 'Comment', %s \n",  Comment);

		if	(ComplexFlag==0||DataSize<8) {
			printf("File is not a complex exit wave!\n");
			return NULL;
			}
		cufftDoubleComplex *data_d=(cufftDoubleComplex*)malloc(Nx*Ny*sizeof(cufftDoubleComplex));
		if	 (DataSize==8)   {
			cufftComplex *data_s=(cufftComplex*)malloc(Nx*Ny*sizeof(cufftComplex));
    		read = fread(data_s, sizeof(cufftComplex),Nx*Ny, fp);
			for(int i=0;i<Nx*Ny;i++){
				data_d[i].x=(double)data_s[i].x;
				data_d[i].y=(double)data_s[i].y;
				}
			free(data_s);
			}  else {
    			read = fread(data_d, sizeof(cufftDoubleComplex),Nx*Ny, fp);
			}

		fclose(fp);
		pxa=(float)pxSizeX;
		pxb=(float)pxSizeY;
		dimx=Nx;
		dimy=Ny;
		 thickness=(float)ThiknessDefocus;
		//free(HeaderSize) ;
		  return data_d;
		}   else {
		printf("Error get file %s info\n",filename);
		return NULL;
		}
	}