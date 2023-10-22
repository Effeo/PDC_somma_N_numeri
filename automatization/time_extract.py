import os

# Function to extract lines starting with a specific prefix
def extract_lines_with_prefix(file_path, prefix):
    file_name = os.path.basename(file_path).replace('.out', '')
    # Just skips result_job words
    values = file_name.split('_')[2:]
              #   s                p               n
    result = [values[0][1:2], values[1][1:2], values[2][1:]]

    with open(file_path, 'r') as file:
        for line in file:
            if line.startswith(prefix):
              result.append(line.split(':')[1].strip().split(' ')[0].strip()+'_')
    return result

directory_path = "outs/"
output_file = "extracted_times.txt"
file_extension = ".out"
prefix_to_search = "Tempo totale impiegato: "
extracted_lines = ['Strat,Procs,N,Time\n']

# Search for ".out" files and extract lines
for filename in os.listdir(directory_path):
    if filename.endswith(file_extension):
        file_path = os.path.join(directory_path, filename)
        r = extract_lines_with_prefix(file_path, prefix_to_search)
        extracted_lines.append(",".join(r) + '\n')

# Write extracted lines to the output file
with open(output_file, 'w') as outfile:
    outfile.writelines(extracted_lines)

print(f"Extracted lines have been written to {output_file}")