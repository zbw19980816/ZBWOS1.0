all: \
	./nandflash.o \
	./sdram.o \
	./uart.o \
	./led.o \
	./interrupt.o \
	./timer.o \
	./lcd/font.o \
	./lcd/framebuffer.o \
	./lcd/geometry.o \
	./lcd/lcd.o \
	./lcd/lcd_4.3.o \
	./lcd/lcd_controller.o \
	./lcd/lcd_test.o \
	./lcd/s3c2440_lcd_controller.o \
	./lcd/font_8x16.o \
	./touch/touchscreen.o \
	./touch/tslib.o
	arm-linux-ar rcs lib2440drv.a ./*.o ./lcd/*.o ./touch/*.o
clean:
	rm *.bin *.elf *.dis *.o *.a -rf
	rm ./lcd/*.o -rf
	rm ./touch/*.o -rf
	
%.o : %.c
	arm-linux-gcc -c -o $@ $< -I ../../include/soc -I ../../zbwos_core -I ../../file_system -I ../../user/jpeg

%.o : %.s
	arm-linux-gcc -c -o $@ $<

%.o : %.S
	arm-linux-gcc -c -o $@ $<
