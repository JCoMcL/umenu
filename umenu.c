#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#define COLOR(s) "\e[96m" s "\e[39m"
#define BOLD(s) "\e[1m" s "\e[22m"
#define RED(s) "\e[91m" s "\e[39m"

struct Option {
	char key;
	char *text;
	struct Option *next;
};

typedef struct Option *(*outOfKeysCallback)(struct Option*);

char getKeyFromIndex(int i, const char *keyString) {
	/* traversing the string ourselves as it's slightly more effiecient than using strlen*/
	for(int j=0; j<i; j++) {
		if (keyString[j] == '\0')
			return '\0';
	}
	return keyString[i];
}

void freeOptions(struct Option *options) {
	struct Option *tmp;
	while (options != NULL) {
		tmp = options;
		options = options->next;
		free(tmp);
	}
}

void die(const char *s) {
	if(s) { fprintf(stderr, RED("%s\n"), s);}
	exit(1);
}

struct Option *getOptions(outOfKeysCallback ook, const char *keyString) {
	struct Option *options = NULL;
	struct Option *prev = NULL;
	char buf[BUFSIZ];
	for (int i = 0; fgets(buf, sizeof buf, stdin); i++) {
		/* remove trailing newline */
		char *p = strchr(buf, '\n');
		if (p)
			*p = '\0';

		char key = getKeyFromIndex(i, keyString);
		if (!key){
			if (ook == NULL)
				die(NULL);
			else
				return ook(options);
		}

		struct Option *curr = malloc(sizeof(struct Option));
		if (!curr) {
			if (options)
				freeOptions(options);
			die("Can't malloc");
		}
		curr->key = key;
		curr->text = strdup(buf);
		curr->next = NULL;

		if (prev)
			prev->next = curr;
		else
			options = curr; //first iteration; assign curr as the list head
		
		prev = curr;
	}
	return options;
}

struct Option *printOutOfKeysError(struct Option *options) {
	int optionCount=0;
	for (struct Option *curr=options; curr=curr->next; optionCount++){}
	fprintf(stderr, RED("Warning! Maximum options (") "%i" RED(") reached\n"), optionCount);
	return options;
}

void displayOptions(struct Option *options) {
	for(struct Option *o = options; o; o = o->next) {
		fprintf(stderr, BOLD("["COLOR("%c")"]") " %s\n", o->key, o->text);
	}
}

char getUserInput(void) {
	/* reopen stdin for user input */
	if (freopen("/dev/tty", "r", stdin) == NULL)
		die("Can't reopen tty.");
	
	struct termios saved_settings;
	tcgetattr(0, &saved_settings);

	struct termios required_settings = saved_settings;
	/* disable unwanted attributes */
	required_settings.c_lflag &= ~(
		ICANON| //reads input per line rather than per character
		ECHO //echoes the typed input
	); 
	required_settings.c_cc[VMIN]=1; //require minimum of 1 char

	tcsetattr(0, TCSANOW, &required_settings);

	char c;
	read(0, &c, 1);
	tcsetattr(0, TCSANOW, &saved_settings);
	return c;
}

const char *findOption(struct Option *options, char c) {
	struct Option *o;
	for (o = options; o != NULL && (o->key != c); o = o->next) {}
	if (o != NULL)
		return o->text;
	else
		return NULL;
}

static const struct option longopts[] = {
	{ "display", 1, 0, 'd' },
	{ "keys", 1, 0, 'k' },
	{ "no-newline", 0, 0, 'n' },
	{ "quit-on-full", 0, 0, 'q' },
	{ "skip", 0, 0, 's' },
	{ "no-skip", 0, 0, 'S' },
	{ "help", 0, 0, 'h' }
};

int main(int argc, char *argv[]) {
	// options
	const char *displayString = "";
	const char *keyString = "1234567890abcdefghijklmnoopqrstuvwxyz";
	outOfKeysCallback ook = printOutOfKeysError;
	const char *outputTerminator = "\n";
	bool skipIfOneOption = true;

	// arguments
	int ch;
	while ((ch = getopt_long(argc, argv, "d:k:nqsSh", longopts, NULL)) != EOF) {
		switch (ch) {
		case 'd':
			displayString = optarg;
			break;
		case 'k':
			keyString = optarg;
			break;
		case 'n':
			outputTerminator = "";
			break;
		case 'q':
			ook = NULL;
			break;
		case 's':
			skipIfOneOption = true;
			break;
		case 'S':
			skipIfOneOption = false;
			break;
		case 'h':
			printf("Usage: %s [options]\n", argv[0]);
			puts("Select a line from standard input with a keypress.\n");
			puts("  -d, --display <string>    The message to display above the menu (if not skipped)");
			puts("  -k, --key <string>        The keys for each repsective option (e.g. \"yn\", \"12345\")");
			puts("  -n, --no-newline          Do not print a newline after output");
			puts("  -q, --quit-on-full        Die upon running out of keys rather than trimming the input");
			puts("  -s, --skip                Don't prompt user if only one option exists. (default)");
			puts("  -S, --no-skip             Always prompt user, even if only one option exists");
			puts("  -h, --help                Display this help messagen");
			return 0;
		}
	}

	struct Option *options = getOptions(ook, keyString);

	if (! options)
		return 1;
	if (! options->next && skipIfOneOption) {
		printf("%s\n", options->text);
		return 0;
	}

	if (*displayString) 
		fprintf(stderr, "%s\n", displayString);
	displayOptions(options);
	const char *output;

	for (char c = getUserInput(); !(output=findOption(options, c)); c = getUserInput())
		fprintf(stderr, "Invalid Character: %c\n", c);
	fprintf(stdout, "%s%s", output, outputTerminator);
	freeOptions(options);
	return 0;
}
