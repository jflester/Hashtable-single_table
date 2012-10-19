#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <stdlib.h>
#include "hashtable.h"

/* Cleans up the input - gets rid of whitespace at the start
   of a line */
static void clean_input_line(char *buffer){
  while (strlen(buffer) > 0 && buffer[0] == ' '){
    strcpy(&buffer[0], &buffer[1]);
  }
}

int main(int argc, char *argv[]){
  FILE *input;
  Table table;
  int size_line = 1024;
  char buffer[1024];
  char command[1024];
  char display[1024];
  char key[1024];
  char val[1024];
  int i;

  /* Initialize the table */
  init_table(&table);

  /* reads from stdin in no file specified */
  if (argc == 1){
    input = stdin;
  }
  /* opens specified file or throws error */
  else if (argc == 2){
    if ((input = fopen(argv[1], "r")) == NULL){
      perror("Error opening file. Try something else.\n");
      exit(EX_OSERR);
    }
  }
  else {
    perror("No valid file to open. Maybe make a file?\n");
    exit(EX_USAGE);
  }

  /* starts reading in the file/stdin line by line */
  while (fgets(buffer, size_line, input) != NULL){
    clean_input_line(buffer);
    sscanf(buffer, "%s", command);
    /* after cleaning line */
    /* skips comment lines */
    if (command[0] ==  '#'){
      continue;
    }
    /* skips new lines/blank lines */
    if (strcmp(buffer, "\n") == 0){
      sscanf(buffer, "\n\n%s", command);
    }
    /* If command is to insert into the table */
    else if (strcmp(command, "insert") == 0){
      if (sscanf(buffer, "%s %s %s", command, key, val) == 3){
	if (insert(&table, key, val) == 0){
	  printf("Insertion of %s => %s succeeded.\n", key, val);
	}
	else {
	  printf("Insertion of %s => %s failed.\n", key, val);
	}
      }
      else {
	perror("You entered invalid data. You bad person you.\n");
	exit(EX_DATAERR);
      }
    }
    /* If command is to search the table */
    else if (strcmp(command, "search") == 0){
      if (sscanf(buffer, "%s %s", command, key) == 2){
	if (search(&table, key, val) == 0){
	  printf("Search for %s succeeded %s.\n", key, val);
	}
	else {
	  printf("Search for %s failed.\n", key);
	}
      }
      else {
	perror("That input wasn't correct. Go think about what you did.\n"); 
	exit(EX_DATAERR);
      }
    }
    /*If command is to delete something from the table */
    else if (strcmp(command, "delete") == 0){
      if (sscanf(buffer, "%s %s", command, key) == 2){
	if (delete(&table, key) == 0){
	  printf("Deletion of %s succeeded.\n", key);
	}
	else {
	  printf("Deletion of %s failed.\n", key);
	}
      }
      else {
	perror("Your input was erroneous. You're not allowed to do that.\n");
	exit(EX_DATAERR);
      }
    }
    /* If command is to reset the table */
    else if (strcmp(command, "reset") == 0){
      if (sscanf(buffer, "%s", command) == 1){
	reset_table(&table);
	printf("Table reset.\n");
      }
      else {
	perror("The table was not reset. You didn't clear the utensils.\n");
	exit(EX_DATAERR);
      }
    }
    /* If command is to display item */
    else if (strcmp(command, "display") == 0){
      if (sscanf(buffer, "%s %s", command, display) == 2){
	/* display key count */
	if (strcmp(display, "key_count") == 0){
	  printf("Key count: %d\n", key_count(&table));
	}
	/* display table */
	if (strcmp(display, "table") == 0){
	  for (i = 0; i < NUM_BUCKETS; i++){
	    if (table.buckets[i].state == EMPTY){
	      printf("Bucket %d: EMPTY\n", i);
	    }
	    else if (table.buckets[i].state == DIRTY){
	      printf("Bucket %d: DIRTY\n", i);
	    }
	    else {
	      printf("Bucket %d: FULL (%s => %s)\n", i, 
		     table.buckets[i].data.key, table.buckets[i].data.value);
	    }
	  }
	}
      }
      else {
	perror("You can't display that. Are you even allowed to use this?\n");
	exit(EX_DATAERR);
      }
    }
    /* If command is completely invalid */
    else {
      perror("That was just wrong.\n");
      exit(EX_DATAERR);
    }
  }
  fclose(input);
  return 0;
}