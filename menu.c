#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>


static struct termios tio_old;

typedef struct Option Option;
struct Option {
	char key;
	char *text;
};
static Option *options = NULL;
static int option_count;

char getkey(int i) {
	if (i < 9) {		return '1' + i;}
	else if (i == 9) {	return '0';}
	else if (i < 35) {	return 'a' + i - 10;}
	else {			return '\0';}
}

void cleanup(void) {
	if (tio_old.c_lflag) {
		tcsetattr(0, TCSANOW, &tio_old);
	}
	free(options);
}

void die(const char *s) {
	fprintf(stderr, "%s\n", s);
	cleanup();
	exit(1);
}

void genoptions() {
	char buf[BUFSIZ], *p;
	for (int i = 0; fgets(buf, sizeof buf, stdin); i++) {
		Option opt;
		/* remove trailing newline */
		p = strchr(buf, '\n');
		if (p) {
			*p = '\0';
		}

		char key = getkey(i);
		if (!key){
			fprintf(stderr, "Warning! Maximum options (%i) reached\n", i-1);
			return;
		}
		opt.key = key;
		
		opt.text = strdup(buf);
		
		options = realloc(options, sizeof(Option) * (i + 1));
		if (!options) {
			die("Can't realloc");
		}

		options[i] = opt;
		fprintf(stderr, "%c %s\n", options[i].key, options[i].text);
		option_count = i + 1;
	}
}
void tsetup(void) {
	/* reopen stdin for user input */
	if (freopen("/dev/tty", "r", stdin) == NULL) {
		die("Can't reopen tty.");
	}
	
	tcgetattr(0, &tio_old);
	struct termios tio_new = tio_old;
	/* disable unwanted attributes */
	tio_new.c_lflag &= ~(
			ICANON| //reads input per line rather than per character
			ECHO //echoes the typed input
			); 
	tio_new.c_cc[VMIN]=1; //require minimum of 1 char
	
	tcsetattr(0, TCSANOW, &tio_new);
}

const char *getoption(char c) {
	int i;
	for (i = 0; i < option_count && (options[i].key != c); i++) {}
	if (i < option_count) {
		return options[i].text;
	} else {
		return NULL;
}}
		

int main() {
	//readargs();
	genoptions();
	tsetup();
	
	char c;
	read(0, &c, 1);
	const char *out = getoption(c);
	if (out) {
		printf(out);
		putchar('\n');
	} else {
		die("Invalid Character");
	}
	cleanup();
	return 0;
}
