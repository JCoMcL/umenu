PREFIX = /usr
all: menu.o

menu.o:
	$(CC) -o menu.o menu.c
install: menu.o
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp menu.o $(DESTDIR)$(PREFIX)/bin/umenu
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/umenu
clean:
	rm -f menu.o
