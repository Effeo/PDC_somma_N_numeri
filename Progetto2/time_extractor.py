import csv
import re

# Open the input and output files
with open('Progetto2Rettangolare.out', 'r') as infile, open('tempi_matrici_rettangolari.csv', 'w', newline='') as outfile:
    # Create a CSV writer
    writer = csv.writer(outfile)
    # Write the header row
    writer.writerow(['strategy', 'dimension', 'NThreads', 'Tempo (s)'])

    # Initialize variables
    dimension = None
    strategy = None
    nthreads = None
    tempo = None

    # Read the input file line by line
    for line in infile:
        # Remove leading/trailing whitespace
        line = line.strip()

        # Match and extract data
        if 'righe matrice' in line:
            dimension = int(re.search(r'righe matrice: (\d+)', line).group(1))
        elif 'strategia' in line:
            strategy = int(re.search(r'strategia (\d+)', line).group(1))
        elif 'numero di threads' in line:
            nthreads = int(re.search(r'numero di threads: (\d+)', line).group(1))
        elif 'tempo' in line:
            tempo = float(re.search(r'tempo: ([\d.]+)', line).group(1))

        # If all data is collected, write a row to the CSV file
        if dimension is not None and strategy is not None and nthreads is not None and tempo is not None:
            writer.writerow([strategy, dimension, nthreads, tempo])
            # Reset variables for the next row
            dimension = None
            strategy = None
            nthreads = None
            tempo = None