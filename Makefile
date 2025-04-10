PREFIX = /usr
MANPREFIX = $(PREFIX)/share/man

umenu: umenu.c
	$(CC) -o $@ $<

%.gz: %
	gzip -fk $<

install: umenu umenu.1.gz
	mkdir -p  $(DESTDIR)$(PREFIX)/bin
	install $< $(DESTDIR)$(PREFIX)/bin/umenu
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	install -m 644 umenu.1.gz $(DESTDIR)$(MANPREFIX)/man1/umenu.1.gz

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/umenu
	rm -f $(DESTDIR)$(MANPREFIX)/man1/umenu.1.gz

clean:
	rm -f umenu *.gz

push:
	git push origin --tags

.PHONY: install uninstall clean push
