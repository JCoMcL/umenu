PREFIX = /usr
MANPREFIX = $(PREFIX)/share/man

all: menu.o

%.o: %.c
	$(CC) -o $@ $<

install: menu.o umenu.1
	mkdir -p  $(DESTDIR)$(PREFIX)/bin
	install menu.o $(DESTDIR)$(PREFIX)/bin/umenu
	install -m 644 umenu.1 $(DESTDIR)$(MANPREFIX)/man1/umenu.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/umenu
	rm -f $(DESTDIR)$(MANPREFIX)/man1/umenu.1

clean:
	rm -f menu.o

.PHONY: all install uninstall clean
