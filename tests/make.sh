../bin/ba < $1.ba > $1.s
cc $1.s -o $1 -m32
./$1
echo $?
