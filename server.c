#include <ctype.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 256
struct node {
    char rule[BUFFERSIZE];
    struct node *next;
};

// Function to print all the rules in the linked list
void print_rules(struct node *p) {
    while (p != NULL) {
        printf("%s\n", p->rule);
        p = p->next;
    }
}

bool only_digit(const char *str) {
    while (*str) {
        if (!isdigit(*str)) {
            return false;
        }
        str++;
    }
    return true;
}

bool valid_rule(char *rule) {
    char temp[BUFFERSIZE];
    strncpy(temp, rule ,BUFFERSIZE);
    char *split = strtok(temp, ".");
    int parts = 0;

    while (split != NULL) {
        if (!only_digit(split)) {
            return false;
        }
        int num = atoi(split);
        if (num < 0 || num > 255) {
            return false;
        }
        parts++;
        split = strtok(NULL, ".");
    }
    if (parts != 4) {
        return false;
    }
    return true;
}


// Function to add a new rule to the end of the linked list
void add_rule(struct node **head, char *new_rule) {
    struct node *new_node = (struct node*)malloc(sizeof(struct node));
    if (new_node == NULL) {
        perror("malloc");
        return;
    }

    // Copy the new rule into the node
    strcpy(new_node->rule, new_rule);
    new_node->next = NULL;

    // If the list is empty, make the new node the head
    if (*head == NULL) {
        *head = new_node;
    } else {
        // Otherwise, find the last node and append the new node
        struct node *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

bool isOnline = false;

int main (int argc, char **argv) {
    char command[BUFFERSIZE];

    struct node *head = NULL;  // Initialize the list of rules as empty
    if(argc == 2 && strcmp(argv[1],"-i") == 0) {
        isOnline = true;
        printf("Server Running\n");
    }
    // Main server loop
    while (isOnline) {
        // Use fgets to capture the entire input including spaces
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);

        // Remove the newline character at the end of the input if it exists
        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n') {
            command[len - 1] = '\0';
        }

        if (strcmp(command, "R") == 0) {
            // Print all the rules if "R" is entered
            print_rules(head);
        } else if (command[0] == 'A' && command[1] == ' ') {
            // Handle the "A <rule>" command
            char new_rule[BUFFERSIZE];
            // Copy the rest of the command as the new rule (skip the first two characters "A ")
            strncpy(new_rule, command + 2, sizeof(new_rule) - 1);
            new_rule[sizeof(new_rule) - 1] = '\0'; // Ensure null-termination

            if(valid_rule(new_rule)) {
                add_rule(&head, new_rule);
                printf("Rule added\n");
            }
            else {
                printf("Invalid Rule\n");
            }
        } else if (strcmp(command, "e") == 0) {
            isOnline = false;
        } else {
            printf("Command not recognised: %s\n", command);
        }
    }

    // Free allocated memory for the linked list
    struct node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
