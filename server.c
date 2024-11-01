#include <ctype.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 256
bool isOnline = false;
struct node {
    char ip[BUFFERSIZE];
    char port[BUFFERSIZE];
    struct node *matched_connections;
    struct node *next;
};

void get_input (char *buffer, int size);
void add_request(struct node **head, char *new_ip);
void print_rules(struct node *p);
bool only_digit(const char *str);
bool valid_port(const char *port);
bool valid_ports(const char *port);
bool valid_ip(const char *ip);
bool valid_ip_range (const char *ip_range);
bool check_valid_rules(const char *ip ,const char *port);
void check_in_rule(char *ip_port);
void delete_matched_connections(struct node *connections_head);
void delete_rules(struct node **head, const char *ip,const char *port);
void add_rule(struct node **head, char *new_ip, char *new_port);
bool check_in_rule(struct node **head, char *ip, char *port);



void get_input (char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        buffer[0] = '\0';
    }
}

// void print_requests(struct node *head) {
//  //TODO
//
// }

void add_request(struct node **head, char *new_ip) {
    struct node *new_node = (struct node*)malloc(sizeof(struct node));
    if (new_node == NULL) {
        perror("malloc");
        return;
    }
    // Copy the new rule into the node
    strcpy(new_node->ip, new_ip);
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


// Function to print all the rules in the linked list
void print_rules(struct node *p) {
    while (p != NULL) {
        printf("%s %s\n", p->ip, p->port);
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
bool valid_port(const char *port) {
    //to finish
    if (!only_digit(port)) {
        return false;
    }
    long int val = strtol(port, NULL, 10);
    if (val < 0 || val > 65535) {
        return false;
    }
    return true;
}

bool valid_ports(const char *port) {
    char temp[BUFFERSIZE];
    strncpy(temp, port,BUFFERSIZE);
    char *left = strtok(temp, "-");
    char *right = strtok(NULL, "-");

    if (!left || !right || !only_digit(left) || !only_digit(right)) {
        return false;
    }

    int left_int = atoi(left);
    int right_int = atoi(right);

    if (valid_port(left) && valid_port(right) && left_int <= right_int) {
        return true;
    }
    return false;
}

bool valid_ip(const char *ip) {
    char temp[BUFFERSIZE];
    strncpy(temp, ip ,BUFFERSIZE);
    char *split = strtok(temp, ".");
    int parts = 0;

    while (split != NULL) {
        if (!only_digit(split)) {
            return false;
        }
        long int num = strtol(split, NULL, 10);
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
bool valid_ip_range (const char *ip_range) {
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
        }
    }
    return false;
}

bool check_valid_rules(const char *ip ,const char *port) {
    if(ip == NULL || port == NULL) {
        return false;
    }
    bool is_valid_ip = false;
    bool is_valid_port = false;

    if (strchr(ip, '-')) {
        is_valid_ip = valid_ip_range(ip);
    } else {
        is_valid_ip = valid_ip(ip);
    }
    if(strchr(port, '-')) {
        is_valid_port = valid_ports(port);
    } else {
        is_valid_port = valid_port(port);
    }

    return is_valid_ip && is_valid_port;

}
void check_in_rule(char *ip_port) {
    //TODO
}
void delete_matched_connections(struct node *connections_head) {
    //TODO
}
void delete_rules(struct node **head, const char *ip, const char *port) {
    if (*head == NULL) {
        printf("Rule Invalid\n");
        return;
    }

    struct node *temp = *head, *prev = NULL;

    // Check if the head node needs to be deleted
    while (temp != NULL && strcmp(temp->ip, ip) == 0 && strcmp(temp->port, port) == 0) {
        *head = temp->next;
        free(temp);
        temp = *head;
        printf("Rules deleted\n");
        return;
    }

    // Traverse the list to find the matching node
    while (temp != NULL) {
        if (strcmp(temp->ip, ip) == 0 && strcmp(temp->port, port) == 0) {
            prev->next = temp->next;
            free(temp);
            printf("Rules deleted\n");
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    // If we exit the loop without deleting, the rule wasn't found
    printf("Rule Invalid\n");
}

// Function to add a new rule to the end of the linked list
void add_rule(struct node **head, char *new_ip, char *new_port) {
    struct node *new_node = (struct node*)malloc(sizeof(struct node));
    if (new_node == NULL) {
        perror("malloc");
        return;
    }
    // Copy the new rule into the node
    strcpy(new_node->ip, new_ip);
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


int main (int argc, char **argv) {
    char command[BUFFERSIZE];
    struct node *rules = NULL;
    struct node *requests = NULL;
    // struct node *requests = NULL;
    if(argc == 2 && strcmp(argv[1],"-i") == 0) {
        isOnline = true;
        printf("Server Running\n");
    }
    // Main server loop
    while (isOnline) {
        printf("Enter a command: ");

        get_input(command, BUFFERSIZE);
        add_request(&requests, command);

        if (strcmp(command, "R") == 0) {
            // Print all the requesrss if "R" is entered
            print_rules(requests); // TODO Change
        } else if (command[0] == 'A' && command[1] == ' ') {
            // Handle the "A IP Port" command
            char new_ip[BUFFERSIZE] = {0};
            char new_port[BUFFERSIZE] = {0};
            char extra[BUFFERSIZE] = {0};
            int args = sscanf(command + 2, "%s %s %s", new_ip, new_port, extra);
            if (args == 2 && check_valid_rules(new_ip, new_port)) {
                add_rule(&rules, new_ip, new_port);
                printf("Rules added\n");
            } else {
                printf("Invalid Rule\n");
            }
        } else if (command[0] == 'D' && command[1] == ' ') {
            char ip_delete[BUFFERSIZE] = {0};
            char port_delete[BUFFERSIZE] = {0};
            char extra[BUFFERSIZE] = {0};
            int args = sscanf(command + 2, "%s %s %s", ip_delete, port_delete, extra);
            if (args == 2 && check_valid_rules(ip_delete, port_delete)) {
                delete_rules(&rules, ip_delete, port_delete);
                //TODO Check printing bug and deleting
            } else {
                //TODO Check this
                printf("Invalid Rule\n");
            }
        } else if (strcmp(command, "E") == 0) {
            isOnline = false;
        } else if (strcmp(command, "L") == 0) {
            //TODO Change Print rules and its connections
            print_rules(rules);
        }
        else {
            printf("Command not recognised: %s\n", command);
        }
    }

    // Free allocated memory for the linked list
    struct node *temp;
    while (rules != NULL) {
        temp = rules;
        rules = rules->next;
        free(temp);
    }

    return 0;
}
