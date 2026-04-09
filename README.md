as minil.S -o minil.o
gcc -c minibash.c -ffreestanding -O2 -nostdlib -o minibash.o
ld -o app minil.o minibash.o -e _start
