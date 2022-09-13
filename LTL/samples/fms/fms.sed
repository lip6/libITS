s/^/LTLSPEC !(/g
s/$/)/g

s/P3s/(P3s>=1)/g
s/P3M2/(P3M2>=1)/g
s/P2s/(P2s>=1)/g
s/P2wP1/(P2wP1>=1)/g
s/P2d/(P2d>=1)/g
s/P2M2/(P2M2>=1)/g
s/P2wM2/(P2wM2>=1)/g
s/P12M3/(P12M3>=1)/g
s/P12wM3/(P12wM3>=1)/g
s/P1wP2/(P1wP2>=1)/g
s/P1s/(P1s>=1)/g
s/P1d/(P1d>=1)/g
s/P1M1/(P1M1>=1)/g
s/P1wM1/(P1wM1>=1)/g
s/P12s/(P12s>=1)/g

s/\(P1\)\([^>2wsdM]\)/(P1>=1)\2/g
s/\(M1\)\([^>]\)/(M1>=1)\2/g
s/\(P12\)\([^wMs]\)/(P12>=1)\2/g
s/\(P2\)\([^swdM>]\)/(P2>=1)\2/g
s/\(M2\)\([^>]\)/(M2>=1)\2/g
s/\(P3\)\([^sM]\)/(P3>=1)\2/g
s/\(M3\)\([^>]\)/(M3>=1)\2/g

s/ R / V /g
s/FG/F G/g
s/GF/G F/g
s/"//g
