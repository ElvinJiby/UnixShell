/*
    Unix Programming - Assignment 2
    31/03/2024
    Elvin Jiby

    A simple shell based on the Bash shell in Unix/Linux

    Basic Features:
    - Print a prompt (# character).
    - Read in a line from stdin.
    - This line will be a command followed by space separated arguments.
    - Execute the command with the given arguments.
    - Wait until command completes.
    - Print the prompt again and accept more commands.
    - Exit when EOF is received (Ctrl+D)
    - SIGINT signal (Ctrl+C)
    - Change Directory (enter command cd)
    - Redirect Builtin (use symbols like >,<,>>, etc.)

    NOTES:
    - when using cd, certain directory names with spaces in them (e.g. Downloads/My F o l d e r)
      do not require any double quotes or "\ " when specifying, like you would in the standard Bash shell (i.e cd "Downloads/My F o l d e r" or "My\ F\ o\ l\ d\ e\ r").
      Just the directory name is enough, no need for double quotes or single quotes
    - However commands like cat do not work with directory names with spaces, but regular names should work fine
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_INPUT_LEN 1024 // defines the maximum length for an input char array (I just assumed it wouldn't be bigger than 1024)

void parse(char *input, int *argc, char **args[]); // function that parses the input
void execute_command(int argc, char *args[]); // function that executes a command based on the given arguments
void printPrompt(); // function that prints the custom shell prompt
void signalHandler(int sig); // function that handles CTRL+C signal
void execute_CD(int argc, char *args[]); // function that handles "cd" command input
void accessDir(char *directory); // function that tries to access a given directory
int redirectToFile(char *input); // function that handles STDOUT redirection