#pragma once

class Microscope {
public:
	Microscope();
	Microscope(double keV, double Cs, double aperture, double defocus);
	
	double Es(double k);
	double alpha(double k);

	double	getKeV();
	double	getCs();
	double	getAperture();
	double	getDefocus();
	double	getSigma();
	double	getLambda();

	void	setKeV(double keV);
	void	setCs(double Cs);
	void	setAperture(double aperture);
	void	setDefocus(double defocus);

private:
	double keV;
	double Cs;
	double aperture;
	double defocus; // nm
	double lambda;
	double sigma;

	double Sigma(double kev);
	double wavelength(double kev);

};