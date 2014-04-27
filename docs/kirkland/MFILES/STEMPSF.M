%
%  Matlab file to plot the STEM probe profile
%  this script calls stemhr.m
%
%  started 27-feb-1997 E. Kirkland
%  last modified 2-mar-1997 ejk
%
disp( 'Plot STEM probe intensity' );
kev  = input( 'Type electron energy in keV :');
Cs   = input( 'Type spherical aberation Cs in mm :');
df   = input( 'Type defocus df in Angstroms :');
amax = input( 'Type objective aperture semiangle in mrad :');
%
wav = 12.3986/sqrt((2*511.0+kev)*kev);  % electron wavelength
rmax = sqrt( sqrt( Cs*1.0e7*wav*wav*wav ));
r = 0:(rmax/200):rmax;  % 200 points
param = [ Cs df kev amax ];
psf = stemhr( r, param );
a = psf(1);
psf = psf/a;  % normalize psf(r)
plot( r, psf );
xlabel( 'radius in Angstroms');
ylabel( 'PSF' );
s1 = sprintf('Cs= %gmm, df= %gA, E= %gkeV, ', Cs, df, kev);
s2 = sprintf('OA= %gmrad', amax);
title([s1 s2]);
