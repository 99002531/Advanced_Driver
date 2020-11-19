obj-m += dummy_serial_dev.o
obj-m += dummy_serial_driver.o

KSRC=/home/saumya/Desktop/linux-4.14.202
ARCH=arm
CROSS_COMPILE_PREFIX=arm-linux-gnueabi-

all:
	make -C ${KSRC} M=${PWD} ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE_PREFIX} modules
clean:
	make -C ${KSRC} M=${PWD} ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE_PREFIX} clean

