import pandas as pd
import matplotlib.pyplot as plt

# Load the data
df = pd.read_csv('tempi_matrici.csv', names=['Matrix Size', 'Number of Processors', 'Total Time (s)'])

# Calculate the sequential time for each matrix size
df_sequential = df[df['Number of Processors'] == 1].set_index('Matrix Size')['Total Time (s)']

# Define function to calculate speedup
def calculate_speedup(row):
    if row['Matrix Size'] in df_sequential.index:
        return df_sequential.loc[row['Matrix Size']] / row['Total Time (s)']
    else:
        return None  # or some other value indicating missing data

# Define function to calculate efficiency
def calculate_efficiency(row):
    speedup = calculate_speedup(row)
    if speedup is not None:
        return speedup / row['Number of Processors']
    else:
        return None  # or some other value indicating missing data

# Calculate the speedup and efficiency
df['Speedup'] = df.apply(calculate_speedup, axis=1)
df['Efficiency'] = df.apply(calculate_efficiency, axis=1)

# Filter data for number of processors 1 and 4
df = df[df['Number of Processors'].isin([1, 4])]

# Create a plot for total time
plt.figure()
for num_procs in [1, 4]:  # assuming you only have these two number of processors
    group = df[df['Number of Processors'] == num_procs]
    plt.plot(group['Matrix Size'], group['Total Time (s)'], label=f'NProcessors {num_procs}')
plt.xlabel('Matrix Size')
plt.ylabel('Total Time (s)')
plt.legend(loc='upper left')
plt.savefig('TotalTime.png')

# Create a plot for speedup
plt.figure()
for num_procs in df['Number of Processors'].unique():
    group = df[df['Number of Processors'] == num_procs]
    plt.plot(group['Matrix Size'], group['Speedup'], label=f'Number of Processors {num_procs}')
plt.xlabel('Matrix Size')
plt.ylabel('Speedup')
plt.legend()
plt.savefig('Speedup.png')

# Create a plot for efficiency
plt.figure()
for num_procs in df['Number of Processors'].unique():
    group = df[df['Number of Processors'] == num_procs]
    plt.plot(group['Matrix Size'], group['Efficiency'], label=f'Number of Processors {num_procs}')
plt.xlabel('Matrix Size')
plt.ylabel('Efficiency')
plt.legend()
plt.savefig('Efficiency.png')