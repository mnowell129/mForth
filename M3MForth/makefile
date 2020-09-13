default : coredefs.h forthmakes.exe

coredefs.h : coredefs.src forthmakes.exe
	./forthmakes coredefs.src coredefs.h

forthmakes.exe : forthmakes.c
	gcc -o forthmakes.exe forthmakes.c

clean:
	rm -rf forthmakes.exe coredefs.h

gitignore:
	@echo coredefs.h > .gitignore

