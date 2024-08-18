### Multithreaded Chatroom Server

#### Description:
This program implements a multithreaded chatroom server using TCP sockets. It allows multiple clients to connect and chat simultaneously. The server maintains a list of connected clients, sends welcome messages to new clients, and broadcasts messages when clients join or leave the chatroom. It supports commands `\list` to list connected users and `\bye` to disconnect from the chatroom.

#### Command-line Arguments:
1. Port: Port number on which the server listens for incoming connections.
2. Maximum Number of Clients: Maximum number of clients the server can handle simultaneously.
3. Timeout: Timeout option in seconds to automatically disconnect inactive clients.

#### Usage:
```bash
./chat_server <port> <max_clients> <timeout>
```

#### Features:
- Multithreaded: Handles each client connection in a separate thread to enable concurrent chatting.
- User Management: Maintains a list of connected clients and sends notifications when users join or leave.
- Command Support: Supports `\list` command to list connected users and `\bye` command to disconnect from the chatroom.
