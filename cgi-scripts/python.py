

import os

# Leer variables de entorno
query_string = os.getenv('QUERY_STRING')
request_method = os.getenv('REQUEST_METHOD')

# Imprimir encabezado HTTP

print("<html><body>")

# Mostrar las variables de entorno en el cuerpo HTML
print("<h1>SOY UN PYTHON CGI</h1>")
print("<h2>Variables de Entorno CGI</h2>")
print(f"<p>QUERY_STRING: {query_string}</p>")
print(f"<p>REQUEST_METHOD: {request_method}</p>")
print("</body></html>")