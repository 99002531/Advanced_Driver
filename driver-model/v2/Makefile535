obj-m += dummy_serial_dev.o dummy_serial_driver.o
KSRC=/lib/modules/$(shell uname -r)/build	
#replace KSRC if building for other kernel version
all:
	make -C ${KSRC} M=${PWD} modules
clean:
	make -C ${KSRC} M=${PWD} clean
