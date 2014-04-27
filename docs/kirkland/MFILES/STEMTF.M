%
%  Matlab file to plot the STEM probe profile
%  this script calls stemhk.m
%
%  started 6-apr-1997 E. Kirkland
%  last modified 6-apr-1997 ejk
%
disp( 'Plot STEM probe intensity' );
kev  = input( 'Type electron energy in keV :');
Cs   = input( 'Type spherical aberation Cs in mm :');
df   = input( 'Type defocus df in Angstroms :');
amax = input( 'Type objective aperture semiangle in mrad :');
%
wav = 12.3986/sqrt((2*511.0+kev)*kev);  % electron wavelength
kmax = 2*0.001*amax/wav;
k = 0:(kmax/200):kmax;  % 200 points
param = [ Cs df kev amax ];
mtf = stemhk( k, param );
plot( k, mtf );
xlabel( 'k in inv. Angstroms');
ylabel( 'MTF' );
s1 = sprintf('Cs= %gmm, df= %gA, E= %gkeV, ', Cs, df, kev);
s2 = sprintf('OA= %gmrad', amax);
title([s1 s2]);
