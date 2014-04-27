function psf = stemhr(r,params)
%
%  MATLAB function stemhf.m to calculate STEM probe profile vs. r
%    input array r has the radial positions (in Angs.)
%    input array params has the optical parameters
%          params = [Cs, df, kev, amax]
%    output array contains the transfer function
%    NOTE: the psf(r) values are not normalize
%
%  Cs   = spherical aberration (in mm)
%  df   = defocus (in Angstroms)
%  kev  = electron energy (in keV)
%  amax = objective aperture  (in mrad)
%
%  started 27-Feb-1997 E. Kirkland
%  last modified 2-Mar-1997 ejk
%
Cs = params(1)*1.0e7;
df = params(2);
kev = params(3);
amax = params(4)*0.001;
nk = 200;  % number of points in integral over k
wav = 12.3986/sqrt((2*511.0+kev)*kev);  % electron wavelength
kmax = amax/wav;
dk = kmax / nk;
k = 0:dk:kmax;
k2 = k .* k;
w1 = 0.5*Cs*wav*wav*wav;
w2 = wav*df;
w = pi*( w1.*k2 - w2 ).*k2;
expw = exp( -i*w );
nr = length( r );
for ir=1:nr,
	h = expw .* besselj( 0, 2*pi*r(ir)*k ) .*k;
	psf(ir) = (abs(sum(h))^2)*2/(nk*pi);
end;
