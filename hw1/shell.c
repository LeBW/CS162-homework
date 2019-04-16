#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "tokenizer.h"

#define BUFFERSIZE 1024

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
int cmd_pwd(struct tokens *tokens);
int cmd_cd(struct tokens *tokens);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_exit, "exit", "exit the command shell"},
  {cmd_pwd, "pwd", "print name of current/working directory"},
  {cmd_cd, "cd", "change current working directory"},
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) {
  exit(0);
}

int cmd_pwd(unused struct tokens *tokens) {
    char *buf = malloc(BUFFERSIZE);
    getcwd(buf, BUFFERSIZE);
    printf("%s\n", buf);
    return 1;
}

int cmd_cd(struct tokens *tokens) {
    char *token = tokens_get_token(tokens, 1);
    if (token == NULL)
        return 1;
    chdir(token);
    return 1;
}


/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive) {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

/* Resolve path  */
char* resolve_path(char *path) {
    char *is_exist = strchr(path, '/');
    if (is_exist != NULL)
        return path;
    char* result = malloc(BUFFERSIZE);
    char* env_path = getenv("PATH");
    char* temp_path = malloc(BUFFERSIZE);
    strcpy(temp_path, env_path);

    char *p = strtok(temp_path, ":");
    while (p != NULL) {
        strcpy(result, p);
        strcat(result, "/");
        strcat(result, path);

        if (access(result, F_OK) == -1) {
            p = strtok(NULL, ":");
        }
        else {
            return result;
        }
    }
    return NULL;
}

int main(unused int argc, unused char *argv[]) {
  init_shell();

  static char line[4096];
  int line_num = 0;

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin)) {
    /* Split our line into words. */
    struct tokens *tokens = tokenize(line);

    /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));

    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } else {
      /* REPLACE this to run commands as programs. */
      //fprintf(stdout, "This shell doesn't know how to run programs.\n");
      /* fork a child process and execute the program. */
      pid_t cpid = fork();
      if (cpid == 0) {
          // in child process.
          size_t length = tokens_get_length(tokens);
          char* path = tokens_get_token(tokens, 0);
          char* r_path = resolve_path(path);
          char* argv[length+1];
          int i = 0;
          for (; i < length; i++) {
              if (strcmp(tokens_get_token(tokens, i), "<") == 0) {
                  //redirect stdin to the file.
                  char* file_name = tokens_get_token(tokens, i+1);
                  if (file_name == NULL)
                      exit(127);
                  int in = open(file_name, O_RDONLY);
                  if (in == -1) {
                      printf("Error open file: %s\n", file_name);
                      exit(127);
                  }
                  // replace stdin with input file
                  dup2(in, STDIN_FILENO);
                  // close unused file descripters.
                  close(in);

                  for (int j = i; j <= length; j++) {
                      argv[j] = NULL;
                  }
                  execv(r_path, argv);
                  printf("Error execute the program.\n");
                  exit(127);
              }
              else if (strcmp(tokens_get_token(tokens, i), ">") == 0) {
                  //redirect stdout to the file.
                  char* file_name =  tokens_get_token(tokens, i+1);
                  if (file_name == NULL)
                      exit(127);
                  int out = creat(file_name, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                  if (out == -1) {
                      printf("Error open file: %s\n", file_name);
                      exit(127);
                  }
                  dup2(out, STDOUT_FILENO);
                  close(out);

                  for (int j = i; j <= length; j++) {
                      argv[j] = NULL;
                  }
                  execv(r_path, argv);
                  printf("Error execute the program.\n");
                  exit(127);
              }
              else {
                  argv[i] = tokens_get_token(tokens, i);
              }
          }
          argv[length] = NULL;

          execv(r_path, argv);
          printf("Error execute the program.\n");
          exit(0);
      }
      else {
          // in parent process.
          int status;
          wait(&status);
      }
    }

    if (shell_is_interactive)
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);

    /* Clean up memory */
    tokens_destroy(tokens);
  }

  return 0;
}
