all: z_bitmap.o z_static.o z_core.o z_win.o
	arm-linux-ar rcs libzui.a z_bitmap.o z_static.o z_core.o z_win.o
clean:
	rm *.a *.o -rf
	
%.o : %.c
	arm-linux-gcc -c -o $@ $< \
	-I ../../../../include/soc \
	-I ../../../../zbwos_core \
	-I ../../../../file_system \
	-I ../../../zjpeg \
	-I ../../touch/include \
	-I ./include
