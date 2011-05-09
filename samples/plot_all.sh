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
#meth[2]=SOP
meth[2]=BCZ99
meth[3]=SLAP
#meth[5]=SLAP-FSA
meth[4]=SLAP-FST
#meth[7]='FSEL -dR3'
#meth[7]='FSEL -R3f'
meth[5]=FSEL
#meth[10]='FSOWCTY -dR3'
meth[6]=FSOWCTY
#meth[6]='FSOWCTY -R3f'


#set -x
for (( i = 1; i <= 6; i++ ))
do
  for (( j=$(( $i+1 )); j <= 6; j++ ))
    do
    ./graph.sh "${meth[$i]}-${meth[$j]}-time.eps" -v "${meth[$i]}" "${meth[$j]}" 7 $1/**/*.log
    ./graph.sh "${meth[$i]}-${meth[$j]}-mem.eps" -v "${meth[$i]}" "${meth[$j]}" 8 $1/**/*.log
    if [ $i -lt "5" -a $j -lt "5" ]
	then
	./graph.sh "${meth[$i]}-${meth[$j]}-prodS.eps" -v "${meth[$i]}" "${meth[$j]}" 3 $1/**/*.log ;
    fi
    done
  done

