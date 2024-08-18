### Multithreaded Music Streaming Server

#### Description:
This program implements a TCP server for streaming MP3 music to multiple clients simultaneously. It supports multiple streams from the same host by spawning independent threads for each client connection. The server displays the IP address of connected clients along with the requested song name on the console.

#### Command-line Arguments:
1. Port (P): Port number at which the server listens for incoming connections.
2. Root Directory (DIR): Directory containing the MP3 song files.
3. Maximum Number of Streams (N): Maximum number of simultaneous streams the server can support.

#### Usage:
```bash
./music_server <port> <root_directory> <max_streams>
```

#### Features:
- Multithreaded: Each client connection is handled in a separate thread to allow concurrent streaming.
- IP Display: The server console displays the IP address of connected clients along with requested song names.
- Simultaneous Streams: Supports multiple simultaneous streams from different clients.
