import pandas as pd
import matplotlib.pyplot as plt

# --- 1. CONFIGURACIÓN ---
# Archivo de datos a graficar
FILE_SINGLE = 'data/experiment_RK_pattern_size.csv' 
OUTPUT_FILE = 'data/pattern_size_RK.png' 
# Etiqueta de leyenda para la línea
LABEL_SINGLE = 'Rabin-Karp' 

# Nombres de las columnas que se usarán para X (eje horizontal) y Y (eje vertical)
X_COLUMN = 'pattern_length'
Y_COLUMN = 't_mean'

# --- 2. CARGA Y PREPARACIÓN DE DATOS ---
# Carga los datos del único archivo
try:
    df = pd.read_csv(FILE_SINGLE)
except FileNotFoundError:
    print(f"Error: No se encontró el archivo {FILE_SINGLE}. Asegúrate de que la ruta sea correcta.")
    exit()

# Asegura que las columnas de interés sean numéricas
try:
    df[X_COLUMN] = pd.to_numeric(df[X_COLUMN])
    df[Y_COLUMN] = pd.to_numeric(df[Y_COLUMN])
except KeyError:
    print(f"Error: El archivo CSV debe contener las columnas '{X_COLUMN}' y '{Y_COLUMN}'.")
    exit()
except ValueError:
    print(f"Error: Las columnas '{X_COLUMN}' y '{Y_COLUMN}' deben contener datos numéricos.")
    exit()

# --- 3. CREAR GRÁFICO ---
plt.figure(figsize=(10, 6))

# Usa un gráfico de líneas
plt.plot(df[X_COLUMN], df[Y_COLUMN], label=LABEL_SINGLE, marker='o', linestyle='-')

# --- 4. CONFIGURAR Y GUARDAR ---
plt.title('Tiempo de Búsqueda vs. Longitud del Patrón')
plt.xlabel('Longitud del Patrón (carácteres)')
plt.ylabel('Tiempo Medio (ns)')
plt.legend(loc='best') # Muestra la leyenda del LABEL_SINGLE
plt.grid(True, linestyle='--', alpha=0.7)
plt.tight_layout()

# Guarda la figura en el archivo especificado
plt.savefig(OUTPUT_FILE)

print(f"Gráfico guardado como {OUTPUT_FILE}")