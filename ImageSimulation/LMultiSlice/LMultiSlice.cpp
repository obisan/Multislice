// LMultiSlice.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Dispatcher.h"
#include "test.h"

#define K 1
#define NX K*512
#define NY K*512
#define NUMBERSLICES 1
#define RADIUC 50.0f
#define DPA 10.0f // dots per angstrem

#define APERTURE 10.37
#define CS 1.3
#define DEFOCUS -70 // nm
#define KEV 200

int main(int argc, int *argv[]) {
	Dispatcher *task = new Dispatcher();

	task->Run("../samples/template.xml");

	delete task;

	//test("..\\samples\\C_Si_Cu_Au_U.coo", "..\\samples\\C_Si_Cu_Au_U.txt", NUMBERSLICES, NX, NY, DPA, RADIUC, KEV, CS, APERTURE, DEFOCUS);
	//test("..\\samples\\fourSameAtomsDifferentZ.coo", "..\\samples\\fourSameAtomsDifferentZ.txt", NUMBERSLICES, NX, NY, DPA, RADIUC, KEV, CS, APERTURE, DEFOCUS);
	
	//test("..\\samples\\fourSameAtomsSameZ.coo", "..\\samples\\fourSameAtomsSameZ", NUMBERSLICES, NX, NY, DPA, RADIUC, KEV, CS, APERTURE, DEFOCUS);
	//test("..\\samples\\Si_one.coo", "..\\samples\\Si_one", NUMBERSLICES, NX, NY, DPA, RADIUC, KEV, CS, APERTURE, DEFOCUS);
	//test("..\\samples\\Au_one.coo", "..\\samples\\Au_one.txt", NUMBERSLICES, NX, NY, DPA, RADIUC);
	//test("..\\samples\\Empty.coo", "..\\samples\\Empty.txt", NUMBERSLICES, NX, NY, DPA, RADIUC);
	//test("..\\samples\\Ni.coo", "..\\samples\\Ni.txt", NUMBERSLICES, NX, NY, DPA, RADIUC);
	//test("..\\samples\\Ni_mono.coo", "..\\samples\\Ni_mono.txt", NUMBERSLICES, NX, NY, DPA, RADIUC);
	//test("..\\samples\\Fe1000_10x10x2_down.coo", "..\\samples\\Fe1000_10x10x2_down.txt", NUMBERSLICES, NX, NY, DPA, RADIUC);
	
	//test("..\\samples\\fourDifferentAtomsSameZ.coo", "..\\samples\\fourDifferentAtomsSameZ.txt", NUMBERSLICES, NX, NY, DPA, RADIUC);
	//test("..\\samples\\fourSameAtomsDifferentZ.coo", "..\\samples\\fourSameAtomsDifferentZ.txt", NUMBERSLICES, NX, NY, DPA, RADIUC);
	return 0;
}