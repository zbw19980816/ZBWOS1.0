all: \
	./touch/touch_proc.o \
	./win/Desktop.o ./win/win_setting.o \
	./win/zui/z_bitmap.o ./win/zui/z_static.o ./win/zui/z_win.o ./win/zui/z_core.o
	arm-linux-ar rcs libapp.a \
	./win/zui/z_bitmap.o ./win/zui/z_static.o ./win/zui/z_win.o ./win/zui/z_core.o \
	./touch/touch_proc.o \
	./win/Desktop.o ./win/win_setting.o
	
clean:
	rm ./win/*.o *.a ./touch/*.o -rf
	
%.o : %.c
	arm-linux-gcc -c -o $@ $< \
	-I ../../include/soc \
	-I ../../zbwos_core \
	-I ../../file_system \
	-I ../zjpeg \
	-I ./touch/include \
	-I ./win/zui/include
