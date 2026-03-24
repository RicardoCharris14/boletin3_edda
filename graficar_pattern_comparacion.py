import pandas as pd
import matplotlib.pyplot as plt

FILE_A = 'data/experiment_FM-index_pattern_size.csv' 
FILE_B = 'data/experiment_RK_pattern_size.csv' 
OUTPUT_FILE = 'data/graphic_pattern_size.png' 
# Etiquetas de leyenda que aparecerán en el gráfico
LABEL_A = 'FM-index' 
LABEL_B = 'Rabin-Karp' 

# Carga los datos de ambas estructuras
df_A = pd.read_csv(FILE_A)
df_B = pd.read_csv(FILE_B)

# Agrega una columna para identificar la estructura en el gráfico
df_A['Structure'] = LABEL_A
df_B['Structure'] = LABEL_B

# Combina los DataFrames en uno solo
df_combined = pd.concat([df_A, df_B])

# Asegura que las columnas de interés sean numéricas
df_combined['pattern_length'] = pd.to_numeric(df_combined['pattern_length'])
df_combined['t_mean'] = pd.to_numeric(df_combined['t_mean'])


# --- 3. CREAR GRÁFICO ---
plt.figure(figsize=(10, 6))

# Usa un gráfico de líneas para comparar t_mean vs. pattern_length, agrupando por 'Structure'
for label, data in df_combined.groupby('Structure'):
    plt.plot(data['pattern_length'], data['t_mean'], label=label, marker='o')

# --- 4. CONFIGURAR Y GUARDAR ---
plt.title('Tiempo de Búsqueda vs. Longitud del Patrón')
plt.xlabel('Longitud del Patrón(carácteres)')
plt.ylabel('Tiempo Medio (ns)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.tight_layout()

plt.show()

# Guarda la figura en el archivo especificado
plt.savefig(OUTPUT_FILE)

print(f"Gráfico guardado como {OUTPUT_FILE}")