all: \
	./start.o
	arm-linux-ar rcs libstart.a ./*.o
clean:
	rm *.o *.a -rf
	
%.o : %.c
	arm-linux-gcc -c -o $@ $<

%.o : %.s
	arm-linux-gcc -c -o $@ $<

%.o : %.S
	arm-linux-gcc -c -o $@ $<
