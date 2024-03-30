all: \
	./initsoc.o \
	./task.o \
	./task_s.o \
	./my_printf.o \
	./lib1funcs.o \
	./stl.o \
	./shell.o \
	./corectrl.o \
	./heap.o
	arm-linux-ar rcs libzbwos.a ./*.o
clean:
	rm *.o *.a -rf
	
%.o : %.c
	arm-linux-gcc -c -o $@ $< -I ../include/soc -I ../zbwos_core -I ../file_system 

%.o : %.s
	arm-linux-gcc -c -o $@ $< -I ../include/soc -I ../zbwos_core -I ../file_system 

%.o : %.S
	arm-linux-gcc -c -o $@ $<
