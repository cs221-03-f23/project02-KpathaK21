#include "passwords.h"
#include "sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PASSWORD 64
#define DIG_BIN_LEN 32
#define DIG_STR_LEN ((DIG_BIN_LEN * 2) + 1)

// struct represetns entry in linked list. contains passwords, sha-256 hash and
// pointer to next entry
struct entry {
  char password[MAX_PASSWORD + 1];
  char hash[DIG_STR_LEN + 1];
  struct entry *next;
};

// function given by professor
void sha256_hash(char *dest, char *src) {
  struct sha256_ctx ctx;
  memset(&ctx, 0, sizeof(ctx));

  unsigned char dig_bin[DIG_BIN_LEN];
  memset(dig_bin, 0, DIG_BIN_LEN);

  memset(dest, 0, DIG_STR_LEN);

  __sha256_init_ctx(&ctx);
  __sha256_process_bytes(src, strlen(src), &ctx);
  __sha256_finish_ctx(&ctx, dig_bin);

  for (int i = 0; i < DIG_BIN_LEN; i++) {
    snprintf(dest, 3, "%02x", dig_bin[i]);
    dest += 2;
  }
}

// Transformation function
void transform_password(char *password) {
  for (int i = 0; i < strlen(password); i++) {
    switch (password[i]) {
    case 'o':
      password[i] = '0';
      break;
    case 'e':
      password[i] = '3';
      break;
    case 'i':
      password[i] = '!';
      break;
    case 'a':
      password[i] = '@';
      break;
    case 'h':
      password[i] = '#';
      break;
    case 's':
      password[i] = '$';
      break;
    case 't':
      password[i] = '+';
      break;
    }
  }
}

// Append '1' to the password
void add_one(char *password) { 
	strcat(password, "1"); // adds two stringa together (passwords + 1)
 }

// Function to insert a new entry into the sorted linked list
//
void insertSorted(struct entry **head, const char *password) {
  // Check if the entry already exists
  struct entry *current = *head;
  while (current != NULL) {
    if (strcmp(current->password, password) == 0) {
      return; // Entry already exists, don't insert
    }
    current = current->next;
  }

  // Entry does not exist, insert it
  struct entry *newEntry = (struct entry *)malloc(sizeof(struct entry));
  if (newEntry == NULL) {
    perror("Error allocating memory"); // used to print error only
    exit(1);
  }

  // copy the password into the input array so that it can be hashed till the
  // null character does not appear
  char input[MAX_PASSWORD + 1];
  strcpy(input, password);

  // compute the hash of the password
  char hash[DIG_STR_LEN + 1];
  sha256_hash(hash, input);
  if (hash == NULL) {
    perror("Error computing SHA-256 hash");
    exit(1);
  }

  strcpy(newEntry->hash, hash);
  strcpy(newEntry->password, password);
  newEntry->next = NULL;

  // make new entry head if new entry is smaller than current head
  if (*head == NULL || strcmp((*head)->hash, newEntry->hash) > 0) {
    newEntry->next = *head;
    *head = newEntry;
  } else {
    struct entry *current = *head;
    while (current->next != NULL &&
           strcmp(current->next->hash, newEntry->hash) <= 0) {
      current = current->next;
    }
    newEntry->next = current->next;
    current->next = newEntry;
  }
}

// Function to write the sorted linked list to a file
void writeDictionaryToFile(const char *filename, struct entry *head) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    perror("Error opening output file");
    exit(1);
  }

  int count = 0;

  struct entry *current = head;
  struct entry *temp = head;
  while (temp != NULL) {
    temp = temp->next;
    count++;
  }
  fprintf(file, "%d\n", count);

  while (current != NULL) {
    fprintf(file, "%s,%s\n", current->hash, current->password);
    current = current->next;
    count++;
  }

  fclose(file);
}

// Function to print the linked list
void print_list(struct entry **head) {
  const struct entry *current = *head;
  while (current != NULL) {

    // print the first 5 characters of  the hash ...
    printf("%s ", current->password);
    for (int i = 0; i < 5; i++) {

      printf("%c", current->hash[i]);
    }

    printf("...\n");

    current = current->next;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    printf("Usage: %s <password_file> <output_file> [-v]\n", argv[0]);
    return 1;
  }

  // Check if verbose option is enabled
  int verbose = 0;
  if (argc == 4 && strcmp(argv[3], "-v") == 0) {
    verbose = 1;
  }

  // opens file in rread mode with "r".
  FILE *passwordFile = fopen(argv[1], "r");
  if (passwordFile == NULL) {
    perror("Error opening password file");
    return 1;
  }

  struct entry *head = NULL;
  char line[MAX_PASSWORD];
  while (fgets(line, sizeof(line), passwordFile)) {
    // Remove trailing newline
    line[strcspn(line, "\n")] = '\0';
    // Insert the (password, hash) pair into the sorted linked list
    insertSorted(&head, line);
    if (verbose) {
      printf("inserting: %s\n", line);
      print_list(&head);
      printf("\n");
    }

    // Create and add l33t version
    char l33t_password[MAX_PASSWORD];
    strcpy(l33t_password, line);
    transform_password(l33t_password);
    // Implement your l33t transformation logic here
    // For example, you can replace 'a' with '4', 'e' with '3', etc.
    insertSorted(&head, l33t_password);
    if (verbose) {
      printf("inserting: %s\n", l33t_password);
      print_list(&head);
      printf("\n");
    }

    char plus1_password[MAX_PASSWORD];
    // Implement your plus1 transformation logic here
    strcpy(plus1_password, line);
    add_one(plus1_password);
    // append 1 to the end of the string
    insertSorted(&head, plus1_password);
    if (verbose) {
      printf("inserting: %s\n", plus1_password);
      print_list(&head);
      printf("\n");
    }
  }

  fclose(passwordFile);

  // Write the sorted linked list to the output file
  writeDictionaryToFile(argv[2], head);

  return 0;
}
