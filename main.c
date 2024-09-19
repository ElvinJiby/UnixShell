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

#include "functions.h"

int main() {
    char input[MAX_INPUT_LEN]; // string that contains the user's input
    char **args = malloc(sizeof(char*)); // dynamic array that contains each argument provided in the input

    if (args == NULL) { // error handling for args array
        printf("Error: Could not dynamically allocate memory to args array.\n");
        exit(EXIT_FAILURE);
    }

    // SIGINT signal handler (for CTRL+C)
    signal(SIGINT, signalHandler);

    // Loop
    while (1) {
        /* Print prompt */
        printPrompt();

        /* Read input */
        if (fgets(input, MAX_INPUT_LEN, stdin) == NULL) { // Exit on EOF (Ctrl+D)
            printf("\n"); // here to make the OS shell is displayed on a new line instead of after the "#"
            break;
        }
        if (input[0] == '\n') continue; // Go to start of program if only Enter key is pressed

        /* Redirect STDOUT Checking */
        int stdoutCopy = dup(STDOUT_FILENO); // make a copy of the STDOUT "setting" to restore later
        if(redirectToFile(input) == -1) continue; // call function to redirect STDOUT, continue next iteration if error occurred
        
        /* Parse input into command and arguments */
        int argc = 0; // number of arguments
        input[strcspn(input, "\n")] = '\0'; // remove \n character at the end
        parse(input, &argc, &args); // parse input into arguments 
        
        /* Execute the command (given there are arguments) */
        if (argc > 0) {
            if (strcmp(args[0],"cd") == 0) { // if the first argument is the cd command, execute separate function for it
                execute_CD(argc,args);
            } else execute_command(argc,args); // otherwise execute the command given
        }

        dup2(stdoutCopy, STDOUT_FILENO); // redirect output back to STDOUT (whether or not it was redirected to a file in the first place)
        close(stdoutCopy);
    }

    if (args != NULL) free(args); // free dynamically allocated memory
    return 0;
}