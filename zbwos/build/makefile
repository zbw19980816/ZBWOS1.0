all: \
	../soc/$(PLATFORM)/start/start.o \
	../user/libuser.a \
	../soc/$(PLATFORM)/lib2440drv.a \
	../zbwos_core/libzbwos.a \
	../file_system/libfilesystem.a 
	arm-linux-ld -T zbwos.lds $^ ../lib/libc.a ../lib/libgcc.a -o zbwos.elf
	arm-linux-objcopy -O binary -S zbwos.elf zbwos.bin
	# arm-linux-objdump -D zbwos.elf > zbwos.dis

clean:
	rm *.bin *.elf  -rf
