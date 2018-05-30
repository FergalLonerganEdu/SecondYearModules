#include <stdio.h>  // The Standard Input and Output library; functions for reading and writing data
#include <string.h> // The String library; functions for manipulating strings
#include <stdlib.h> // The Standard library; general purpose functions

struct word {      // define a word structure as:
    size_t length; // an unsigned integer (non-negative)
    char * text;   // a pointer to one or more characters
};

struct wordlist {              // define a wordlist as:
    size_t count;              // a count of words
    struct word * list[90000]; // a pointer to 90000 word pointers worth of memory
    size_t maxlen;             // the length of the longest word
} * words;                     // a pointer, named words, to a wordlist

void load(char * path) { // word loading function, requires a wordlist location argument
    char temp[30];       // 30 characters worth of memory for holding each word temporarily
    size_t index = 0;    // which word we're currently handling
    words->maxlen = 1;   // the maxlen value from our above-mentioned wordlist struct

    FILE * pFileWords;             // FILE struct (part of C itself) pointer
    pFileWords = fopen(path, "r"); // the fopen function opens a file and returns a pointer to it
    if (pFileWords == NULL) {      // if the file couldn't be opened for whatever reason
        printf("\nno wordlist found at %s\n", path);
        exit(1);                   // tell the user and exit the program
    }
    // the fscanf function scans a file (1st argument), according to format (2nd argument),
    // and saves the data (subsequent arguments), returning EOF (end of file) when applicable
    while (fscanf(pFileWords, "%s", temp) != EOF) {
        size_t length = strlen(temp); // the strlen function returns the length of a string
        if (words->maxlen < length) { // if the current word is longer than the previous longest
            words->maxlen = length;   // that should be the new maxlen
        }
        // the malloc function allocates memory on the heap, which is larger but slower
        // the size we're allocating here is enough to hold a word struct
        words->list[index] = malloc(sizeof(struct word));
        // now we allocate memory for the actual characters of each word
        // (char *) casts this memory as a pointer to one or more characters
        words->list[index]->text = (char *)malloc(length*sizeof(char)+1);
        strcpy(words->list[index]->text, temp); // strcpy copies a string, copying the 2nd arg to the 1st
        words->list[index]->length = length;    // each word also remembers its length
        index++; // words->list[index] is now occupied; thus we increment index by 1
    }
    words->count = index; // once all words have been read, the number of words == index
}

int play(int minlen) { // main game function, requires a minimum length argument
    printf("\n");

    srand(time(NULL));      // initialize the random number generator
    do {                    // main game loop
        struct word * word; // pointer, named word, to a word struct
        do {
            word = words->list[rand()%words->count]; // pointing to a random word
        } while (word->length < minlen);             // of at least minlen length

        int guess;
        int chances = 5;
        char wrong[6] = "";                  // incorrect guesses
        char found[word->length+1];          // found characters
        memset(found, '\0', word->length+1); // null terminate found
        memset(found, '_', word->length);    // otherwise fill with underscores

        // prints to the console, what you've found, remaining chances, and wrong guesses
        printf("solved %s | chances %i | wrong '%s'  ", found, chances, wrong);
        do {                     // loop for each ...
            guess = getchar();   // get the next character from the console
            if (guess == '\n') { // on enter, show our game state again
                printf("solved %s | chances %i | wrong '%s'  ", found, chances, wrong);
                continue;        // and move on to waiting for the next char
            }
            else if (guess == EOF) {
                printf("\n");    // when we encounter an end of file (Ctrl-D)
                return 0;        // abandon the game function
            }
            else if (guess < 97 || guess > 122) {
                continue;        // if it's not a lowercase ASCII char, ignore
            }
            else {               // if it is a lowercase ASCII char
                int i;
                for (i = 0; i < strlen(found); i++) { // loop through word
                    if (word->text[i] == guess) {     // comparing to each char
                        found[i] = guess;             // adding to found as needed
                    }
                }
                // if char guess is in neither found nor wrong
                // strchr returns the index of a char (2nd arg) in a string (1st arg)
                if (strchr(found, guess) == NULL && strchr(wrong, guess) == NULL) {
                    strncat(wrong, (char *)&guess, 5); // append guess to wrong
                    chances--;                         // reduce the remaining chances
                }
            }
        // loop so long as we're still missing characters and not out of chances
        } while (strchr(found, '_') != NULL && chances > 0);
        printf("solved %s | chances %i | wrong '%s'\n", word->text, chances, wrong);
        if (chances > 0) {
            printf("VICTORY!\n\n");
        } else {
            printf("DEFEAT!\n\n");
        }
        while (getchar() != '\n') { /* do nothing */ }; // clear build-up of bad chars
    } while(1); // loop forever
    return 0;
}

int main(int argc, char * argv[]) { // the main function is automatically run and accepts arguments from the command line
                                    // argc is a count of the arguments, argv is an array of arguments (space separated)
    char * path = "/usr/share/wordlist";
    long minlen = 3;                // the default wordlist location and minimum word length

    if (argc > 1) {                       // we've received arguments
        if (strcmp(argv[1], "-h") == 0) { // print help and exit if the first argument is '-h'
            printf("\nhangman command line arguments:\n\
                    \n-w path\n\tPath to a whitespace seperated list of words.\
                    \n-s size\n\tMinimum word size.\n");
            return 0;
        }
        else {
            int i = 1;
            do {                                       // looping through our arguments
                if (strcmp(argv[i], "-w") == 0) {      // if one of them is '-w'
                    if (i+1 < argc) { i++; }           // if there's another argument
                    else {
                        printf("\n-w requires a wordlist path\n");
                        return 1;
                    }
                    path = argv[i];                    // make that the path of our wordlist
                }
                if (strcmp(argv[i], "-s") == 0) {      // if one of them is '-s'
                    if (i+1 < argc) { i++; }           // if there's another argument
                    else {
                        printf("\n-s requires a minimum size\n");
                        return 1;
                    }
                    minlen = strtol(argv[i], NULL, 0); // make that our minimum length
                }
                i++;
            } while (i < argc);
        }
    }
    load(path);                   // call our load function (remember that?)
    if (minlen > words->maxlen) { // exit if the longest word is shorter than the requested minimum
        printf("\nsize must not be larger than the longest word, %u\n", words->maxlen);
        exit(1);
    }
    return play(minlen);          // otherwise call our play function, which loops infinitely
}
