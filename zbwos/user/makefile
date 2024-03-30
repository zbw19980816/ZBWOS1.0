all: \
	./main.o
	arm-linux-ar rcs libuser.a ./*.o  ./zjpeg/*.o ./zjpeg/tjpgd/*.o 
clean:
	rm *.o *.a -rf
	
%.o : %.c
	arm-linux-gcc -c -o $@ $< -I ../include/soc -I ../zbwos_core -I ../file_system 

%.o : %.s
	arm-linux-gcc -c -o $@ $< -I ../include/soc -I ../zbwos_core -I ../file_system 

%.o : %.S
	arm-linux-gcc -c -o $@ $<
