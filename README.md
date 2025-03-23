# OS-TCP-Server
# Firewall Configuration Management

## Overview
This project implements a server and client system to manage firewall configurations. The server maintains a collection of firewall rules, handles requests for adding, deleting, and querying rules, and ensures high concurrency with no memory leaks. The client interacts with the server, sending commands and displaying results.

## Features
1. **Server**:
   - Maintain a list of firewall rules.
   - Keep track of IP addresses and ports matched with rules.
   - Process commands:
     - `R`: List all requests in order.
     - `A <rule>`: Add a firewall rule.
     - `C <IPAddress> <port>`: Check if a connection is allowed.
     - `D <rule>`: Delete a rule.
     - `L`: List all rules with associated queries.
     - Invalid commands respond with `Illegal request`.
   - Supports interactive and socket-based operation.

2. **Client**:
   - Send commands to the server and display responses.
   - Usage: `./client <server_ip> <server_port> <message>`

## Commands
### Server
- **Interactive Mode**:
  ```bash
  ./server -i
  ./server <port>
./client <serverHost> <serverPort> <command>
## Fire Rule Format
<IPAddresses> <ports>
147.188.193.0-147.188.194.255 21-22
## Example Usage
Start server at a port: ./server 2200
Client sends a message : ./client localhost 2200 A 147.188.192.41 443
