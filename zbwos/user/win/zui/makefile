all: zui_win.o zui_control.o zui_control_static.o zui_vector.o
	arm-linux-ar rcs libzui.a *.o

%.o : %.cpp
	arm-linux-g++ -c -o $@ $< -I ../../../include/soc -I ../../../zbwos_core -I ../../../file_system -I ../../../user/jpeg

clean:
	rm *.o -rf
	rm *.a -rf

