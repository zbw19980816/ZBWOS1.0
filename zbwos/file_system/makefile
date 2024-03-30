all: \
	./filesystem.o
	arm-linux-ar rcs libfilesystem.a ./*.o
clean:
	rm *.o *.a -rf
	
%.o : %.c
	arm-linux-gcc -c -o $@ $< -I ../include/soc -I ../zbwos_core -I ../file_system 

%.o : %.s
	arm-linux-gcc -c -o $@ $< -I ../include/soc -I ../zbwos_core -I ../file_system 

%.o : %.S
	arm-linux-gcc -c -o $@ $<
