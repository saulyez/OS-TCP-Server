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
    char port[BUFFERSIZE];
    // struct node *matchedconnections;
    struct node *next;
};

// Function to print all the rules in the linked list
void print_rules(struct node *p) {
    while (p != NULL) {
        printf("%s %s\n", p->rule, p->port);
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
bool valid_port(char *port) {
    //to finish
    if (!only_digit(port)) {
        return false;
    }
    int val = atoi(port);
    if (val < 1 || val > 65535) {
        return false;
    }
    return true;
}

bool valid_ip(char *rule) {
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
bool valid_ip_range (char *ip_range) {
    // xxx.xxx.xxx.xxx-xxx.xxx.xxx.xxx
    // xxx.xxx.xxx.xxx and xxx.xxx.xxx.xxx
    // run valid_ip on both
    // if both are valid, compare the values;
    // tokenize both into octets
    char temp[BUFFERSIZE];
    strncpy(temp, ip_range, BUFFERSIZE);
    char *left = strtok(temp, "-");
    char *right = strtok(NULL, "-");

    if(left == NULL || right == NULL || !valid_ip(left) || !valid_ip(right)) {
        return false;
    }
    char temp_left[BUFFERSIZE], temp_right[BUFFERSIZE];
    strncpy(temp_left, left, BUFFERSIZE);
    strncpy(temp_right, right, BUFFERSIZE);


    int left_ip[4], right_ip[4];
    char *split = strtok(temp_left, ".");

    for (int i = 0; i < 4 && split != NULL; i++) {
        left_ip[i] = atoi(split);
        split = strtok(NULL, ".");
    }
    if(split != NULL) {
        return false;
    }

    split = strtok(temp_right, ".");
    for (int i = 0; i < 4 && split != NULL; i++) {
        right_ip[i] = atoi(split);
        split = strtok(NULL, ".");
    }
    if (split != NULL) {
        return false;
    }

    for (int i = 0; i < 4; i++) {
        if (left_ip[i] < right_ip[i]){
            return true;
        } else if (left_ip[i] > right_ip[i]) {
            return false;
        }
    }
    return false;
}

// Function to add a new rule to the end of the linked list
void add_rule(struct node **head, char *new_rule, char *new_port) {
    struct node *new_node = (struct node*)malloc(sizeof(struct node));
    if (new_node == NULL) {
        perror("malloc");
        return;
    }

    // Copy the new rule into the node
    strcpy(new_node->rule, new_rule);
    strcpy(new_node->port, new_port);
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

    struct node *head = NULL;
    if(argc == 2 && strcmp(argv[1],"-i") == 0) {
        isOnline = true;
        printf("Server Running\n");
    }
    // Main server loop
    while (isOnline) {
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);

        // Remove the newline character
        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n') {
            command[len - 1] = '\0';
        }

        if (strcmp(command, "R") == 0) {
            // Print all the rules if "R" is entered
            print_rules(head);
        } else if (command[0] == 'A' && command[1] == ' ') {
            // Handle the "A IP Port" command
            char new_ip[BUFFERSIZE];
            char new_port[BUFFERSIZE];
            int args = sscanf(command + 2, "%s %s", new_ip, new_port);

            // if(args == 2 && valid_ip(new_rule) && valid_port(new_port)) {
            //     add_rule(&head, new_rule, new_port);
            //     printf("Rule added\n");
            // }
            if (args == 2 && strchr(new_ip,'-')) {
                if (valid_ip_range(new_ip) && valid_port(new_port)) {
                    add_rule(&head, new_ip, new_port);
                    printf("Rule added\n");
                } else {
                    printf("Invalid Rule\n");
                }
            }
            else if (args == 2 && !strchr(new_ip, '-')) {
                if (valid_ip(new_ip) && valid_port(new_port)) {
                    add_rule(&head, new_ip, new_port);
                    printf("Rule added\n");
                }
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
