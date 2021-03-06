PREFIX = /usr
MANPREFIX = $(PREFIX)/share/man

all: menu.o

%.o: %.c
	$(CC) -o $@ $<

install: menu.o umenu.1
	mkdir -p  $(DESTDIR)$(PREFIX)/bin
	install menu.o $(DESTDIR)$(PREFIX)/bin/umenu
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	install -m 644 umenu.1 $(DESTDIR)$(MANPREFIX)/man1/umenu.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/umenu
	rm -f $(DESTDIR)$(MANPREFIX)/man1/umenu.1

clean:
	rm -f menu.o

push:
	git push origin --tags

.PHONY: all install uninstall clean push
