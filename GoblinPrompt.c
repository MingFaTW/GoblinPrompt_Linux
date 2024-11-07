#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_COUNT 64
#define MAX_PATH_COUNT 64
#define MAX_HUNGER 100 
#define MAX_COMMANDS 10

char *pathList[MAX_PATH_COUNT];  
int pathCount = 0;               

void handle_error(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
}

void handle_bye() {
    exit(0);
}

void handle_cd(char *args[]) {
    if (args[1] == NULL) {
        handle_error("cd: missing argument\n");
        return;
    }
    if (chdir(args[1]) != 0) {
        perror("cd failed");
    }
}

void handle_path(char *args[]) {
    pathCount = 0; 
    for (int i = 1; args[i] != NULL && pathCount < MAX_PATH_COUNT; i++) {
        pathList[pathCount++] = args[i];
    }
    pathList[pathCount] = NULL; 
}

char *find_executable(char *command) {
    for (int i = 0; i < pathCount; i++) {
        char fullPath[MAX_INPUT_SIZE];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", pathList[i], command);
        if (access(fullPath, X_OK) == 0) {
            return strdup(fullPath); 
        }
    }
    return NULL;  
}

typedef struct {
    char name[50];
    int hunger_level; 
} Pet;

Pet my_pet;
int pet_created = 0; 

void create_pet() {
    printf("Please enter your pet's name: ");
    fgets(my_pet.name, sizeof(my_pet.name), stdin);
    my_pet.name[strcspn(my_pet.name, "\n")] = '\0';

    my_pet.hunger_level = 50;
    pet_created = 1;
    printf("You have created a pet named %s with an initial hunger level of %d.\n", my_pet.name, my_pet.hunger_level);
}

void *update_hunger(void *arg) {
    while (1) {
        sleep(60);
        if (pet_created) {
            my_pet.hunger_level += 1;
            if (my_pet.hunger_level > MAX_HUNGER) {
                my_pet.hunger_level = MAX_HUNGER; 
            }
        }
    }
    return NULL;
}

void *update_breaktime(void *args){
    while (1) {
        sleep(3600);
        printf("Time to have a rest for our future clan!!!");
    }
    return NULL;
}

void handle_check_pet_status() {
    if (!pet_created) {
        printf("Please create a pet first! Use the 'pet' command to create a pet.\n");
        return;
    }

    printf("Pet Name: %s\n", my_pet.name);
    printf("Hunger Level: %d\n", my_pet.hunger_level);

    if (my_pet.hunger_level <= 0) {
        printf("%s is not hungry right now.\n", my_pet.name);
    } else if (my_pet.hunger_level > 0 && my_pet.hunger_level <= 50) {
        printf("%s is a bit hungry; consider feeding.\n", my_pet.name);
    } else {
        printf("%s is very hungry! Please feed immediately.\n", my_pet.name);
    }
}


void handle_feed_pet() {
    if (!pet_created) {
        printf("Please create a pet first! Use the 'pet' command to create a pet.\n");
        return;
    }

    if (my_pet.hunger_level > 0) {
        my_pet.hunger_level -= 20;
        if (my_pet.hunger_level < 0) my_pet.hunger_level = 0;
        printf("%s is full! Hunger level decreased to %d.\n", my_pet.name, my_pet.hunger_level);
    } else {
        printf("%s is not hungry and doesn't need feeding right now.\n", my_pet.name);
    }
}

void handle_good_man(int times){
    printf("A good goblin always get: \n");
    for(int i=0; i<times; i++){
        printf("Girls: I am sorry, but you are a good man.\n");
    }
    printf("Your confidentiality status is recovered!\n");
}

void handle_calculator(void *args){
    printf("Active Calculator model.\n");
    
}

void handle_goblin_rules() {
    printf("Goblin Rules:\n");
    printf("1. Goblins must avoid contact with humans, elves, and adventurers.\n");
    printf("2. Do not trust humans, no matter what they say.\n");
    printf("3. Stay hidden in shadows to avoid being seen.\n");
    printf("4. Protect the goblin territory at all costs.\n");
    printf("5. Only trade with trusted goblin merchants.\n");
    printf("6. Never reveal goblin secrets to outsiders.\n");
    printf("7. Remember: Safety in numbers. Always stick with the goblin tribe.\n");
    printf("8. Retreat if facing a heavily armed adventurer.\n");
    printf("9. Gather resources quietly and efficiently.\n");
    printf("10. Respect the goblin chief's commands above all else.\n");
}


time_t start_time;

void handle_show_time() {
    time_t current_time;
    time(&current_time);
    struct tm *local_time = localtime(&current_time);
    printf("------------------Time Area-----------------\n");
    printf("Current Date and Time: %04d-%02d-%02d %02d:%02d:%02d\n",
           local_time->tm_year + 1900,
           local_time->tm_mon + 1,
           local_time->tm_mday,
           local_time->tm_hour,
           local_time->tm_min,
           local_time->tm_sec);
    double elapsed_seconds = difftime(current_time, start_time);
    int hours = (int)(elapsed_seconds / 3600);
    int minutes = (int)((elapsed_seconds - hours * 3600) / 60);
    int seconds = (int)(elapsed_seconds - hours * 3600 - minutes * 60);
    printf("You have been working for %02d:%02d:%02d (HH:MM:SS)\n", hours, minutes, seconds);
    printf("------------We will defeat Elf!!!-----------\n");
}

void execute_command(char *args[], int in_fd, int out_fd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    } else if (pid == 0) {
        if (in_fd != STDIN_FILENO) {
            dup2(in_fd, STDIN_FILENO); 
            close(in_fd);
        }
        if (out_fd != STDOUT_FILENO) {
            dup2(out_fd, STDOUT_FILENO); 
            close(out_fd);
        }

        char *execPath = find_executable(args[0]);
        if (execPath == NULL) {
            handle_error("command not found\n");
            exit(EXIT_FAILURE);
        }

        if (execvp(execPath, args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        free(execPath);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

void parse_input(char *input, char *commands[]) {
    if (input == NULL || input[0] == '\0') {
        return;
    }
    char *token = strtok(input, "|");
    int i = 0;

    while (token != NULL) {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }
    commands[i] = NULL;
}

void parse_command(char *command, char *args[]) {
    int i = 0;
    char *token = strtok(command, " \n"); 
    while (token != NULL && i < MAX_ARG_COUNT - 1) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL; 
}

void execute_pipeline(char *commands[]) {
    int pipefd[2];
    pid_t pid;
    int i = 0;
    int prev_fd = -1;
    while (commands[i] != NULL) {
        if (pipe(pipefd) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { 
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            if (commands[i + 1] != NULL) {
                dup2(pipefd[1], STDOUT_FILENO);
            }

            close(pipefd[0]); 
            close(pipefd[1]);

            char *args[MAX_ARG_COUNT];
            parse_command(commands[i], args);
            execvp(args[0], args); 
            perror("exec failed"); 
            exit(EXIT_FAILURE);
        } else { 
            close(pipefd[1]);

            if (prev_fd != -1) {
                close(prev_fd);
            }

            prev_fd = pipefd[0];
            i++;
        }
    }
    while (wait(NULL) > 0); 
}

int main() {
    time(&start_time);
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARG_COUNT];
    char *commands[MAX_COMMANDS];
    int worksCounter = 0, level = 0;
    pathList[0] = "/bin";
    pathList[1] = "/usr/bin";
    pathCount = 2;

    pthread_t hunger_thread, breaktime_thread;
    pthread_create(&hunger_thread, NULL, update_hunger, NULL);
    pthread_create(&breaktime_thread, NULL, update_breaktime, NULL);

    printf("Welcome to GoblinPrompt!\n");
    printf("If you need help, please type 'help' to see the instructions!\n");
    while (1) {
        printf("GoblinPrompt> ");
        
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            perror("fgets failed");
            continue;
        }

        if (input[0] == '\0') {
            printf("No input provided. Please enter a command.\n");
            continue; 
        }
        worksCounter++;
        if(worksCounter % 15 == 0){
            ++level;
            printf("Congratulations! Your Level is UP to %d!!! Lets fight for our future and clan!\n", level);
        }
        char input_copy[MAX_INPUT_SIZE];
        strncpy(input_copy, input, MAX_INPUT_SIZE);

        input[strcspn(input, "\n")] = 0; 

        if (strcmp(input, "bye") == 0) {
            handle_bye();
        }

        int argc = 0;
        char *token = strtok(input, " ");
        while (token != NULL && argc < MAX_ARG_COUNT - 1) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;
        parse_input(input_copy, commands);
        if (args[0] == NULL) {
            printf("Empty command. Please try again.\n");
            continue;
        }
        if (commands[1] != NULL) {
            execute_pipeline(commands);
        } else {
            if (strcmp(args[0], "cd") == 0) {
                handle_cd(args);
                continue;
            }

            if (strcmp(args[0], "path") == 0) {
                handle_path(args);
                continue;
            }

            if (strcmp(args[0], "help") == 0) {
                printf("We Together STRONG!!!\n");
                printf("Available commands:\n");
                printf("  help         - Display this help message\n");
                printf("  rules        - Display the surviving rules for us\n");
                printf("  bye          - Exit the shell\n");
                printf("  cd <dir>     - Change directory to <dir>\n");
                printf("  path <paths> - Set executable search paths\n");
                printf("  showtime     - Shows the current date and time, and the duration you've been working since starting this session.\n");
                printf("  pet          - Create a new pet\n");
                printf("  check_pet    - Check the pet's status\n");
                printf("  feed         - Feed the pet to reduce hunger\n");
                printf("  goodman <n>  - Perform goodman action with n times (custom command)\n");
                printf("  <command>    - Execute specified command with optional arguments\n");
                printf("  <command> < <file> - Redirect input from <file>\n");
                printf("  <command> > <file> - Redirect output to <file>\n");
                printf("  |            - Use pipe to connect multiple commands\n");
                printf("\nUse 'help' to see this list again.\n");
                continue;
            }


            if (strcmp(args[0], "pet") == 0) {
                create_pet();
                continue;
            }

            if (strcmp(args[0], "check_pet") == 0) {
                handle_check_pet_status();
                continue;
            }

            if (strcmp(args[0], "feed") == 0) {
                handle_feed_pet();
                continue;
            }

            if (strcmp(args[0], "goodman") == 0) {
                int times = 1; 
                if (args[1] != NULL) {
                    times = atoi(args[1]);
                }
                handle_good_man(times); 
                continue;
            }

            if (strcmp(args[0], "showtime") == 0) {
                handle_show_time();
                continue;
            }

            if (strcmp(args[0], "rules") == 0) {
                handle_goblin_rules();
                continue;
            }

            int in_fd = STDIN_FILENO;
            int out_fd = STDOUT_FILENO;

            for (int i = 0; i < argc; i++) {
                if (strcmp(args[i], "<") == 0) {
                    if (args[i + 1] != NULL) {
                        in_fd = open(args[i + 1], O_RDONLY);
                        if (in_fd < 0) {
                            perror("input redirection failed");
                            return 1;
                        }
                        for (int j = i; j < argc - 2; j++) {
                            args[j] = args[j + 2];
                        }
                        argc -= 2;
                        args[argc] = NULL;
                        i--;
                    } else {
                        handle_error("input redirection requires a file\n");
                        return 1;
                    }
                } else if (strcmp(args[i], ">") == 0) {
                    if (args[i + 1] != NULL) {
                        out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (out_fd < 0) {
                            perror("output redirection failed");
                            return 1;
                        }
                        args[i] = NULL;
                        argc = i;
                    } else {
                        handle_error("output redirection requires a file\n");
                        return 1;
                    }
                }
            }

            execute_command(args, in_fd, out_fd);

            if (in_fd != STDIN_FILENO) close(in_fd);
            if (out_fd != STDOUT_FILENO) close(out_fd);
        }
    }

    pthread_join(hunger_thread, NULL);
    pthread_join(breaktime_thread, NULL);
    return 0;
}
