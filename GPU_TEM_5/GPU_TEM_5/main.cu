///********************************************************************
//*  GPU_TEM
//*  (c) pust@lemoi.phys.dvgu.ru
//*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h> 
#include <winuser.h>
#include <csetjmp>
#include <cstdio>

#include <direct.h>


#include <cuComplex.h>
//#include <cutil_inline.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

//#include <cutil_gl_inline.h>
#include <cuda_gl_interop.h>
// Utilities and timing functions
//#include <curand.h>    // includes cuda.h and cuda_runtime_api.h
#include <helper_functions.h>    // includes cuda.h and cuda_runtime_api.h
#include <timer.h>               // timing functions

// CUDA helper functions
#include <helper_cuda.h>         // helper functions for CUDA error check
#include <helper_cuda_gl.h>      // helper functions for CUDA/GL interop
#include "VersionNo.h"



#include "../../include/mrc.h"
#include "../../include/functions.h" 
#include "tem_gl.h"
#include "resource.h"

  static jmp_buf env;

bool FileNameShow=true;
HANDLE hStdout, hStdin; 

//TCHAR  filename[2048];
char path[2048];
char fname[2048];
char APP_path[2048];

char * pch;
HICON hIcon;

HINSTANCE hinst;            // handle to current instance 
HWND hwnd;                  // main window hand

//#define ClientWidth 800
//#define ClientHeight 600
//#include <test_gl_kernel.cu>

//#define REFRESH_DELAY 100
int REFRESH_DELAY=100;

char filename[4096]="d:\\VisualProfiler\\fe75b25.wav";

GLuint pbo = 0;     // OpenGL pixel buffer object
GLuint texid = 0;   // texture
int vx=0,vy=0;
//unsigned int timer = 0;


int fpsCount = 0;        // FPS count for averaging
int fpsLimit = 1;        // FPS limit for sampling
unsigned int frameCount = 0;
unsigned int *d_result;

float *SeriaImage=0;
//unsigned char *SeriaImage=0;

double Plank_h = 6.6256E-34;
float c_light = 2.9979E+08f;
float eV=1.602E-19f;
float voltage=300.0f;
float Cc=1.0f;
int ox, oy;
int buttonState = 0;
float OldZoom=1.0f;
float tx_old=0.0f,ty_old=0.0f;
bool changes=false;
bool NewFile2Open=false;
bool SeriaOn=false;
int SeriaCounter=0;
int SeriaEnd=0;
char fps[256];
float aspect=1.0f;
float DfStep=10.0f,DfStepSmall=1.0f;
string ver;
float dFOld=0.0;
float DefocusStart=-50.0f,DefocusStep=1.0f,DefocusSeriaCs=-5000.0;
char ErrorMessage[180];
bool ErrorOn=false;


void  OpenParamFile(char * filenamepath);


void initCudaBuffers(cufftComplex *h_img,int dimx,int dimy,int nproj);
	void* bitmap_fonts[7] = {
      GLUT_BITMAP_9_BY_15,
      GLUT_BITMAP_8_BY_13,
      GLUT_BITMAP_TIMES_ROMAN_10,
      GLUT_BITMAP_TIMES_ROMAN_24,
      GLUT_BITMAP_HELVETICA_10,
      GLUT_BITMAP_HELVETICA_12,
      GLUT_BITMAP_HELVETICA_18     
   };

   char* bitmap_font_names[7] = {
      "GLUT_BITMAP_9_BY_15",
      "GLUT_BITMAP_8_BY_13",
      "GLUT_BITMAP_TIMES_ROMAN_10",
      "GLUT_BITMAP_TIMES_ROMAN_24",
      "GLUT_BITMAP_HELVETICA_10",
      "GLUT_BITMAP_HELVETICA_12",
      "GLUT_BITMAP_HELVETICA_18"     
   };
  

void
print_bitmap_string(void* font, char* s)
{
   if (s && strlen(s)) {
      while (*s) {
         glutBitmapCharacter(font, *s);
         s++;
      }
   }
}
/************************************************************************/
/* Init CUDA                                                            */
/************************************************************************/
#if __DEVICE_EMULATION__

bool InitCUDA(void){return true;}

#else
bool InitCUDA(void)
{
	int count = 0;
	int i = 0;
	cudaError_t error;
	int devID = 0;
	cudaDeviceProp deviceProp;

	cudaGetDeviceCount(&count);
	if(count == 0) {
		fprintf(stderr, "There is no GPU device. Press any key.\n");
		_getch();
		return false;
	}

	for(i = 0; i < count; i++) {
		cudaDeviceProp prop;
		if(cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
			if(prop.major >= 1) {
				break;
			}
		}
	}
	if(i == count) {
		fprintf(stderr, "There is no GPU device supporting CUDA. Press any key.\n");
		_getch();
		return false;
	}
	
	cudaSetDevice(devID);

	error = cudaGetDevice(&devID);

	if (error != cudaSuccess)
	{
		printf("cudaGetDevice returned error code %d, line(%d)\n", error, __LINE__);
	}

	error = cudaGetDeviceProperties(&deviceProp, devID);

	if (deviceProp.computeMode == cudaComputeModeProhibited)
	{
		fprintf(stderr, "Error: device is running in <Compute Mode Prohibited>, no threads can use ::cudaSetDevice().\n");
		exit(EXIT_SUCCESS);
	}

	if (error != cudaSuccess)
	{
		printf("cudaGetDeviceProperties returned error code %d, line(%d)\n", error, __LINE__);
	}
	else
	{
		printf("GPU Device %d: \"%s\" with compute capability %d.%d\n\n", devID, deviceProp.name, deviceProp.major, deviceProp.minor);
	}
//	int	  devID;
	//devID = cutGetMaxGflopsDeviceId();
	//cudaSetDevice(1);
	//cudaSetDevice(0);
	//cudaGetDevice(&devID);
	//printf("CUDA initialized.%d\n",devID);
	//_getch();
	return true;
}

#endif
HWND GetConsoleHwnd(void)
   {
       #define MY_BUFSIZE 1024 // Buffer size for console window titles.
       HWND hwndFound;         // This is what is returned to the caller.
       char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
                                           // WindowTitle.
       char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
                                           // WindowTitle.

       // Fetch current window title.

       GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

       // Format a "unique" NewWindowTitle.

       wsprintf(pszNewWindowTitle,"%d/%d",
                   GetTickCount(),
                   GetCurrentProcessId());

       // Change current window title.

       SetConsoleTitle(pszNewWindowTitle);

       // Ensure window title has been updated.

       Sleep(40);

       // Look for NewWindowTitle.

       hwndFound=FindWindow(NULL, pszNewWindowTitle);

       // Restore original window title.

       SetConsoleTitle(pszOldWindowTitle);

       return(hwndFound);
   }
	HICON SetIcon(HWND hWnd, HICON hIcon, bool bBigIcon=true)
{
return (HICON)
SendMessage(hWnd, 
(UINT)WM_SETICON,
(WPARAM) bBigIcon ? ICON_BIG : ICON_SMALL,
(LPARAM) hIcon);
}
bool ShowOpenDialogMy(HWND HWndOwner, char *Title, 
    char *InitialDir, char *Filter,int iFlag,char *FileName) 
{ 
    FileName[0] = '\0';

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    //  OPENFILENAME 
    ofn.lStructSize = sizeof(OPENFILENAME); 
    ofn.hwndOwner = HWndOwner; 
    ofn.lpstrFile = FileName; 
    ofn.nMaxFile = MAX_PATH; 
    ofn.lpstrFilter = Filter; 
    ofn.nFilterIndex = 0; 
    ofn.lpstrTitle = Title; 
    ofn.lpstrInitialDir = InitialDir; 
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;  
	return GetOpenFileName(&ofn);
    //if ( iFlag == FILE_LOAD )return GetOpenFileName(&ofn);
    //else if ( iFlag == FILE_SAVE )return GetSaveFileName(&ofn); 
}
void LoadNewImage(){
	//cufftComplex * image_c=0;
	//TCHAR  filename[2048];
	HWND test=GetConsoleHwnd();
	ShowOpenDialogMy(test, "Open wave complex function", "", "wav\0*.wav\0All\0*.*\0",0,filename);
	NewFile2Open=true;
	//glutLeaveMainLoop();
	 //longjmp(env, 1);

	// exit(0);
	//	char * pch;
 //   pch=strrchr(filename,'\\');

	//printf("test %s %d\n",path,pch-path+1);


	//strncpy(path,filename,pch-filename+1);
	//	   path[pch-filename+2]='\0';

	//float * image=0;
	//cufftComplex *image_c=0;
	//int dimx,dimy,nproj;

	////image=ReadMRC2Float(filename,dimx,dimy,nproj);
	///*image_c=ReadMRC2Complex(filename,dimx,dimy,nproj);
	//if (image_c == NULL) {
	//		fprintf(stderr,"End of story\n");
	//			return ;
	//		}
	//int size=dimx*dimy*nproj*sizeof(cufftComplex);
	//checkCudaErrors( cudaMemcpy( d_img, image_c, size, cudaMemcpyHostToDevice));
	//for (int i=0;i<nproj;i++)
	//	CreateFFT(d_img+i*dimx*dimy,dimx,dimy);

	//free(image_c);*/

	//cufftDoubleComplex *image_d_c=0;
	////image=ReadMRC2Float(filename,dimx,dimy,nproj);
	////image_c=ReadMRC2Complex(filename,dimx,dimy,nproj);
	////HT=300;
	////a=70,b=70;
	//nproj=1;
	//image_d_c=ReadWavAndParams(filename,a,b,c,alpha,beta,gamma,HT,dimx,dimy,thickness);
	//if (image_d_c == NULL) {
	//		fprintf(stderr,"End of story\n");
	//			return ;
	//		}
	//float Eng=HT*eV*1000.0f;
	//float E0=511000*eV;
	//Lambda = ( Plank_h * c_light / sqrt( 2 * Eng * E0 + Eng * Eng ) ) * 1E9;
	//ux=1.0f/a*10.0f;
	//uy=1.0f/b*10.0f;
	////Df=-41.0f;
	//H_Cc=Cc*(3.5e-06)*(1-Eng/E0)/(1+Eng/(2*E0));
	//Df=-1.2f*sqrt(Cs*Lambda);
	//tetta=conv/(Lambda*sqrt(log(2.0f))) ;

	//image_c = (cufftComplex*)malloc(dimx*dimy*sizeof(cufftComplex));
	//memset(image_c,0,dimx*dimy*sizeof(cufftComplex));
	//
	//double pix_per_nm;
	//pix_per_nm=1024.0/max(a,b);
	//width=(int)(a*pix_per_nm);
	//height=(int)(b*pix_per_nm);
	//for(int i=0;i<dimx*dimy;i++)
	//	{
	//	image_c[i].x=(float)image_d_c[i].x;
	//	image_c[i].y=(float)image_d_c[i].y;
	//	}
	//int size=dimx*dimy*nproj*sizeof(cufftComplex);
	//
	////checkCudaErrors( cudaMemcpy( d_img, image_c, size, cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaFree(d_img));
	//checkCudaErrors(cudaFree(d_tmp_c));
	//checkCudaErrors(cudaFree(d_result_tex));
	////checkCudaErrors(cudaFree(cu_array));
 //   initCudaBuffers(image_c,dimx,dimy,nproj);
	//cufftPlan2d(&plan, dimx,dimy, CUFFT_C2C);
	//for (int i=0;i<nproj;i++)
	//	CreateFFT(d_img+i*dimx*dimy,dimx,dimy);

	//free(image_d_c);
	//free(image_c);



	}
void CreateConsole(){
	system("cls");
	int Mag=(int)(Zoom*(((float)1024/(float)96)*0.0254f)/(a*1e-09f));
	COORD CursorPosition;
	CursorPosition.X=0;
	CursorPosition.Y=0;
	SetConsoleCursorPosition(hStdout,CursorPosition);
	int devID=0;
	cudaGetDevice(&devID);
	for(int i=0;i<25;i++)
	 printf("                                                                              \n");
	SetConsoleCursorPosition(hStdout,CursorPosition);
	//printf("File loaded: %s \n",filename);
	printf("Commands:                                                                     \n");
	printf("d - image or difraction; c - CTF or image; r - real or imaginary part of CTF   \n");
	printf("s - save data; i - save image; 8/2 +- 10 nm defocus; 4/6 +- 1 nm defocus       \n");
	printf("7/1 - increase decrease contrast for difraction; 9/3 - inc/dec apperture radius \n");
	//printf("9/3 - increase decrease apperture radius                                       \n");
	printf("5 - restore defaults; up(+shift) or down(+shift) - change Cs                  \n");
	printf("a - insert/remove apperture; left or right + Ctrl change image size            \n");
	printf("Grey + or - magnification. Up(+ctrl) or down(+ctrl) - change Exp.time         \n");
	printf("Mouse left/middle/right - shift/magnification/menu                            \n\n");
	printf("Esc - exit                                                                    \n\n");
	printf("Current parameters:                                                            \n");
	printf("Image size %.1f nm; DF= %.0f nm Cs=%.1e dZ=%.0f                              \n",ImagesizeInnm,Df,Cs,dZ);
	printf("Mode: %s                                                \n",ImageOrCTF==1?"image":RealCTFOrComplexCTF==0?"Phase CTF":"Real CTF");
	printf("Apperture %s , radius %.3f 1/nm (d= %.2f nm), Ax=%.1f, Ay=%.1f            \n",Apperture==1?"in":"out",AppertureRadius,1.0f/AppertureRadius,Ax,Ay);
	printf("Contrast for difraction or CTF %.3f               \n",Contrast);
	//printf("img_min %.3e, img_max %.3e\n",img_min,img_max);
	printf("Mag %.3f %s                                      \n",(Mag>1.0e6)?(Mag/1.0e6):(Mag>1.0e3)?(Mag/1.0e3):(int)(Mag),(Mag>1.0e6)?"Mx":(Mag>1.0e3)?"kx":"x");
	printf("Zoom %.3e %.3e Exposition %.2fs                                \n\n",Zoom,ZoomImage,(float)REFRESH_DELAY/1000.0f);
	//char * pch;
    //pch=strrchr(path,'\\');
 	//printf("(c) pust@lemoi.phys.dvgu.ru gpuid=%d, dimx %d dimy %d version %s  %s %s      ",devID,dimx,dimy,STRPRODUCTVER,DATESTAMP,TIMESTAMP);
 	printf("(c) pust@lemoi.phys.dvgu.ru, x %d y %d ver. %s %s %s",dimx,dimy,STRPRODUCTVER,DATESTAMP,TIMESTAMP);
		if (FileNameShow) {
		printf("\nfile name=%s",filename);
		}
		
 	//printf("(c) pust@lemoi.phys.dvgu.ru %s",ver);
	 // adasd
	//printf("test %.3e\n",a*1e-09f);
	//printf("test %d\n",(int)(((float)1024/(float)96)*0.0254f*Zoom/(a*1e-09f))/(1000000));
	
	}
void init(){
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, 800.0, 0.0, 800.0);
};


int xx=400,yy=300;
void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
        buttonState |= 1<<button;
    else if (state == GLUT_UP)
        buttonState = 0;

    ox = x; oy = y;


    glutPostRedisplay();
}


//void mouse(int button, int state, int x, int y){
//    xx=x; yy=y;
//    if((button==0)&(state==0))
//        glClearColor((float)(rand()%100)/100.0, (float)(rand()%100)/100.0, (float)(rand()%100)/100.0, 0.0);
//        
//	glutPostRedisplay();
//}   

void motion(int x, int y)
{
if (ImageOrCTF==1){
    float dx, dy;
    dx = (float)x - ox;
    dy = (float)y - oy;

    if (buttonState & 1) {
        // left = translate
        tx -= dx/Zoom;
        ty -= dy/Zoom;
    }
    else if (buttonState & 2) {
        // middle = zoom
		if (ImageOrDifr==1) {
		OldZoom=Zoom;
        Zoom -= (dy / 1000.0f);
		if (Zoom<1.0e-4f)
			Zoom=OldZoom;
		ZoomImage=Zoom;

		changes=true;
			}

    }

    ox = x; oy = y;
	//changes=true;

    glutPostRedisplay();
	}
}
//
void computeFPS()
{
    frameCount++;
    fpsCount++;
    //if (fpsCount == fpsLimit-1) {
    //  //  g_Verify = true;
    //}
    if (fpsCount == fpsLimit) {
        float ifps = 1.f / (sdkGetAverageTimerValue(&timer) / 1000.f);
		sprintf(fps, "%s (HT=%.f kV, w=%.1f %s(%d px), h=%.1f %s(%d px), Cs=%.3fmm, Df=%.1f nm, dZ=%.0f nm %s): %3.1f fps v.%s exp: %.1fs noise %d","GPU TEM simulation",
			HT,ImageOrCTF==1?a/Zoom:Zoom/a,ImageOrCTF==1?"nm":"1/nm",width,ImageOrCTF==1?b/Zoom:Zoom/b,ImageOrCTF==1?"nm":"1/nm",height,Cs/1000000,Df,dZ,ImageOrCTF==1?"image":RealCTFOrComplexCTF==0?"Im CTF":"Re CTF",ifps,STRPRODUCTVER,(float)REFRESH_DELAY/1000.0f,NoiseLevel);  

        glutSetWindowTitle(fps);
		//SetIcon( GetConsoleHwnd(), hIcon);

        fpsCount = 0; 
        sdkResetTimer(&timer);  

      //  AutoQATest();
    }
}
// display results using OpenGL
void display()
{
    sdkStartTimer(&timer);  

    // execute filter, writing results to pbo
    checkCudaErrors(cudaGLMapBufferObject((void**)&d_result, pbo));
    //CreateTexture
	DrawImage(d_img, d_result, dimx, dimy, slice);

    checkCudaErrors(cudaGLUnmapBufferObject(pbo));

    // load texture from pbo
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
    glBindTexture(GL_TEXTURE_2D, texid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dimx, dimy, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    // display results
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f(1, 0);
    glTexCoord2f(1, 0); glVertex2f(1, 1);
    glTexCoord2f(0, 0); glVertex2f(0, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    //if (g_CheckRender && g_CheckRender->IsQAReadback() && g_Verify) {
    //    // readback for QA testing
    //    printf("> (Frame %d) Readback BackBuffer\n", frameCount);
    //    g_CheckRender->readback( width, height );
    //    g_CheckRender->savePPM(sOriginal[g_Index], true, NULL);
    //    if (!g_CheckRender->PPMvsPPM(sOriginal[g_Index], sReference[g_Index], MAX_EPSILON_ERROR, THRESHOLD)) {
    //        g_TotalErrors++;
    //    }
    //    g_Verify = false;
    //}
	if (FileNameShow) {
		glColor3f(0.0,1.0,0.0);
		glRasterPos2f(  0.01f, (float)(height-15)/height);
		//glRasterPos2f(  0.0, (float)dimy/2);
		print_bitmap_string(bitmap_fonts[6],filename_short);
   //print_bitmap_string(bitmap_fonts[6],filename);
		glColor3f(1.0,1.0,1.0);
		}
	if (SeriaOn) {
		glColor3f(1.0,0.0,0.0);
		glRasterPos2f(  0.01f, (float)(height-40)/height);
		//glRasterPos2f(  0.0, (float)dimy/2);
		print_bitmap_string(bitmap_fonts[6],"Wait for focal series!");
   //print_bitmap_string(bitmap_fonts[6],filename);
		glColor3f(1.0,1.0,1.0);
	   	}
	if (ErrorOn) {
		glColor3f(0.5,0.0,0.0);
		glRasterPos2f(  0.01f,  (float)(height-40)/height);
		//glRasterPos2f(  0.0, (float)dimy/2);
		print_bitmap_string(bitmap_fonts[6],ErrorMessage);
		//print_bitmap_string(bitmap_fonts[6],filename);
		glColor3f(1.0,1.0,1.0);
	}
    glutSwapBuffers();
	if (changes) {
		CreateConsole();
		changes=false;
		}
    sdkStopTimer(&timer);  
	seed=(int)(sdkGetAverageTimerValue(&timer)*1000.0f);

    computeFPS();

}
void idle()
{
    glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
        case 27:
            exit(0);
			//glutLeaveMainLoop ();
            break;
		case 'v':
			if (!CTFoff){
			 CTFoff=true;
			} else {
				CTFoff=false;

			}
			break;
		case '0':
			glutPositionWindow(0 , 0);
			break;
        case '-':
        //    slice-=1;
          //  if (slice < 0) slice = nproj-1;
			//RecalcuateMinMax=1;
			if (ImageOrDifr==1) {
				Zoom=ZoomImage;
				OldZoom=Zoom;
				Zoom/=1.3f;
				if (Zoom<1.0e-4f)
					Zoom=OldZoom;	
				ZoomImage=Zoom;} else { Zoom=1.0f; }

			//glPixelZoom(Zoom,Zoom);

            break;
            
        case '+':
			if (ImageOrDifr==1) {Zoom=ZoomImage; Zoom*=1.3f;ZoomImage=Zoom;} else { Zoom=1.0f; }
			//glPixelZoom(Zoom,Zoom);
            //slice+=1;
			//RecalcuateMinMax=1;
            //if (slice >= nproj) slice = 0;
			break;
			
		case '6':
			if (ImageOrDifr==1) {Df+=DfStepSmall;} else {Ax+=1.0f;}
			break;

		case '4':
			if (ImageOrDifr==1) {Df-=DfStepSmall;} else {Ax-=1.0f;}
			break; 
		case '8':
			if (ImageOrDifr==1) {Df+=DfStep;} else {Ay-=1.0f;}
			break;

		case '2':
			if (ImageOrDifr==1) {Df-=DfStep;} else {Ay+=1.0f;}
			break;
		case '5':
			if	(Cs>0) {
			Df=-1.2f*sqrt(Cs*Lambda);
				} else {
				 Df=1.2f*sqrt(abs(Cs)*Lambda);
				}
			Contrast=1.0f;
			Zoom=1.0f;
			tx=0;
			ty=0;
			Ax=0.0f;
			Ay=0.0f;
			dZ=0.0;
			//glPixelZoom(Zoom,Zoom);
			glRotated(0.0f,0.0,0.0,0.0);
			//Cs=0.6e+6;
			break;
		case 'c':
			if (ImageOrCTF==1) {
			ImageOrCTF=0;
			OldZoom=Zoom;
			Zoom=2.0f;
			tx_old=tx;
			ty_old=ty;
			tx=0.0f;
			ty=0.0f;
			glutReshapeWindow(dimx,dimy);
				} else  {
			ImageOrCTF=1;
			Zoom=OldZoom;
			tx=tx_old;
			ty=ty_old;
			glutReshapeWindow(width,height);

				}
			//RecalcuateMinMax=1;
			break;
		case 'r':
			if (RealCTFOrComplexCTF==1) {
			RealCTFOrComplexCTF=0;
				} else  {
			RealCTFOrComplexCTF=1;
				}
			//RecalcuateMinMax=1;
			break;
		case 'd':
			if (ImageOrDifr==1) {
			ImageOrDifr=0;
			ZoomImage=Zoom;

			Zoom=1.0f;
	  				} else  {
			ImageOrDifr=1;
			Zoom=ZoomImage;
				}
			//RecalcuateMinMax=1;
			break;
		case '7':
			Contrast*=1.3f;
			break;

		case '1':
			Contrast*=0.7f;
			break;
		case '9':
			AppertureRadius*=1.3f;
			break;

		case '3':
			AppertureRadius*=0.7f;
			break;
		case 'a':
			if (Apperture==1) {
			Apperture=0;
				} else  {
			Apperture=1;
				}
			//RecalcuateMinMax=1;
			break;
		case 'x':
			//glutReshapeWindow(width,995);
			glRotated(10.0f,1.0,1.0,0.0);

			break;
		case 'l':
			LoadNewImage();
			glutLeaveMainLoop();
			break;
		case 's':
			pch=strrchr(filename,'\\');

			//printf("test %s %d\n",path,pch-path+1);


			strncpy(path,filename,pch-filename+1);
			path[pch-filename+2]='\0';
		SaveImage(strcat (path,"0_d_tmp_c_slice.mrc"),d_tmp_c,dimx,dimy);
		//SaveImage("d:\\image\\mrc\\0_d_tmp_c_slice.mrc",d_tmp_c,dimx,dimy);
		//SaveImage("d:\\image\\mrc\\0_d_img_slice.mrc",d_img+slice*dimx*dimy,dimx,dimy);
		printf("Original image saved slice %d\n",slice);
			break;
		case 'i':
					
			pch=strrchr(filename,'\\');

			//printf("test %s %d\n",path,pch-path+1);


		strncpy(path,filename,pch-filename+1);
		path[pch-filename+2]='\0';
		//strncpy(fname,filename+pch+1,filename-pch);
		//fname[filename-pch+2]='\0';
		substr(fname,filename,(int)(pch - filename + 1),(int)strlen(filename)-(int)(pch - filename));
		//SaveImage1("d:\\image\\mrc\\0_buffer_slice.mrc",d_result,dimx,dimy);
		//SaveImage1(strcat (path,"0_buffer_slice.mrc"),d_result,dimx,dimy);
		SaveImage1(strcat (path,fname),d_result,dimx,dimy);
		printf("Buffer image saved\n");
			break;
		case 'f':
			FileNameShow=!FileNameShow;
			break;
		case 'p':
			OpenParamFile(filename);
			//printf("defocus seria cs= %.3e\n",DefocusSeriaCs);
			//_getch();
			break;
		case 'n':

			if (NoiseOn==1) {
			NoiseOn=0;
				} else  {
			NoiseOn=1;
				}
			break;
		case 'w':
		   //SeriaEnd=4;
		   //	PROCESS_MEMORY_COUNTERS info;
			//GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
			//MessageBox(NULL,(size_t)info.PeakWorkingSetSize, "Memory alert", MB_OK);
		  //long seriaSize,memSize;
		   //seriaSize=(long)dimx*(long)dimy*(long)SeriaEnd;
		   //memSize=(long)1024*(long)1024*(long)2048-(long)info.PeakWorkingSetSize;
			//MessageBox(NULL, "Focal series stack to large!", "Memory alert", MB_OK);
			//printf(" 0 seria memsize= %d programm memory %d\n",dimx*dimy*SeriaEnd, info.PeakWorkingSetSize+dimx*dimy*SeriaEnd);
			//_getch();
		   //if   (seriaSize<memSize) {
			//SeriaEnd=81;

				//SeriaImage=(unsigned char *)malloc(dimx*dimy*SeriaEnd*sizeof(unsigned char));
				SeriaImage=(float *)malloc(dimx*dimy*SeriaEnd*sizeof(float));
				if	(SeriaImage==NULL) {
   						 SeriaOn=false;
						 char mem[2048];
						 sprintf(mem,"Focal series stack (%.2f Mbytes) exeed free memory size! /n dx=%d, dy=%d, dz=%d",sizeof(float)*(float)((float)dimx*(float)dimy*(float)SeriaEnd)/(1024*1024),dimx,dimy,SeriaEnd); 
						 MessageBox(NULL, mem, "Memory alert", MB_OK);
					} else {
						//memset(SeriaImage,0,dimx*dimy*SeriaEnd);
						dFOld=Df;
						Df=DefocusStart;
						DfStepSmall=DefocusStep;
						Cs=DefocusSeriaCs;
						SeriaCounter=0;
						SeriaOn=true;
					}

				 //SeriaImage[0]=0;
				 //memset()
			//printf(" 1 memsize= %d , free memory %d\n",info.PeakWorkingSetSize,(long)(1024*1024)*2048-info.PeakWorkingSetSize);
			//_getch();

			 
			break;
		case 'W':
						 MessageBox(NULL, "W", "Memory alert", MB_OK);

			break;
        default:
            break;
    }
	if(key!=0)
		ErrorOn=false;

   // printf("sigma = %f\n", sigma);
	CreateConsole();
    glutPostRedisplay();
}
void processSpecialKeys(int key, int x, int y) {
int specialKey = glutGetModifiers();
//printf("Skey %d %d\n",key,specialKey);

	switch(key) {
		case GLUT_KEY_F1:
			
			ShellExecute(GetDesktopWindow(), "open", "notepad", NULL, NULL, SW_SHOWNORMAL); 

		case GLUT_KEY_LEFT:

			if (   specialKey==GLUT_ACTIVE_CTRL) {
				height=height-10;
				width=width-(int)(10*aspect);
				}
	 			break;
		case GLUT_KEY_RIGHT:

			if (   specialKey==GLUT_ACTIVE_CTRL) {
				height=height+10;
				width=width+(int)(10*aspect);
				}


			break;

		case GLUT_KEY_UP:

			 switch(specialKey) {
				 case GLUT_ACTIVE_SHIFT:
					 Cs+=10000;
					 break;
				 case GLUT_ACTIVE_ALT:
					 dZ+=1.0;
					 break;
				 case GLUT_ACTIVE_CTRL:
					 REFRESH_DELAY+=200;
					 if (REFRESH_DELAY>10000)
						 REFRESH_DELAY=10000;
					 break;
				 default :
					 Cs+=100000;
				 }
	/*		if (specialKey==GLUT_ACTIVE_SHIFT) {
				  Cs+=10000;
			
				} else {
			if (specialKey==GLUT_ACTIVE_ALT)
				{
					dZ+=1.0;

				} else {
				  Cs+=100000;
				}
				}*/
			//if	   (binaryLevel>= img_max)
			//	   binaryLevel=	img_max;
			//changes=true;
			break;
				 case GLUT_KEY_DOWN:
				 switch(specialKey) {
				 case GLUT_ACTIVE_SHIFT:
					 Cs-=10000;
					 break;
				 case GLUT_ACTIVE_ALT:
					 dZ-=1.0;
					 break;
				 case GLUT_ACTIVE_CTRL:
					 REFRESH_DELAY-=200;
					 if (REFRESH_DELAY<100)
						 REFRESH_DELAY=100;
					 break;
				 default :
					 Cs-=100000;
				 }
			//if (   specialKey==GLUT_ACTIVE_SHIFT) {
			//	  Cs-=10000;
			//
			//	} else {
			//
			//		if (specialKey==GLUT_ACTIVE_ALT)
			//		{
			//		dZ-=1.0;
			//	
			//		} else {
			//	  Cs-=100000;
			//	}
			//
			//	}

			//if	   (binaryLevel<= img_min)
			//	   binaryLevel=	img_min;
			//changes=true;
				 // Cs-=100000;

			break;
	}
		CreateConsole();
   // glutPostRedisplay();

	}
void TimeEvent(int i) {
	if (SeriaOn ) {
		//SaveImage1(strcat (path,fname),d_result,dimx,dimy);
		//CopyByteImageToSeria(SeriaImage,d_result,dimx,dimy,SeriaCounter);
		CopyFloatImageToSeria(SeriaImage,d_result,dimx,dimy,SeriaCounter);
		defocusSeria[SeriaCounter]=Df*1.0e-09f;
  		keyboard('6', 0, 0);
		SeriaCounter++;
		if (SeriaCounter==SeriaEnd) {
		   SeriaOn=false;
		   keyboard('4', 0, 0);
	   		pch=strrchr(filename,'\\');
			strncpy(path,filename,pch-filename);
			path[pch-filename+1]='\0';
  			//substr(fname,filename,(int)(pch - filename + 2),(int)strlen(filename)-(int)(pch - filename)+1);
			char fname1[4096];
			sprintf(fname1,"\\float_dF_%.0fA_Cs_%.0f_um_dZ_%.0f_nm_%s",DfStepSmall*10.0,Cs/1.0e03,dZ,filename_short);
			//sprintf(fname1,"\\byte_dF_%.0fA_Cs_%.0f_um_dZ_%.0f_nm_%s",DfStepSmall*10.0,Cs/1.0e03,dZ,filename_short);
			//SaveByteImageSeria(SeriaImage,strcat (path,fname1),dimx,dimy,SeriaCounter);
			SaveFloatImageSeria(SeriaImage,strcat (path,fname1),dimx,dimy,SeriaCounter);
			free(SeriaImage);
			DfStepSmall=1.0f;
			Df=dFOld;
			Cs=-10000.0;
			}
		}
	if(NoiseOn==1) {
		cudaMemcpy(d_tmp_show,d_tmp_a,dimx*dimy*sizeof(cufftComplex),cudaMemcpyDeviceToDevice);
		cudaMemset(d_tmp_a,0,dimx*dimy*sizeof(cufftComplex));
	//	//cudaMemset(d_result,0,dimx*dimy*sizeof(uint));
	//	cudaMemset(d_tmp_c,0,dimx*dimy*sizeof(cufftComplex));
					//REFRESH_DELAY=1000;
	} else {
					REFRESH_DELAY=100;

	}
	glutReshapeWindow(width,height);
	 //printf("\t timer %d\n",i);
	glutTimerFunc( REFRESH_DELAY, TimeEvent,1);
    glutPostRedisplay();
	}
void reshape(int x, int y)
{
    glViewport(0, 0, x, y);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0); 
	//glutReshapeWindow(width,height);
	  //gluPerspective(45.0, (float)width / (float)height, 0.1, 100.0);
//	if (y==0)
//y=1; // avoid div by 0
//glViewport(0, 0, x, y);
//glMatrixMode(GL_MODELVIEW);
//glLoadIdentity();
//if (y<=x)
//glOrtho(0, 250, 0, 250*y/x, 1.0, -1.0);
//else
//glOrtho(0, 250*y/x, 0, 250, 1.0, -1.0);
//glMatrixMode(GL_PROJECTION);
//glLoadIdentity();
	vx=x;
	vy=y;

}

void mainMenu(int i)
{
    keyboard(i, 0, 0);
}
void NMenu(int i)
{
	if (i<=0) {
		switch( i) {
		case -1:
			REFRESH_DELAY+=200;
			if (REFRESH_DELAY>10000)
				REFRESH_DELAY=10000;

			break;
		case -2:
			REFRESH_DELAY-=200;
			if (REFRESH_DELAY<100)
				REFRESH_DELAY=100;
			break;
		case -3:
			NoiseLevel+=5;
			if (NoiseLevel>100)
				NoiseLevel=100;
			break;
		case -4:
					NoiseLevel-=5;
					if (NoiseLevel<5)
						NoiseLevel=5;
					break;
		}
	


	} else {
		keyboard(i, 0, 0);
	}
}
void CsMn(int i){
	//int j;
	switch(i){
	case 1:
		//j=	GLUT_KEY_UP;
		//processSpecialKeys(j, 0, 0);
		Cs+=100000;
		break;
	case 2:
		//j=GLUT_KEY_DOWN;
		//processSpecialKeys(j, 0,0);
		Cs-=100000;

		break;
	case 3:
		Cs+=10000;
		break;
	case 4:
		Cs-=10000;
		break;
	}

}
void SubMenudZ(int i){
	switch(i){
	case 1:
		dZ+=1.0;
		break;
	case 2:
		dZ-=1.0;
		break;
	case 3:
		dZ+=10.0;
		break;
	case 4:
		dZ-=10.0;
		break;}
}
void initMenus()
{
	GLint NoiseMenu,FocusMenu,CsMenu,dZMenu;
	NoiseMenu= glutCreateMenu(NMenu);
	glutAddMenuEntry("Noise on/off   [n]", 'n');
	glutAddMenuEntry("Exposition + [Ctrl+Up]", -1);
	glutAddMenuEntry("Exposition - [Ctrl+Down]", -2);
	glutAddMenuEntry("Noise + []", -3);
	glutAddMenuEntry("Noise - []", -4);
	FocusMenu= glutCreateMenu(mainMenu);
	glutAddMenuEntry("Focus +10 nm   [8]", '8');
	glutAddMenuEntry("Focus -10 nm   [2]", '2');
	glutAddMenuEntry("Focus +1 nm    [6]", '6');
	glutAddMenuEntry("Focus -1 nm    [4]", '4');
	glutAddMenuEntry("Zoom in        [+]", '+');
	glutAddMenuEntry("Zoom out       [-]", '-');
	CsMenu=glutCreateMenu(CsMn);
	glutAddMenuEntry("Cs +0.1 mm   [Up]", 1);
	glutAddMenuEntry("Cs -0.1 mm   [Down]", 2);
	glutAddMenuEntry("Cs +0.01 mm  [Shft+Up]", 3);
	glutAddMenuEntry("Cs -0.01 mm  [Shft+Down]", 4);
	//glutAddMenuEntry("Noise and expo [n]", 'n');
	dZMenu=glutCreateMenu(SubMenudZ);
	glutAddMenuEntry("Z +1 nm   [Alt+Up]", 1);
	glutAddMenuEntry("Z -1 nm   [Alt+Down]", 2);
	glutAddMenuEntry("Z +10 nm  []", 3);
	glutAddMenuEntry("Z -10 nm  []", 4);

	glutCreateMenu(mainMenu);
    glutAddMenuEntry("Reset image    [5]", '5');
	glutAddMenuEntry("CTF            [c]", 'c');
    glutAddMenuEntry("CTF off        [v]", 'v');
	glutAddMenuEntry("File name show [f]", 'f');
	glutAddMenuEntry("Defocus seria  [w]", 'w');
	glutAddMenuEntry("Df params read [p]", 'p');
	glutAddSubMenu("Focus & Zoom",FocusMenu);
	glutAddSubMenu("Z",dZMenu);
	glutAddSubMenu("Cs",CsMenu);
	glutAddSubMenu("Noise",NoiseMenu);
    glutAddMenuEntry("Quit           [esc]", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void initGL(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	 vx=glutGet (GLUT_SCREEN_WIDTH);
	 vy=glutGet (GLUT_SCREEN_HEIGHT);
	//width=width/2;
	//height=height/2;
	glutInitWindowSize(width, height);
	 //window_x = (glutGet (GLUT_SCREEN_WIDTH) - window_width)/2;
    //window_y = (glutGet (GLUT_SCREEN_HEIGHT) - window_height)/2;
	//glutInitWindowPosition(1850, 0);  

    glutCreateWindow("TEM GPU");
	//glutSetIconTitle("microsope.ico");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
	glutSpecialFunc(processSpecialKeys);
	glutTimerFunc( REFRESH_DELAY, TimeEvent,1);
	initMenus();
   // printf("Press '+' and '-' to change filter width\n");
   // printf("0, 1, 2 - change filter order\n");

    glewInit();
    if (!glewIsSupported("GL_VERSION_2_0 GL_ARB_vertex_buffer_object GL_ARB_pixel_buffer_object")) {
        fprintf(stderr, "Required OpenGL extensions missing.");
        cudaThreadExit();
        exit(-1);
    }
}
void initCudaBuffers(cufftComplex *h_img,int dimx,int dimy,int nproj)
{
    unsigned int size = dimx * dimy * nproj*sizeof(cufftComplex);
    unsigned int sizef = dimx * dimy * nproj*sizeof(float);

    // allocate device memory
    checkCudaErrors( cudaMalloc( (void**) &d_img, size));
    checkCudaErrors( cudaMalloc( (void**) &d_tmp_c, dimx * dimy * sizeof(cufftComplex)));
    checkCudaErrors( cudaMalloc( (void**) &d_tmp_show, dimx * dimy * sizeof(cufftComplex)));
    checkCudaErrors( cudaMalloc( (void**) &d_tmp_a, dimx * dimy * sizeof(cufftComplex)));
    checkCudaErrors( cudaMalloc( (void**) &d_result_tex, sizef));
	checkCudaErrors( cudaMemset(d_tmp_a,0,dimx*dimy*sizeof(cufftComplex)));
	checkCudaErrors( cudaMemset(d_tmp_show,0,dimx*dimy*sizeof(cufftComplex)));

    checkCudaErrors( cudaMemcpy( d_img, h_img, size, cudaMemcpyHostToDevice));

     sdkCreateTimer( &timer);
	 ///////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//free(h_img);

	tex_image_2d.filterMode = cudaFilterModeLinear;
	//tex_image_2d.filterMode = cudaFilterModePoint;
	//tex_image_2d.addressMode[0] = cudaAddressModeClamp; 
	//tex_image_2d.addressMode[1] = cudaAddressModeClamp; 
	tex_image_2d.addressMode[0] = cudaAddressModeWrap; 
	tex_image_2d.addressMode[1] = cudaAddressModeWrap; 
	//tex_image_2d.addressMode[1] = cudaAddressModeWrap; 

	//tex_image_2d.normalized = true;

	checkCudaErrors( cudaMallocArray( &cu_array, &channelDesc, dimx, dimy )); 
	checkCudaErrors( cudaBindTextureToArray( tex_image_2d, cu_array, channelDesc));
	/*int BLOCK_DIM=16;
		dim3 grid(dimx / BLOCK_DIM, dimy / BLOCK_DIM, 1);
		dim3 threads(BLOCK_DIM, BLOCK_DIM, 1);
			setup_kernel<<<grid,threads>>>(devStates,(unsigned long)time(NULL),dimx,dimy);
*/
}
void cleanup()
{
     sdkDeleteTimer( &timer);
    /*if (!h_img) {
	    free(h_img);
	}*/

    checkCudaErrors(cudaFree(d_img));
	
    //checkCudaErrors(cudaFree(devStates));

    //if (!runBenchmark) {
        if (pbo) {
            checkCudaErrors(cudaGLUnregisterBufferObject(pbo));    
            glDeleteBuffersARB(1, &pbo);
        }
        if (texid) {
            glDeleteTextures(1, &texid);
        }
    //}
	cufftDestroy(  plan );
	//printf("cleanup\n");
	//_getch();
    /*if (g_CheckRender) {
        delete g_CheckRender; g_CheckRender = NULL;
    }*/
}
void initGLBuffers()
{
    // create pixel buffer object to store final image
    glGenBuffersARB(1, &pbo);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dimx*dimy*sizeof(GLubyte)*4, h_img, GL_STREAM_DRAW_ARB);

    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    checkCudaErrors(cudaGLRegisterBufferObject(pbo));

    // create texture for display
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, dimx, dimy, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}



//bool getinfo()
//{
//    string temp;
//    bool bResult = false;
//    DWORD size;
//    DWORD dummy;
//    char filename[ 2048 ];
//    unsigned int len;
//    GetModuleFileName( NULL, filename, 2047 );
//    size = GetFileVersionInfoSize( filename, &dummy );
//	printf("size %d\n",size);
//	printf("file name %s\n", filename);
//    if( size == 0 )
//    {
//        ver = "No Version Information!";
//        return true;
//    }
//    char* buffer = new char[ size ];
//    VS_FIXEDFILEINFO* data = NULL;
//    if( buffer == NULL ){ return true; }
//    bResult = GetFileVersionInfo( filename, 0, size, (void*)buffer );
//    if( !bResult )
//    {
//        ver = "error"; // STRLASTERROR is a custom macro
//        return true;
//    }
//    bResult = VerQueryValue( buffer, "\\", (void**)&data, &len );
//    if( !bResult || data == NULL || len != sizeof(VS_FIXEDFILEINFO) )
//    {
//        ver = "Could Not Retrieve Values!";
//        return true;
//    }
//    // here I would extract the needed values
//	ver="OK";
//    delete[] buffer;
//    return true;
//	}
void  OpenParamFile(char * filenamepath){
		
	char *pch1=strrchr(filenamepath,'\\');
	char path1[4096];
	//size_t found;
	strncpy(path1,filenamepath,pch1-filenamepath);

	path1[pch1-filenamepath]='\0';
	      string textLine;
		 // printf("path='%s'\n",path1);
		 // _getch();
      // try to open a file
	  //ifstream ifs("D:\\model\\amoph_complex\\step\\new_fe1000_30x30x1_step_00_Angstr.ace", ifstream::in); 
	/*  ifstream ifs(strcat (path1,"\\GPU_TEM.ini"), ifstream::in); 
	  //ifstream ifs("D:\\model\\Crystall\\Si_20x20x5_.ACE", ifstream::in); 
      if (ifs.good())   { // if opening is successful
            // while file has lines
            while (!ifs.eof()) {
                  // read line of text
                  getline(ifs, textLine);
                  // print it to the console
                 // cout << textLine << endl;

				  //if (textLine.find("Atom "))
//cout << textLine.find("Atom ") << endl;
				   // different member versions of find in the same order as above:
				 // found=textLine.find("defocusseriasize");
				//	if (found!=textLine.npos ) {

				//		}
				  char ip[256], mac[256];
				  if(sscanf(textLine, "%s=%s", ip, mac) != 2)
        {
                fprintf(stderr, "Syntax error, line \n");
                continue;
        }	   
			  printf("Line :  param %s val %s\n",  ip, mac);
				}
		  }	 else	  {
            // otherwise print a message
			  cout << "ERROR: can't open GPU_TEM.ini file: " << path1 <<endl;
			  cout << "Press any key to continue."  <<endl;
			_getch();
			
		  }
	  //_getch();
	  */
	 		 FILE *fp;
		fopen_s(&fp,strcat (path1,"\\gpu_tem.ini"), "r");
    if (!fp) {
       sprintf(ErrorMessage, "Error load '%s' %s", path1,strerror( errno ) );
       // fprintf(stderr, "Press any key to continue.");
		//_getch();
		ErrorOn=true;
        return;
    } else {
		sprintf(ErrorMessage, "Loaded '%s'", path1);
		// fprintf(stderr, "Press any key to continue.");
		//_getch();
		ErrorOn=true;
	}

	 char line[256];
   int linenum=0;
while(fgets(line, 256, fp) != NULL)
{
        char parameter_name[256], parameter_value[256];

        linenum++;
        if(line[0] == '#') continue;

        if(sscanf(line, "%s %s", parameter_name, parameter_value) != 2)
        {
                fprintf(stderr, "Syntax error, line %d\n", linenum);
                continue;
        }
	     if (strcmp(parameter_name,"defocusseriasize")==0)
		   SeriaEnd=atoi(parameter_value);
	     if (strcmp(parameter_name,"defocusstart")==0)
		   DefocusStart=(float)atof(parameter_value);
		 if (strcmp(parameter_name,"defocusstep")==0)
		   DefocusStep=(float)atof(parameter_value);
		 if (strcmp(parameter_name,"defocusseriacs")==0)
		   DefocusSeriaCs=(float)atof(parameter_value);
		 
        //printf("Line %d:  IP %s MAC %s\n", linenum, ip, mac);
}
	//_getch();
	fclose(fp);
	}
int main(int argc,  char** argv)
{
	printf("argc=%d\n",argc);
	//	_getch();
	if	 (argc >= 2   ) {

	if (strcmp(argv[1],"/d") || strcmp(argv[1],"/D"))
		debugON=true;
	}

	if(!InitCUDA()) {
		return 0;
	}
	if (debugON)
		printf("InitCUDA OK\n");

    hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
    if (hStdin == INVALID_HANDLE_VALUE || hStdout == INVALID_HANDLE_VALUE) 
    {
        MessageBox(NULL, TEXT("GetStdHandle"),TEXT("Console Error"), MB_OK);
        return -1;
    }
	if (debugON)
		printf("hStdin &  hStdout OK\n");

	//strcpy(APP_path,argv[0]);
	//printf("path = %s\n",APP_path);
//	wchar_t fullPath[MAX_PATH];
//GetModuleFileName(NULL, fullPath, MAX_PATH);
//
//// cut the string at the last slash, because we just want the directory
//// not the actual executeable file name. 
//TCHAR *lastSlash = wcsrchr(fullPath, L'\\');
//lastSlash = 0;
//	printf("path = %s\n",fullPath);
	/*char myPath[_MAX_PATH+1];
GetModuleFileName(NULL,myPath,_MAX_PATH);
	printf("path = %s\n",myPath);
	wgetcwd*/
	//wchar_t* buffer;

   // Get the current working directory: 
  // if( (buffer = _wgetcwd( NULL, 0 )) == NULL )
   //   perror( "_wgetcwd error" );
  // else
   //{
   //   wprintf( L"%s \n", buffer );

  //    free(buffer);
  // }
	//_getch();

	//getinfo();
	//printf("%s \n",ver);
	//_getch();
	//return 0;
//	HICON hIcon1 = (HICON) LoadImage( NULL, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,16,16,LR_DEFAULTSIZE); 
	//SetClassLong(hStdout, -14, (LONG) LoadIcon(hinst, MAKEINTRESOURCE(102))   ); 
//	HANDLE hIcon1 = LoadImage(NULL, "microsope.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	//SendMessage( hStdout, STM_SETIMAGE, IMAGE_ICON, (LPARAM)(UINT)hIcon1);

	//hIcon = LoadIcon(NULL,MAKEINTRESOURCE(1));
	/*HICON hIcon = LoadIcon (NULL, IDI_APPLICATION) ;

	SetIcon( FindWindow(GlutClassName, Title), hIcon);*/

	
	 /*glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(ClientWidth, ClientHeight);
    glutCreateWindow("Point");
    
    srand( (unsigned)time( NULL ) );
    glClearColor((float)(rand()%100)/100.0, (float)(rand()%100)/100.0, (float)(rand()%100)/100.0, 0.0);
    
    glutMouseFunc(mouse);
    glutKeyboardFunc(KeyDown);
    
    glutIdleFunc(Display);
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    
    glutMainLoop();*/
	//float dE=
	//printf("Plank = %.3e c= %.3e E= %.3e\n",Plank_h,c_light,Eng);
		//printf("Lambda %.6e\n",Lambda);
	
	if	 (argc < 2   ) {
		HWND test=GetConsoleHwnd();
		bool res=ShowOpenDialogMy(test, "Open wave complex function", "", "wav\0*.wav\0img\0*.img\0All\0*.*\0",0,filename);
	 //printf("res ='%d'\n",res);
	   if (res==NULL)
		   exit(-2);
			 NewFile2Open=true;
		} else {
			if (!debugON){
 
		  strcpy(filename,argv[1]);
			 NewFile2Open=true;
			} else {
				HWND test=GetConsoleHwnd();
				bool res=ShowOpenDialogMy(test, "Open wave complex function", "", "wav\0*.wav\0img\0*.img\0All\0*.*\0",0,filename);
				//printf("res ='%d'\n",res);
				if (res==NULL)
					exit(-2);
				NewFile2Open=true;

			}

		}
	 //OpenParamFile(argv[0]);
		//printf(" argc %d, %s %s\n",argc,argv[0],argv[1]);
		//_getch();
//char path[2048];
	 //printf("filename ='%s'\n",filename);
	 //printf("filename ='%s'\n",filename);
	 //seed=(int)time(NULL);
	//checkCudaErrors( cudaMalloc( (void**) &seed, sizeof( int ) ));
	SeriaEnd=81;
	OpenParamFile(filename)	;
	//float * image=0;
	while (NewFile2Open) {
	char * pch;
    pch=strrchr(filename,'\\');
	int result = (int)(pch-	filename+1);
	//printf(" position found at %d\n",result);
	//_getch();
	//printf("test %s %d\n",path,pch-path+1);
	//strstr
	//&filename_short=&filename;
	 if ( pch != NULL )
		 {
	//strncpy(filename_short,filename,4096);
    //filename_short[result]='\0';
	memcpy(filename_short,filename+result,4096-result);
	 printf("filename short ='%s'\n",filename_short);

	//memcpy(filename_short,filename+result,*filename-result);
	//filename_short[*filename-result]='\0';
		 }
	 cufftComplex *image_c=0;
	cufftDoubleComplex *image_d_c=0;
	//image=ReadMRC2Float(filename,dimx,dimy,nproj);
	//image_c=ReadMRC2Complex(filename,dimx,dimy,nproj);
	//HT=300;
	//a=70,b=70;
	char *pdest;
   //int  result;
//   printf( "String to be searched:\n   %s\n", string );
  // printf( "   %s\n   %s\n\n", fmt1, fmt2 );
   pdest = strstr( filename_short, ".img" );
   result = (int)(pdest - filename_short + 1);
   if ( pdest != NULL )
	   ImgNotWav=true;
      //printf( "%s found at position %d\n", ".img", result );
  

	nproj=1;
	//_getch();
	if (ImgNotWav) {
		float pxa,pxb;
			image_d_c=ReadImgAndParams(filename,pxa,pxb,dimx,dimy,thickness);
			a=pxa*dimx;
			b=pxb*dimy;
			HT=300;
		} else {
			image_d_c=ReadWavAndParams2(filename,a,b,c,alpha,beta,gamma,HT,dimx,dimy,thickness);
		}

	if (image_d_c == NULL) {
			fprintf(stderr,"End of story\n");
			_getch();
				return -1;
			}
	  NewFile2Open=false;
	float Eng=HT*eV*1000.0f;
	float E0=511000*eV;
	Lambda = (float)( Plank_h * c_light / sqrt( 2 * Eng * E0 + Eng * Eng ) ) * 1E9f;
	a/=10.0f;
	b/=10.0f;
	ux=1.0f/a;
	uy=1.0f/b;
	//Df=-41.0f;	
	if (debugON)
	printf("dimx=%d dimy=%d a=%.3f b=%.3f ux=%.3e uy=%.3e\n",dimx,dimy,a,b,ux,uy);
	//_getch();

	H_Cc=Cc*(3.5e-06f)*(1.0f-Eng/E0)/(1.0f+Eng/(2.0f*E0));
	Df=-1.2f*sqrt(Cs*Lambda);
	tetta=conv/(Lambda*sqrt(log(2.0f)));

	image_c = (cufftComplex*)malloc(dimx*dimy*sizeof(cufftComplex));
	memset(image_c,0,dimx*dimy*sizeof(cufftComplex));
	cx=dimx*0.5f;
	cy=dimy*0.5f;
	tx=0.0f;
	ty=0.0f;
	ImagesizeInnm=a;
	//float imax,imin;
	//	imax=numeric_limits<float>::min();
	//	imin=numeric_limits<float>::max();
	/*for (int i=0;i<nproj;i++){
		for(int j=0;j<dimy;j++){
			for(int k=0;k<dimx;k++){
					int	idx	=	i*dimx*dimy	+	j*dimx	+	k;
					if (image[idx]<imin)
					imin=image[idx];
					if (image[idx]>imax)
					imax=image[idx];

				}
			}

		}
	for (int i=0;i<nproj;i++){
	for(int j=0;j<dimy;j++){
			for(int k=0;k<dimx;k++){
					int	idx	=	i*dimx*dimy	+	j*dimx	+	k;
					image[idx]=(image[idx]-imin)/(imax-imin);
					image_c[idx].y=(image[idx]-imin)/(imax-imin);
					image_c[idx].x=0.0f;
				}
			}
			}*/
	//printf("min max %.3f %.3f\n",imin,imax);
	//CalcGPUMinMax(image,dimx,dimy);
	//width=dimx,height=dimy;
    //GLint m_viewport[4];
 
	//glGetIntegerv( GL_VIEWPORT, m_viewport );
	//printf("x= %d y=%d\n",m_viewport[0],m_viewport[1]);
	//_getch();
	// int w=glutGet (GLUT_SCREEN_WIDTH);
	// int h=glutGet (GLUT_SCREEN_HEIGHT);
	//printf("x= %d y=%d\n",w,h);
	//_getch();
	double pix_per_nm,k;
	int w,h;
	w=min(dimx,1024);
	k=dimx/w;
	h=dimy/k;
	pix_per_nm=1024.0/max(a,b);
	width=w;//(int)(a*pix_per_nm);
	height=h;//(int)(b*pix_per_nm);
	aspect=(float)width/(float)height;
	if (debugON)
	printf("pix_per_nm %.3f w=%.3f h=%.3f a=%.5f  b=%.5f aspect=%.3f \n",pix_per_nm,a*pix_per_nm,b*pix_per_nm,a,b,aspect );
	if (debugON)
	printf("w=%d h=%d u=%.3f v=%.5f\n",w,h,w/a,h/b);
	//_getch();

	int device;
    struct cudaDeviceProp prop;
    cudaGetDevice( &device );
    cudaGetDeviceProperties( &prop, device );
    if( !strncmp( "Tesla", prop.name, 5 ) ) {
        printf("Tesla card detected, running the test in benchmark mode (no OpenGL display)\n");
//        runBenchmark = CUTTrue;
       // g_bQAReadback = true;
		return -2;
    }      
	if (debugON)
 	 printf("device=%d!!!!!!!!!!!!!!!\n",device);
	for(int i=0;i<dimx*dimy;i++)
		{
		image_c[i].x=(float)image_d_c[i].x;
		image_c[i].y=(float)image_d_c[i].y;
		}

    cudaMalloc ( (void**) &d_seed, BLOCK_DIM*BLOCK_DIM*sizeof( int ) );
	h_seed=(int*)malloc(BLOCK_DIM*BLOCK_DIM*sizeof( int ));
    //checkCudaErrors( cudaMalloc( (void**) &devStates, dimx*dimy*sizeof( curandState ) ));

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
/*	union mrc_h header_out;
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
		memset(&ext_header_out.buff,0,1032*128);
		WriteMRC<cufftComplex>("d:\\image_complex_test.mrc", image_c, header_out.header , ext_header_out);

	 */
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111111



	//free(image_d_c);
    initGL(argc, argv);
    //cudaGLSetGLDevice( cutGetMaxGflopsDeviceId() );
   // cudaGLSetGLDevice( 1 );

    initCudaBuffers(image_c,dimx,dimy,nproj);
	cufftPlan2d(&plan, dimy, dimx, CUFFT_C2C);
	for (int i=0;i<nproj;i++)
		CreateFFT(d_img+i*dimx*dimy,dimx,dimy);
	if (debugON)
 	printf("dimx1=%d, dimy1=%d\n",dimx,dimy);
	//checkCudaErrors( cudaMemcpy( image_c, d_img, dimx*dimy*sizeof(cufftComplex), cudaMemcpyDeviceToHost));
	//printf("OK");
	//WriteRawFileT<cufftComplex>(image_c,"d:\\test_cmplx_TEM.dat",dimx,dimy,1);
	//_getch();
		CreateConsole();

	initGLBuffers();
 
    atexit(cleanup);
    if (!setjmp(env))
    glutMainLoop();
	//printf("After glutMainLoop\n"); 
	//_getch();
//    glutMainLoop();
//display();
//reshape(0,0);
//DrawGLScene();
//_getch();
//	cleanup();
	//printf("finish\n");
	//_getch();
	}
    cudaThreadExit();
	//free(image);
	return 0;
}


