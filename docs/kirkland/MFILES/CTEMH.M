function y = ctemh(k,params,type)
%
%  MATLAB function ctemh.m to calculate CTEM bright field
%  phase contrast transfer function with partial coherence
%  for weak phase objects
%    input array k has the spatial frequency values (in 1/A)
%    input array params has the optical parameters
%          params = [Cs, df, kev, ddf, beta]
%    input type = 0 for phase contrast
%           and 1 for amplitude contrast
%    output array contains the transfer function vs k
%
%  Cs   = spherical aberration (in mm)
%  df   = defocus (in Angstroms)
%  kev  = electron energy (in keV)
%  ddf  = chromatic aberation defocus spread (in Angstroms)
%  beta = spread in illumination angles (in mrad)
%
%  started 12-Nov-1996 E. Kirkland
%  last modified 2-Mar-1997 ejk
%
%  reference
%  R. H. Wade and J. Frank, Optik 49 (1977) p.81
%
Cs = params(1)*1.0e7;
df = params(2);
kev = params(3);
ddf = params(4);
beta = params(5)*0.001;
mo = 511.0;      % electron rest mass in keV
hc = 12.3986;    % in keV-Angstroms
wav = (2*mo)+kev;
wav = hc/sqrt(wav*kev);
w1 = pi*Cs*wav*wav*wav;
w2 = pi*wav*df;
e0 = (pi*beta*ddf)^2;
k2 = k .* k;
wr = (w1.*k2-w2).*k*beta/wav;
wi = pi*wav*ddf.*k2;
wi = wr.*wr + 0.25.*wi.*wi;
wi = exp(-wi./(1+e0.*k2));
wr = ( 0.5*w1.*k2.*(1-e0.*k2) - w2 ).*k2./(1+e0.*k2);
if type == 0
    y = sin(wr).* wi;
else
    y = cos(wr).* wi;
end;

