<p align="center">
  <a>
    <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/8d/42_Logo.svg/1200px-42_Logo.svg.png" alt="Logo" width="200" height="200">
  </a>

  <p align="center">
    El momento de entender porque las URLs empiezan con HTTP
    <br />
	</p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Makefile-8A2BE2">
  <img src="https://img.shields.io/badge/C-4682B4">
  <img src="https://img.shields.io/badge/Shell-2E8B57">
  <img src="https://img.shields.io/badge/Gcc-00FF00">
</p>

## &#x1F4CC; Index 
- [What is webserver?](#-what-is-inception)
- [Características](#-caracteristicas)
- [Requisitos](#-requisitos)
- [Usage](#-usage)

## &#x2728; What is webserver?
`webserv` es un servidor HTTP escrito desde cero en C++98. Está diseñado para ser compatible con navegadores reales y cumplir con gran parte del estándar HTTP 1.1. Este proyecto pone a prueba tu comprensión sobre sockets, manejo de procesos, configuración de servidores y procesamiento de peticiones HTTP.

## Características
- Manejo de múltiples conexiones con `poll()` (o `select()`, `epoll()`, `kqueue()`).
- No-blocking I/O.
- Compatible con navegadores reales.
- Soporte para métodos HTTP: `GET`, `POST`, `DELETE`.
- Servir archivos estáticos.
- Soporte para uploads.
- Soporte para CGI (ej. PHP, Python).
- Múltiples puertos y servidores virtuales.
- Archivos de configuración estilo NGINX.
- Manejo de errores con páginas por defecto.

## Requisitos

- C++98 (compilación con `-Wall -Wextra -Werror -std=c++98`)
- No se permite usar Boost ni librerías externas.
- Código 100% estable: no puede crashear bajo ninguna condición.
- Compatible con Linux y macOS (con restricciones específicas).


## &#x1F4BB; Usage

`make` to compile the project.

./webserv [archivo_de_configuración]

