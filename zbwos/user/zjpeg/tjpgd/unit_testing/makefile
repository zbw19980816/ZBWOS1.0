all: ../tjpgd.o unit_testing.o
	gcc $^ -o unit_testing

clean:
	rm unit_testing -rf	
	rm *.o ../*.o -rf
	
%.o : %.c
	gcc -c -o $@ $<

%.o : %.s
	gcc -c -o $@ $<

%.o : %.S
	gcc -c -o $@ $<
