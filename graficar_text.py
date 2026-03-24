import pandas as pd
import matplotlib.pyplot as plt
import re
import os

def extract_size_mb(filename_label):
    """
    Extrae el valor numérico del tamaño (en MB o GB) de una etiqueta de archivo 
    como 'FM_sources200MB' y lo convierte a MB.
    """
    # Regex para encontrar un número seguido de 'MB' o 'GB'
    match = re.search(r'(\d+)(MB|GB)$', filename_label, re.IGNORECASE)
    if match:
        size_val = int(match.group(1))
        unit = match.group(2).upper()
        
        # Convertir GB a MB para mantener una unidad consistente
        if unit == 'GB':
            return size_val * 1024 
        return size_val
    return None

def generar_grafico_comparativo_tamano(file_path_A, file_path_B, label_A, label_B, output_filename):
    """
    Carga dos CSV, extrae el tamaño del archivo (MB) de la columna 'file', 
    los combina y crea un gráfico de líneas comparando t_mean vs. file_size_mb.
    """
    
    # 1. Cargar y procesar datos
    
    dfs = {}
    for path, label in [(file_path_A, label_A), (file_path_B, label_B)]:
        try:
            df = pd.read_csv(path)
            
            # Crear la columna de tamaño en MB
            df['file_size_mb'] = df['file'].apply(extract_size_mb)
            
            # Eliminar filas donde no se pudo extraer el tamaño
            df.dropna(subset=['file_size_mb'], inplace=True)
            
            # Agregar la etiqueta de la estructura
            df['Estructura'] = label
            dfs[label] = df
            
        except FileNotFoundError:
            print(f"Advertencia: No se encontró el archivo {path}. Se omitirá esta estructura.")
            continue
        except pd.errors.EmptyDataError:
            print(f"Advertencia: El archivo {path} está vacío. Se omitirá esta estructura.")
            continue
        except Exception as e:
            print(f"Error procesando {path}: {e}")
            continue

    if len(dfs) < 2:
        print("Error: Se necesita al menos dos estructuras para la comparación.")
        return

    # Combinar los DataFrames procesados
    df_combined = pd.concat(dfs.values())
    
    # Asegurar tipos numéricos
    df_combined['file_size_mb'] = pd.to_numeric(df_combined['file_size_mb'])
    df_combined['t_mean'] = pd.to_numeric(df_combined['t_mean'])
    
    # 2. Crear Gráfico
    plt.figure(figsize=(10, 6))

    # Trazar los datos, agrupando por la etiqueta de la estructura
    for name, data in df_combined.groupby('Estructura'):
        # Ordenar por tamaño de archivo para que la línea se vea limpia
        data_sorted = data.sort_values(by='file_size_mb')
        
        plt.plot(data_sorted['file_size_mb'], data_sorted['t_mean'], 
                 marker='o', 
                 linestyle='-', 
                 label=name)

    # 3. Configurar y Guardar
    plt.title('Tiempo medio vs. Tamaño del Archivo')
    plt.xlabel('Tamaño del Archivo (MB)')
    plt.ylabel('Tiempo Medio (ns)')
    plt.legend()
    
    # Ajustar las etiquetas del eje X para mayor claridad
    unique_sizes = sorted(df_combined['file_size_mb'].unique())
    plt.xticks(unique_sizes, labels=[f'{s}MB' for s in unique_sizes])
    
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()

    # Guardar la figura
    plt.savefig(output_filename)
    print(f"\nGráfico guardado exitosamente como: {output_filename}")
    
# =================================================================
# === INSTRUCCIONES DE USO ===
# =================================================================

# 1. Reemplaza estas rutas de archivo con las rutas reales de tus dos CSV
ARCHIVO_ESTRUCTURA_A = 'data/experiment_FM-index_text_size.csv' 
ARCHIVO_ESTRUCTURA_B = 'data/experiment_RK_text_size.csv'

# 2. Define las etiquetas para la leyenda del gráfico
ETIQUETA_A = 'FM-Index'
ETIQUETA_B = 'Rabin-Karp'

# 3. Define el nombre del archivo de salida
NOMBRE_GRAFICO = 'data/graphic_text_size.png'

# Llama a la función
generar_grafico_comparativo_tamano(
    ARCHIVO_ESTRUCTURA_A, 
    ARCHIVO_ESTRUCTURA_B, 
    ETIQUETA_A, 
    ETIQUETA_B, 
    NOMBRE_GRAFICO
)