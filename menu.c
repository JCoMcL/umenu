#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

#define COLOR(s) "\e[96m" s "\e[39m"
#define BOLD(s) "\e[1m" s "\e[22m"
#define RED(s) "\e[31m" s "\e[39m"

struct Option {
	char key;
	char *text;
};
static int option_count;

struct Option *getOptions();
const char *getOptionFromInput(void);
char getKeyFromIndex(int i);
char getUserInput(void);
const char *findOption(struct Option *options, char c);
void die(const char *s);

int main() {
	struct Option *options = getOptions();
	const char *output;

	for (char c = getUserInput(); !(output=findOption(options, c)); c = getUserInput()) {
		fprintf(stderr, "Invalid Character: %c\n", c);
	}
	puts(output);
	free(options);
	return 0;
}

struct Option *getOptions() {
	struct Option *options = NULL;
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
			return options;
		}
		opt.key = key;
		
		opt.text = strdup(buf);
		
		options = realloc(options, sizeof(struct Option) * (i + 1));
		if (!options) {
			free(options);
			die("Can't realloc");
		}

		options[i] = opt;
		fprintf(stderr, BOLD("["COLOR("%c")"]") " %s\n", options[i].key, options[i].text);
		option_count = i + 1;
	}
	return options;
}

char getKeyFromIndex(int i) {
	if (i < 9) {		return '1' + i;} //chars '1'-'9'
	else if (i == 9) {	return '0';}
	else if (i < 36) {	return 'a' + i - 10;} //chars 'a'-'z'
	else {			return '\0';}
}

char getUserInput(void) {
	/* reopen stdin for user input */
	if (freopen("/dev/tty", "r", stdin) == NULL) {
		die("Can't reopen tty.");
	}
	
	struct termios tio_old;
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
	read(0, &c, 1);
	tcsetattr(0, TCSANOW, &tio_old);
	return c;
}

const char *findOption(struct Option *options, char c) {
	int i;
	for (i = 0; i < option_count && (options[i].key != c); i++) {}
	if (i < option_count) {
		return options[i].text;
	} else {
		return NULL;
}	}

void die(const char *s) {
	fprintf(stderr, RED("%s\n"), s);
	exit(1);
}
