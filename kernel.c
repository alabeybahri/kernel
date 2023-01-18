#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_CHARACTER 512
#define MAX_COMMAND_ARGUMENTS 16
#define MAX_LINE_CHARACTER 1024
#define NUMBER_OF_COMMANDS 6
#define MAX_HISTORY_COMMAND 15

// Initial print 
void printUser() {
    char *username = getenv("USER");
    printf("%s >>> ", username);
}
// Read line from stdin, in order to use line later I copied into commandLine  
int readCommand(char *unparsedCommand, char *commandLine) {
    char *line = NULL;
    size_t len = 0;
    ssize_t lineSize = 0;
    lineSize = getline(&line, &len, stdin);
    if (lineSize - 1 == 0) {
        return 1;
    }
    else {
        strcpy(unparsedCommand, line);
        strcpy(commandLine, line);
        return 0;
    }
    free(line);
}

// Parse the line with space delimiter
void parseCommand(char *unparsedCommand, char **parsedCommand) {
    // Remove the new line at the end of the unparsedCommand
    unparsedCommand = strsep(&unparsedCommand, "\n");
    for (int i = 0; i < MAX_COMMAND_ARGUMENTS; i++) {
        parsedCommand[i] = strsep(&unparsedCommand, " ");
        if (parsedCommand[i] == NULL)
            break;
        if (strlen(parsedCommand[i]) == 0)
            i--;
    }
}

// Crate child process with given command and its arguments
void executeBuiltInCommand(char **parsedCommand) {
    pid_t pid = fork();
    if (pid < 0) {
        return;
    } else if (pid == 0) {
        execvp(parsedCommand[0], parsedCommand);
        exit(0);
    } else {
        wait(NULL);
        return;
    }
}

// After printing the line, check if the first stdin input is enter
int readEnter() {
    char c;
    c = fgetc(stdin);
    if (c == 0x0A) {
        return 0;
    } else {
        return 1;
    }
}

// 
void readFile(char *fileName) {

    char *line = (char *) calloc(8, MAX_LINE_CHARACTER);
    FILE *filePointer = (FILE *) calloc(8, 128);
    char *command = (char *) calloc(8, MAX_COMMAND_CHARACTER);
    strcat(command, "less ");
    strcat(command, fileName);
    filePointer = popen(command, "r");
    while (fgets(line, MAX_LINE_CHARACTER, filePointer)) {
        int len = strlen(line);
        if (line[len - 1] == '\n') {
            line[len - 1] = 0;
        }
        printf("%s", line);
        if (readEnter()) {
            continue;
        }
    }
    free(command);
    free(line);
    pclose(filePointer);

}
// Check if the given command is in the history
int checkHistory(char history[MAX_HISTORY_COMMAND][MAX_COMMAND_CHARACTER], char *commandLine) {
    //String operations on line, in order to get what is asked in dididothat
    strsep(&commandLine, "\"");
    int len = strlen(commandLine);
    if (commandLine[len - 2] == '\"') {
        commandLine[len - 2] = '\0';
    }
    for (int i = 0; i < MAX_HISTORY_COMMAND; i++) {
        if (strcmp(history[i], commandLine) == 0) {
            return 1;
        }
    }
    return 0;
}
// This method is for putting methods in history correctly
// At first I only put the command names, so this is why this method is written afterwards
void getCalledFunctionAsString(char *historyCommandName, char **parsedCommand) {
    for (int i = 0; i < 5; i++) {
        if (parsedCommand[i] == NULL) {
            int len = strlen(historyCommandName);
            for (int k = 0; k < len; k++) {
            }
            historyCommandName[len - 1] = '\0';
            return;
        } else {
            strcat(historyCommandName, parsedCommand[i]);
            strcat(historyCommandName, " ");
        }
    }


}

// This method created for to use fork - execvp easily, 
// It takes parsed command and compares which command to execute
// It calls methods accordingly
void changeCommandName(char **parsedCommand, char *historyCommandName,
                       char history[MAX_HISTORY_COMMAND][MAX_COMMAND_CHARACTER],
                       char *commandLine) {

    getCalledFunctionAsString(historyCommandName, parsedCommand);

    char *customCommandNames[NUMBER_OF_COMMANDS];
    customCommandNames[0] = "listdir";
    customCommandNames[1] = "mycomputername";
    customCommandNames[2] = "whatsmyip";
    customCommandNames[3] = "printfile";
    customCommandNames[4] = "dididothat";
    customCommandNames[5] = "hellotext";
    customCommandNames[6] = "exit";

    // Check for the exit and if so terminate the shell
    if (strcmp(parsedCommand[0], customCommandNames[6]) == 0 & parsedCommand[1] == NULL) {
        exit(1);
    }
    for (int i = 0; i < NUMBER_OF_COMMANDS - 1; i++) {
        if (strcmp(parsedCommand[0], customCommandNames[i]) == 0) {
            switch (i) {
                case 0:
                    parsedCommand[0] = "ls";
                    executeBuiltInCommand(parsedCommand);
                    break;
                case 1:
                    parsedCommand[0] = "hostname";
                    executeBuiltInCommand(parsedCommand);
                    break;
                case 2:
                    parsedCommand[0] = "hostname";
                    parsedCommand[1] = "-I";
                    parsedCommand[2] = NULL;
                    executeBuiltInCommand(parsedCommand);
                    break;
                case 3:
                    if (parsedCommand[2] == NULL) {
                        readFile(parsedCommand[1]);
                    } else {
                        parsedCommand[0] = "cp";
                        parsedCommand[2] = parsedCommand[3];
                        parsedCommand[3] = NULL;
                        executeBuiltInCommand(parsedCommand);
                    }
                    break;
                case 4:
                    int iff;
                    iff = checkHistory(history, commandLine);
                    if (iff) {
                        printf("Yes\n");
                    } else {
                        printf("No\n");
                    }
                    break;
                case 5:
                    parsedCommand[0] = "editor";
                    executeBuiltInCommand(parsedCommand);
                    break;
                default:
            }

        }
    }

}


int main() {
    char unparsedCommand[MAX_COMMAND_CHARACTER];
    char commandLine[MAX_COMMAND_CHARACTER];
    char *parsedCommand[MAX_COMMAND_ARGUMENTS];
    char history[MAX_HISTORY_COMMAND][MAX_COMMAND_CHARACTER];
    int historyCounter = 0;
    //While for getting the commands 
    while (1) {
        char *historyCommandName = (char *) calloc(8, 128);
        printUser();
        if (readCommand(unparsedCommand, commandLine))
            continue;
        parseCommand(unparsedCommand, parsedCommand);
        changeCommandName(parsedCommand, historyCommandName, history, commandLine);
        strcpy(history[historyCounter % 15], historyCommandName);
        historyCounter++;
        free(historyCommandName);
    }
}