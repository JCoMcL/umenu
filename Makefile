INSTALLDIR = /usr/bin
all: menu.o

menu.o:
	$(CC) -o menu.o menu.c
install: menu.o
	cp menu.o $(INSTALLDIR)/umenu
uninstall:
	rm -f $(INSTALLDIR)/umenu
clean:
	rm -f menu.o
