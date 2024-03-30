all: \
	./tjpgd/tjpgd.o \
	./jpeg_proc.o
	arm-linux-ar rcs libzjpeg.a ./*.o ./tjpgd/*.o
clean:
	rm *.o *.a ./tjpgd/*.o -rf
	
%.o : %.c
	arm-linux-gcc -c -o $@ $< -I ../../include/soc -I ../../zbwos_core -I ../../file_system -I ./tjpgd 
