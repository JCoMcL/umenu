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
	struct Option *next;
};

struct Option *getOptions();
void displayOptions(struct Option *options);
const char *getOptionFromInput(void);
char getKeyFromIndex(int i);
char getUserInput(void);
const char *findOption(struct Option *options, char c);
void die(const char *s);
void freeOptions(struct Option *options);

// -n: no newline
// -q: quit if maximum options reached
// -s: (don't) skip if only one option is present
// -S: skip verbosely if only one options is present
// -d: display this string at the top of the menu if the menu is printed
// -c: use this string to map characters to options instead of the default getKeyFromIndex
int main() {
	struct Option *options = getOptions();

	//if there is only one option, output it and skip user selection
	if (! options->next) {
		printf("%s\n", options->text);
		return 0;
	}
	displayOptions(options);
	const char *output;

	for (char c = getUserInput(); !(output=findOption(options, c)); c = getUserInput()) {
		fprintf(stderr, "Invalid Character: %c\n", c);
	}
	puts(output);
	freeOptions(options);
	return 0;
}

struct Option *getOptions() {
	struct Option *options = NULL;
	struct Option *prev = NULL;
	char buf[BUFSIZ];
	for (int i = 0; fgets(buf, sizeof buf, stdin); i++) {
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

		struct Option *curr = malloc(sizeof(struct Option));
		if (!curr) {
			if (options)
				{freeOptions(options);}
			die("Can't malloc");
		}
		curr->key = key;
		curr->text = strdup(buf);
		curr->next = NULL;

		if (prev)
			{prev->next = curr;}
		else
			{options = curr;} //first iteration; assign curr as the list head
		
		prev = curr;
	}
	return options;
}

void displayOptions(struct Option *options) {
	for(struct Option *o = options; o; o = o->next) {
		fprintf(stderr, BOLD("["COLOR("%c")"]") " %s\n", o->key, o->text);
	}
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
	struct Option *o;
	for (o = options; o != NULL && (o->key != c); o = o->next) {}
	if (o != NULL) {
		return o->text;
	} else {
		return NULL;
}	}

void die(const char *s) {
	fprintf(stderr, RED("%s\n"), s);
	exit(1);
}

void freeOptions(struct Option *options) {
	struct Option *tmp;
	while (options != NULL) {
		tmp = options;
		options = options->next;
		free(tmp);
	}
}
