
### Multithreaded HTTP Server

#### Description:
This program implements a multithreaded HTTP server that supports GET and POST requests. It serves static web pages along with common mime-types like HTML, CSS, JavaScript, and images. It also handles POST requests for counting characters, words, and sentences in supplied text.

#### Command-line Arguments:
1. Port: Port number at which the server listens for incoming HTTP requests.
2. Root Directory: Root directory where the files to serve are located.

#### Usage:
```bash
./webserver <port> <root_directory>
```

#### Features:
- Multithreaded: Handles each HTTP request in a separate thread to enable concurrent handling of multiple requests.
- Static Content Serving: Serves static web pages, CSS, JavaScript, and images.
- Error Handling: Serves a custom 404 page if the requested resource is not found.
- Default Page: Serves index.html by default if no webpage is mentioned in the request.

