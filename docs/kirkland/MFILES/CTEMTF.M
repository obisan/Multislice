%
%  MATLAB script ctemtf.m to plot CTEM transfer functions
%  this script calls ctemh.m
%
%  12-Nov-1996 E. Kirkland
%  last modified 6-apr-1997 ejk
%
%  Cs = Spherical Aberration
%  df = defocus
%  kev = electron energy in keV
%  ddf = chromatic aberation defocus spread
%  beta =  spread in illumination angles
%
disp( 'Plot CTEM transfer function' );
kev  = input( 'Type electron energy in keV :');
Cs   = input( 'Type spherical aberation Cs in mm :');
df   = input( 'Type defocus df in Angstroms :');
ddf  = input( 'Type defocus spread ddf in Angstroms :');
beta = input( 'Type illumination semiangle in mrad :');
%
wav = 12.3986/sqrt((2*511.0+kev)*kev);  % electron wavelength
ds = sqrt( sqrt( Cs*1.0e7*wav*wav*wav ));
kmax = 2.5/ds;
k = 0.:(kmax/500):kmax;  % 500 points
params = [ Cs, df, kev, ddf, beta ];
sinw = ctemh( k, params, 0 );
plot( k, sinw  );
axis([0, kmax, -1, +1]);
xlabel( 'Spatial Frequency (in 1/A)');
ylabel( 'MTF' );
s1 = sprintf('E= %gkeV, Cs= %gmm, df= %gA,  ', kev, Cs, df);
s2 = sprintf('Beta= %gmrad, ddf= %gA', beta, ddf);
title([s1 s2]);
hold on;		 % plot line through zero
x = [0, kmax];
y = [0, 0];
plot( x, y );
hold off;
