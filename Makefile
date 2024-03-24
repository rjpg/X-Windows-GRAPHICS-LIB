
exe: xexample.o xdriver.o
	gcc -o xexample xexample.o xdriver.o -L/usr/X11R6/lib -lX11 -lm -Wall

demonstracao: xexample.o
	gcc  xexample.c

xdriver: xdriver.o
	gcc -c xdriver.c



clean:
	rm -f *~ *.o xexample 