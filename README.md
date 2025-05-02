
<p align="center">
  <a>
    <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/8d/42_Logo.svg/1200px-42_Logo.svg.png" alt="Logo" width="200" height="200">
  </a>

  <p align="center">
    The moment you finally understand why URLs start with HTTP
    <br />
	</p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Makefile-8A2BE2">
  <img src="https://img.shields.io/badge/C-4682B4">
  <img src="https://img.shields.io/badge/Shell-2E8B57">
  <img src="https://img.shields.io/badge/Gcc-00FF00">
</p>

## ✨ What is webserv?
`webserv` is an HTTP server built from scratch in C++98. It’s made to work with real browsers and aims to support much of the HTTP/1.1 standard. This project tests your understanding of sockets, process management, server configuration, and handling HTTP requests.

## Features
- Handles multiple connections using `epoll()`.
- Non-blocking I/O.
- Works with real-world browsers.
- Supports HTTP methods: `GET`, `POST`, `DELETE`.
- Serves static files.
- File upload support.
- CGI support (e.g., PHP, Python).
- Multiple ports and virtual servers.
- NGINX-style configuration file.
- Default error pages.

## Requirements

- C++98 (compiled with `-Wall -Wextra -Werror -std=c++98`)
- No Boost or external libraries allowed.
- Must be 100% stable — no crashes allowed under any condition.
- Compatible with Linux and macOS (with some platform-specific constraints).

## 💻 Usage

Use `make` to compile the project.

Run it with:

```bash
./webserv ./conf/server.conf



