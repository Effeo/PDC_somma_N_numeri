import csv
import re

# Open the input and output files
with open('Progetto3.out', 'r') as infile, open('tempi_matrici.csv', 'w', newline='') as outfile:
    # Create a CSV writer
    writer = csv.writer(outfile)
    # Write the header row
    writer.writerow(['Matrix Size', 'Number of Processors', 'Total Time (s)'])

    # Initialize variables
    matrix_size = None
    num_procs = None
    total_time = None

    # Read the input file line by line
    for line in infile:
        # Remove leading/trailing whitespace
        line = line.strip()

        # Match and extract data
        if 'Running with matrix size' in line:
            match = re.search(r'Running with matrix size (\d+) and (\d+) processors', line)
            matrix_size = int(match.group(1))
            num_procs = int(match.group(2))
        elif 'Total time:' in line:
            total_time = float(re.search(r'Total time: ([\d.]+) seconds', line).group(1))

        # If all data is collected, write a row to the CSV file
        if matrix_size is not None and num_procs is not None and total_time is not None:
            writer.writerow([matrix_size, num_procs, total_time])
            # Reset variables for the next row
            matrix_size = None
            num_procs = None
            total_time = None