
import cgi
import html
import os

# Ruta al archivo de usuarios
resultados_file = "./usuarios.txt"

query_string = os.getenv('QUERY_STRING')

# Obtener los datos del formulario
form = cgi.FieldStorage()
satisfaccion = form.getvalue('satisfaccion')

# Actualizar el archivo de resultados
if satisfaccion:
    with open(resultados_file, "a") as file:
        file.write(f"{satisfaccion}\n")

# Leer los resultados
resultados = {"Muy Buena": 0, "Buena": 0, "Regular": 0, "Mala": 0}

if os.path.exists(resultados_file):
    with open(resultados_file, "r") as file:
        for line in file:
            line = line.strip()
            if line in resultados.keys():
                resultados[line] += 1

# Generar la página de resultados
http_begin = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Encuesta</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            background-color: #fff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            text-align: center;
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 20px;
        }
        .form-group {
            display: flex;
            justify-content: center;
            align-items: center;
            margin-bottom: 10px;
        }
        label {
            margin-right: 10px;
            color: #555;
        }
        input[type="radio"] {
            margin-right: 5px;
        }
        input[type="submit"] {
            background-color: #007BFF;
            color: #fff;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            margin-top: 20px;
        }
        input[type="submit"]:hover {
            background-color: #0056b3;
        }
        .results {
            margin-top: 20px;
            text-align: left;
        }
        .bar-container {
            display: flex;
            flex-direction: column;
            align-items: flex-start;
        }
        .bar {
            width: 0;
            height: 20px;
            margin: 5px 0;
            background-color: #007BFF;
            border-radius: 5px;
            transition: width 1s ease-in-out;
        }
        .label {
            margin-bottom: 5px;
            color: #333;
        }
    </style>
</head>
<body>
    <h1>Encuesta de Satisfacción</h1>
    <div class="container">
        <form action="/cgi/survey.py" method="get">
            <div class="form-group">
                <label for="satisfaccion">¿Cómo calificaría su experiencia?</label>
                <input type="radio" id="muy_buena" name="satisfaccion" value="Muy Buena">
                <label for="muy_buena">Muy Buena</label>
                <input type="radio" id="buena" name="satisfaccion" value="Buena">
                <label for="buena">Buena</label>
                <input type="radio" id="regular" name="satisfaccion" value="Regular">
                <label for="regular">Regular</label>
                <input type="radio" id="mala" name="satisfaccion" value="Mala">
                <label for="mala">Mala</label>
            </div>
            <input type="submit" value="Enviar">
        </form>
        <div class="results">
            <h2>Resultados de la Encuesta</h2>
            <div class="bar-container">
                <div class="label">Muy Buena: <span id="muy_buena_count">0</span></div>
                <div class="bar" id="muy_buena_bar"></div>
                
                <div class="label">Buena: <span id="buena_count">0</span></div>
                <div class="bar" id="buena_bar"></div>
                
                <div class="label">Regular: <span id="regular_count">0</span></div>
                <div class="bar" id="regular_bar"></div>
                
                <div class="label">Mala: <span id="mala_count">0</span></div>
                <div class="bar" id="mala_bar"></div>
            </div>
        </div>
    </div>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
        const results = """

http_end = """        const total = results["Muy Buena"] + results["Buena"] + results["Regular"] + results["Mala"];
            
            // Actualización de conteos
            document.getElementById('muy_buena_count').textContent = results["Muy Buena"];
            document.getElementById('buena_count').textContent = results["Buena"];
            document.getElementById('regular_count').textContent = results["Regular"];
            document.getElementById('mala_count').textContent = results["Mala"];
            
            // Cálculo de porcentajes
            const muyBuenaPercent = (results["Muy Buena"] / total) * 100;
            const buenaPercent = (results["Buena"] / total) * 100;
            const regularPercent = (results["Regular"] / total) * 100;
            const malaPercent = (results["Mala"] / total) * 100;
            
            // Actualización de barras
            document.getElementById('muy_buena_bar').style.width = muyBuenaPercent + '%';
            document.getElementById('buena_bar').style.width = buenaPercent + '%';
            document.getElementById('regular_bar').style.width = regularPercent + '%';
            document.getElementById('mala_bar').style.width = malaPercent + '%';
        });
    </script>
</body>
</html>"""

http_middle = "{"
for opcion, cantidad in resultados.items():
    http_middle += f'"{opcion}": {cantidad}, '
http_middle = http_middle.rstrip(', ') 
http_middle += "};"

print(http_begin)
print(http_middle)
print(http_end)


#print("<html><head><title>Encuesta de Satisfacción</title></head><body>")
#print("<h1>Resultados de la Encuesta</h1>")
#for opcion, cantidad in resultados.items():
#    print(f"<p>{opcion}: {cantidad}</p>")
#print("</body></html>")