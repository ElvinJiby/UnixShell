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

void parse(char *input, int *argc, char **args[]) { // function that handles the parsing of command-line arguments
    char *temp = strtok(input, " "); // get first token
    while (temp != NULL) {
        (*args)[*argc] = temp; // store argument in args array
        (*argc)++; // increment number of arguments
        *args = realloc(*args, ((*argc)+1)*sizeof(char*)); // dynamically reallocate memory to allow additional arguments
        temp = strtok(NULL, " "); // get next token
    }
    (*args)[*argc] = NULL; // null terminate last position in args array
}

void execute_command(int argc, char *args[]) { // Function that executes a given command
    pid_t child_pid;
    int child_status;

    child_pid = fork(); // create copy of current process
    if (child_pid == 0) {
        // Child process
        execvp(args[0], args); // execute the command and its arguments
        printf("Command not found\n"); // error message if the command isn't found
        exit(EXIT_FAILURE);
    } else {
        // Parent process waits for child to finish
        wait(&child_status);
    }
}

void printPrompt() { // Prints the custom shell prompt message
    time_t currTime;
    struct tm *localTime;
    char prompt[20]; // char array to store prompt string

    time(&currTime); //gets the current time
    localTime = localtime(&currTime); // converts current time into local time
    strftime(prompt,80,"[%d/%m %H:%M]# ",localTime); // formats the time into specifed format

    printf("%s", prompt); // print prompt
    fflush(stdout); // flushes stdout stream
}

void signalHandler(int sig) { // Handles SIGINT
    printf("\n");
    printPrompt();
}

void execute_CD(int argc, char *args[]) { // Executes the changing of directories command
    if (argc == 1) { // No arguments given, change to HOME directory
        char *homeDir = getenv("HOME");
        if (homeDir == NULL) {
            puts("cd: Error: HOME environment not set");
            return;
        }
        accessDir(homeDir);
    }
    else { // access the given directory
        char directory[MAX_INPUT_LEN] = "";

        // append arguments to the directory string        
        for (int i = 1; i<argc; i++) {
            strcat(directory, args[i]);
            if (i < argc - 1) strcat(directory, " "); // add a space between each "word"
        }

        accessDir(directory);
    }
}

void accessDir(char *directory) { // Function that handles the accessing of a directory
    if (chdir(directory) != 0) { // if the directory could not be accessed for some reason
        switch (errno) {
            case ENOENT:
                printf("cd: %s: No such file or directory\n", directory);                    break;
            case EACCES:
                printf("cd: %s: Permission denied\n", directory);
                break;
            default:
                perror("cd: Unknown Error");
                break;
        }
    }
}

int redirectToFile(char *input) { // Function that handles the redirection to a given destination
    char *redirect_char = strchr(input, '>'); // parse token until > is reached
    if (redirect_char != NULL) {
        char *fileName = strtok(redirect_char + 1, " "); // get the file name after the '>'

        // Redirect stdout to the given filename
        int f = open(fileName, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if (f == -1) { // error handling
            printf("Error: Unable to open file %s\n", fileName);
            return -1;
        }

        dup2(f, STDOUT_FILENO); // redirect the output from STDOUT to the file
        close(f);
        *redirect_char = '\0'; // replace '>' and filename with NULL terminator to not be parsed as arguments with the command
    }
    return 0;
}