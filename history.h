#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct history_node {
	int number;
	char *command;
	struct history_node* next;
} history_node;

void push_command(struct history_node** head_ref, char *command);
void print_command(struct history_node* node);
char *join_tokens(char **toks);