PREFIX = /usr
MANPREFIX = $(PREFIX)/share/man
all: menu.o

menu.o: menu.c
	$(CC) -o menu.o menu.c
install: menu.o umenu.1
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp menu.o $(DESTDIR)$(PREFIX)/bin/umenu
	cp umenu.1 $(DESTDIR)$(MANPREFIX)/man1/umenu.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/umenu.1
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/umenu
	rm -f $(DESTDIR)$(MANPREFIX)/man1/umenu.1
clean:
	rm -f menu.o
