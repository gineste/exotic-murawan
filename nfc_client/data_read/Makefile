
#GCC:= arm-none-linux-gnueabi-gcc
GCC:= gcc
SRC:=*.c
all:
	$(GCC) -std=c99 -fPIC -o data $(SRC) ./lib/libS8_64.a
clean:
	rm -f data 
