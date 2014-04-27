function mtf = stemhk(k,params)
%
%  MATLAB function stemhk.m to calculate STEM mtf vs. k
%    input array k has the spatial freq. (in inv. Angs.)
%    input array params has the optical parameters
%          params = [Cs, df, kev, amax]
%    output array contains the transfer function
%
%  Cs   = spherical aberration (in mm)
%  df   = defocus (in Angstroms)
%  kev  = electron energy (in keV)
%  amax = objective aperture  (in mrad)
%
%  started 6-apr-1997 E. Kirkland
%  last modified 6-apr-1997 ejk
%
Cs = params(1)*1.0e7;
df = params(2);
kev = params(3);
amax = params(4)*0.001;
%  first calculate the psf using stemhr()
nr = 200;  % number of points in integral over r
wav = 12.3986/sqrt((2*511.0+kev)*kev);  % electron wavelength
rmax = sqrt( sqrt( Cs*wav*wav*wav) );
r = 0:(rmax/nr):rmax;
psf = stemhr( r, params );
% next inverse psf to get mtf
nk = length( k );
for ik=1:nk,
	h = psf .* besselj( 0, 2*pi*r*k(ik) ) .*r;
	mtf(ik) = sum(h);
end;
a = mtf(1);
mtf = mtf/a;  % normalize mtf(0)=1
