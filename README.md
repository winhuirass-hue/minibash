as minil.S -o minil.o
gcc -c minibash.c \
  -ffreestanding -fno-stack-protector -fno-asynchronous-unwind-tables \
  -fno-unwind-tables -fno-pic -fno-pie \
  -O2 -nostdlib -o minibash.o
ld -o minibash minil.o minibash.o \
  -e _start \
  --gc-sections \
  --build-id=none
strip --strip-all minibash
