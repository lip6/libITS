# /bin/zsh


if test $# -ne 1; then
  echo "syntax: plot_all.sh directory"
  exit 1
fi

if test ! -d "$1"; then
  echo "$1 is not a directory"
  exit 1
fi

meth[1]=SOG
meth[2]=SOP
meth[3]=BCZ99
meth[4]=SLAP
meth[5]=SLAP-FSA
meth[6]=SLAP-FST
meth[7]='FSEL -dR3'
meth[8]='FSEL -R3f'
meth[9]=FSEL
meth[10]='FSOWCTY -dR3'
meth[10]=FSOWCTY
meth[11]='FSOWCTY -R3f'


#set -x
for (( i = 1; i <= 11; i++ ))
do
  for (( j=$(( $i+1 )); j <= 11; j++ ))
    do
    ./graph.sh "${meth[$i]}-${meth[$j]}-time.png" -v "${meth[$i]}" "${meth[$j]}" 7 $1/**/*.log
    ./graph.sh "${meth[$i]}-${meth[$j]}-mem.png" -v "${meth[$i]}" "${meth[$j]}" 8 $1/**/*.log
    if [ $i -lt "7" -a $j -lt "7" ] 
	then
	./graph.sh "${meth[$i]}-${meth[$j]}-prodS.png" -v "${meth[$i]}" "${meth[$j]}" 3 $1/**/*.log ;
    fi
    done
  done

