#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

#define COLOR(s) "\e[96m" s "\e[39m"
#define BOLD(s) "\e[1m" s "\e[22m"
#define RED(s) "\e[31m" s "\e[39m"

static struct termios tio_old;

struct Option {
	char key;
	char *text;
};
static struct Option *options = NULL;
static int option_count;

char getKeyFromIndex(int i) {
	if (i < 9) {		return '1' + i;} //chars '1'-'9'
	else if (i == 9) {	return '0';}
	else if (i < 36) {	return 'a' + i - 10;} //chars 'a'-'z'
	else {			return '\0';}
}

void cleanup(void) {
	if (tio_old.c_lflag) {
		tcsetattr(0, TCSANOW, &tio_old);
	}
	if (options); {
		free(options);
}	}


void die(const char *s) {
	fprintf(stderr, RED("%s\n"), s);
	cleanup();
	exit(1);
}

/* read stdin and initialize global array of options */
void genOptions() {
	char buf[BUFSIZ];
	for (int i = 0; fgets(buf, sizeof buf, stdin); i++) {
		struct Option opt;
		/* remove trailing newline */
		char *p = strchr(buf, '\n');
		if (p) {
			*p = '\0';
		}

		char key = getKeyFromIndex(i);
		if (!key){
			fprintf(stderr,
				RED("Warning! Maximum options (") "%i" RED(") reached\n"),
				i-1);
			return;
		}
		opt.key = key;
		
		opt.text = strdup(buf);
		
		options = realloc(options, sizeof(struct Option) * (i + 1));
		if (!options) {
			die("Can't realloc");
		}

		options[i] = opt;
		fprintf(stderr, BOLD("["COLOR("%c")"]") " %s\n", options[i].key, options[i].text);
		option_count = i + 1;
}	}

const char *findOption(char c) {
	int i;
	for (i = 0; i < option_count && (options[i].key != c); i++) {}
	if (i < option_count) {
		return options[i].text;
	} else {
		return NULL;
}	}

const char *getOptionFromInput(void) {
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

	char c;
	while(1) {
		read(0, &c, 1);
		const char *out = findOption(c);
		if (out) {
			return out;
		} else {
			fprintf(stderr, "Invalid Character: %c\n", c);
}	}	}

int main() {
	//readargs();
	genOptions();
	puts(getOptionFromInput());

	cleanup();
	return 0;
}
