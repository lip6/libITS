s/^/LTLSPEC !(/g
s/$/)/g

s/Idle\([0-9][0-9]*\)/(Idle\1>=1)/g
s/WaitLeft\([0-9][0-9]*\)/(WaitLeft\1>=1)/g
s/HasLeft\([0-9][0-9]*\)/(HasLeft\1>=1)/g
s/WaitRight\([0-9][0-9]*\)/(WaitRight\1>=1)/g
s/HasRight\([0-9][0-9]*\)/(HasRight\1>=1)/g
s/Fork\([0-9][0-9]*\)/(Fork\1>=1)/g
s/IsEating\([0-9][0-9]*\)/(IsEating\1>=1)/g

s/ R / V /g
s/FG/F G/g
s/GF/G F/g
s/"//g
