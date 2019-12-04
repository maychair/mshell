#include <stdio.h>
#include <readline/readline.h>

int main() {
  char *promty = "myshell$ ";
  char *input = readline(promty);
  printf("%s\n", input);
}
