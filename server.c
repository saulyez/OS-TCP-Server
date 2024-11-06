#include <ctype.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFERSIZE 256
bool isOnline = false;
struct node *rules = NULL;
struct node *requests = NULL;
struct node {
    char ip[BUFFERSIZE];
    char port[BUFFERSIZE];
    struct node *matched_connections;
    struct node *next;
};

void get_input (char *buffer, int size);
void add_request(struct node **head, char *request);
void print_rules(struct node **rules_head);
bool only_digit(const char *str);
bool valid_port(const char *port);
bool valid_ports(const char *port);
bool valid_ip(const char *ip);
bool valid_ip_range (const char *ip_range);
bool check_valid_rules(const char *ip ,const char *port);
void delete_matched_connections(struct node **connections_head);
void delete_rules(struct node **head, const char *ip,const char *port);
void add_rule(struct node **head, char *new_ip, char *new_port);
void check_in_rule(struct node **head, char *ip, char *port);
void add_matched_connection(struct node *head, char *ip, char *port);
void free_list(struct node **head);
bool within_ip_range(const char *range, const char *ip);
bool within_ports(const char *port_range, const char *port);



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


void add_request(struct node **head, char *request) {
    struct node *new_node = (struct node*)malloc(sizeof(struct node));
    if (new_node == NULL) {
        perror("malloc");
        return;
    }
    // Copy the new rule into the node
    strcpy(new_node->ip, request);
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
void print_rules(struct node **rules_head) {
    struct node *p = *rules_head;
    while (p != NULL) {
        printf("Rule: %s %s\n", p->ip, p->port);
        struct node *queries = p->matched_connections;
        while (queries != NULL) {
            printf("Query: %s %s\n", queries->ip, queries->port);
            queries = queries->next;
        }
        p = p->next;
    }
}
void print_requests(struct node **request_head) {
    struct node *p = *request_head;
    while (p != NULL) {
        printf("%s\n", p->ip);
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

void add_matched_connection(struct node *head, char *ip, char *port) {
    struct node *new_node = (struct node*)malloc(sizeof(struct node));
    if (new_node == NULL) {
        perror("malloc");
        exit(1);
    }
    strncpy(new_node->ip, ip, BUFFERSIZE - 1);
    new_node -> ip[BUFFERSIZE - 1] = '\0';
    strncpy(new_node->port, port, BUFFERSIZE - 1);
    new_node -> port[BUFFERSIZE - 1] = '\0';

    new_node->next = head->matched_connections;
    head->matched_connections = new_node;
}

bool within_ports(const char *port_range, const char *port) {
    char temp[BUFFERSIZE];
    strncpy(temp, port_range, BUFFERSIZE - 1);
    temp[BUFFERSIZE - 1] = '\0'; // Ensure null-termination

    char *left = strtok(temp, "-");
    char *right = strtok(NULL, "-");

    // Validate range bounds
    if (!left || !right || !valid_port(left) || !valid_port(right)) {
        return false;
    }

    int left_int = atoi(left);
    int right_int = atoi(right);

    // Ensure the range is in proper order
    if (left_int > right_int) {
        return false;
    }

    // Convert port to an integer and validate
    if (!valid_port(port)) {
        return false;
    }
    int port_int = atoi(port);

    // Check if port is within range
    return (port_int >= left_int && port_int <= right_int);
}


bool within_ip_range(const char *range, const char *ip) {
    char temp[BUFFERSIZE];
    char temp_ip[BUFFERSIZE];
    strncpy(temp_ip, ip, BUFFERSIZE);
    strncpy(temp, range, BUFFERSIZE);
    char *left = strtok(temp, "-");
    char *right = strtok(NULL, "-");

    if (left == NULL || right == NULL || !valid_ip(left) || !valid_ip(right) || !valid_ip(ip)) {
        return false;
    }

    int left_ip[4], right_ip[4], target_ip[4];
    char *split;

    // Parse left IP
    split = strtok(left, ".");
    for (int i = 0; i < 4 && split != NULL; i++) {
        left_ip[i] = atoi(split);
        split = strtok(NULL, ".");
    }

    // Parse right IP
    split = strtok(right, ".");
    for (int i = 0; i < 4 && split != NULL; i++) {
        right_ip[i] = atoi(split);
        split = strtok(NULL, ".");
    }

    // Parse target IP
    split = strtok(temp_ip, ".");
    for (int i = 0; i < 4 && split != NULL; i++) {
        target_ip[i] = atoi(split);
        split = strtok(NULL, ".");
    }

    // Check if target_ip is within [left_ip, right_ip] including boundaries
    bool within_lower = true;
    bool within_upper = true;

    for (int i = 0; i < 4; i++) {
        if (target_ip[i] < left_ip[i]) {
            within_lower = false;
            break;
        } else if (target_ip[i] > left_ip[i]) {
            break;
        }
    }

    for (int i = 0; i < 4 && within_lower; i++) {
        if (target_ip[i] > right_ip[i]) {
            within_upper = false;
            break;
        } else if (target_ip[i] < right_ip[i]) {
            break;
        }
    }

    return within_lower && within_upper;
}

void check_in_rule(struct node **head, char *ip, char *port) {
    if (!check_valid_rules(ip, port)) {
        perror("Illegal IP address or port Specified");
    }

    struct node *current = *head;
    //TODO check its adding to all nodes. not the first one
    while (current != NULL) {
        bool inIp = false;
        bool inPort = false;

        // Check IP
        if (strchr(current->ip, '-')) {  // IP Range
            inIp = within_ip_range(current->ip, ip);
        } else {  // Exact IP
            inIp = (strcmp(current->ip, ip) == 0);
        }

        // Check Port
        if (strchr(current->port, '-')) {  // Port Range
            inPort = within_ports(current->port, port);
        } else {  // Exact Port
            inPort = (strcmp(current->port, port) == 0);
        }

        // If both IP and Port match the criteria
        if (inIp && inPort) {
            add_matched_connection(current, ip, port);
            printf("Connection accepted\n");
            return;
        } else {
            printf("Connection rejected\n");
        }

        current = current->next;
    }
}

void delete_matched_connections(struct node **connections_head) {
    //TODO
    if (*connections_head == NULL || (*connections_head)->matched_connections == NULL) return;

    struct node *current = (*connections_head)->matched_connections;

    while (current != NULL) {
        struct node *temp = current->next;
        free(current);
        current = temp;
    }

    (*connections_head)->matched_connections = NULL;

}
void free_list(struct node **head) {
    struct node *current = *head;
    while (current != NULL) {
        delete_matched_connections(&current);
        struct node *temp = current -> next;
        free(current);
        current = temp;
    }
}

void delete_rules(struct node **head, const char *ip, const char *port) {
    if (!check_valid_rules(ip, port)) {
        perror("Rule Invalid");
        return;
    }
    if (*head == NULL) {
        printf("Rule not found\n");  // No changes here
        return;
    }

    struct node *temp = *head, *prev = NULL;

    // Check if the head node needs to be deleted
    while (temp != NULL && strcmp(temp->ip, ip) == 0 && strcmp(temp->port, port) == 0) {
        *head = temp->next;
        delete_matched_connections(&temp);
        free(temp);
        temp = *head; // Move to the next node
        printf("Rules deleted\n");  // Keep here if only the head is deleted
        return;
    }

    // Traverse the list to find and delete a matching node
    while (temp != NULL) {
        if (strcmp(temp->ip, ip) == 0 && strcmp(temp->port, port) == 0) {
            if (prev != NULL) {             // Check if prev is not NULL
                prev->next = temp->next;    // Link previous node to temp's next
            }
            delete_matched_connections(&temp);
            free(temp);
            printf("Rules deleted\n");  // Removed duplication in traversal
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    // If we exit the loop without deleting, the rule wasn't found
    printf("Rule not found\n");  // No changes here
}

// Function to add a new rule to the end of the linked list
void add_rule(struct node **head, char *new_ip, char *new_port) {
    struct node *new_node = (struct node*)malloc(sizeof(struct node));
    if (new_node == NULL) {
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

void interactive_mode() {
    char command[BUFFERSIZE];
     // Main server loop
    while (isOnline) {

        get_input(command, BUFFERSIZE);
        add_request(&requests, command);

        if (strcmp(command, "R") == 0) {
            // Print all the requests if "R" is entered
            print_requests(&requests); // TODO Change
        } else if (command[0] == 'A' && command[1] == ' ') {
            // Handle the "A IP Port" command
            char new_ip[BUFFERSIZE] = {0};
            char new_port[BUFFERSIZE] = {0};
            char extra[BUFFERSIZE] = {0};
            int args = sscanf(command + 2, "%s %s %s", new_ip, new_port, extra);
            if (args == 2 && check_valid_rules(new_ip, new_port)) {
                add_rule(&rules, new_ip, new_port);
                printf("Rule added\n");
                //TODO check the printing
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
        } else if(command[0] == 'C' && command[1]== ' ') {
            //TODO Finish
            char new_ip[BUFFERSIZE] = {0};
            char new_port[BUFFERSIZE] = {0};
            char extra[BUFFERSIZE] = {0};
            int args = sscanf(command + 2, "%s %s %s", new_ip, new_port, extra);
            if (args == 2 && check_valid_rules(new_ip, new_port)) {
                check_in_rule(&rules, new_ip, new_port);
            } else {
                printf("Invalid Rule\n");
            }

        } else if (strcmp(command, "E") == 0) {
            isOnline = false;
        } else if (strcmp(command, "L") == 0) {
            //TODO Change Print rules and its connections
            print_rules(&rules);
        }
    }


}




int main (int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    // struct node *requests = NULL;
    if(argc == 2 && strcmp(argv[1],"-i") == 0) {
        isOnline = true;
        interactive_mode();
    }

    // Free allocated memory for the linked list
    free_list(&rules);
    free_list(&requests);

    return 0;
}
