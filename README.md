
# Basic HTTPS client

A basic https client that parses a URL, establishes TLS connection with the server, sends GET and HEAD requests and parses and saves the response.

Uses my [HTTP version](https://github.com/blr-ophon/min_httpclient) as a library for parsing.

Programmed entirely in C. Uses linux sockets API for TCP connection with the server.


## Requirements
- curl 
- openssl

## Building

Clone the project
```bash
  git clone --recurse-submodules https://github.com/blr-ophon/min_httpsclient
```
Compile using:

```bash
  cd min_httpsclient
  make
```

## Usage

```bash
  ./bin/min_https <url>
```
Then specify the request method.

When the server responds with a file, it is saved in the responses folder. 



