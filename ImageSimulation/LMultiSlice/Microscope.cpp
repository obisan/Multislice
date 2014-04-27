#include "stdafx.h"
#include "Microscope.h"

Microscope::Microscope() {

}

Microscope::Microscope(double keV, double Cs, double aperture, double defocus) {
	this->keV = keV;
	this->Cs = Cs;
	this->aperture = aperture;
	this->defocus = defocus;
	this->lambda = wavelength(keV);
	this->sigma = Sigma(keV);
}

double Microscope::Es(double k) {
	//Es = 1; 
	//Es = (dLambda * k > aperture / 100) ? 0 : 1; //A(k) When threshold is (aperture / 100)
	return exp(- pow(M_PI * aperture / (lambda * 1000), 2) * pow(Cs * pow(lambda, 3) * k * k * k + defocus * lambda * k, 2));
}

double Microscope::alpha(double k) {
	return M_PI * lambda * k * k * (0.5 * Cs * lambda * lambda * k * k - defocus * 10);;
}

double Microscope::getKeV() {
	return keV;
}

double Microscope::getCs() {
	return Cs;	
}

double Microscope::getAperture() {
	return aperture;
}

double Microscope::getDefocus() {
	return defocus;
}

double Microscope::getSigma() {
	return sigma;
}

double Microscope::getLambda() {
	return lambda;
}

void Microscope::setKeV(double keV) {
	this->keV = keV;
	this->lambda = wavelength(keV);
	this->sigma = Sigma(keV);
}

void Microscope::setCs(double Cs) {
	this->Cs = Cs;
}

void Microscope::setAperture(double aperture) {
	this->aperture = aperture;
}

void Microscope::setDefocus(double defocus) {
	this->defocus = defocus;
}

double Microscope::Sigma( double kev ) {
	double s, pi, wavl, x;
	const double emass = 510.99906; /* electron rest mass in keV */
	x = (emass + kev) / (2.0 * emass + kev);
	wavl = wavelength(kev);
	pi = 4.0 * atan(1.0);
	s = 2.0 * pi * x / (wavl * kev);
	return s;
}

double Microscope::wavelength( double kev ) {
	double w;
	const double emass = 510.99906; /* electron rest mass in keV */
	const double hc = 12.3984244; /* Planck's const x speed of light*/
	/* electron wavelength in Angstroms */
	w = hc / sqrt( kev * (2 * emass + kev));
	return w;
}