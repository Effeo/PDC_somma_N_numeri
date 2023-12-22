import pandas as pd
import matplotlib.pyplot as plt

# Your data
data = {
    "Matrix Size": [100, 100, 500, 500, 1000, 1000, 2000, 2000],
    "Number of Processors": [1, 4, 1, 4, 1, 4, 1, 4],
    "Total Time (s)": [0.003183, 0.000712, 0.346667, 0.130989, 5.626958, 1.028714, 54.699814, 16.398171]
}

# Convert to DataFrame
df = pd.DataFrame(data)

# Separate data for 1 and 4 processors and reset index to align both DataFrames
df_1_processor = df[df["Number of Processors"] == 1].reset_index(drop=True)
df_4_processors = df[df["Number of Processors"] == 4].reset_index(drop=True)


# Calculate Speedup: Time for 1 processor / Time for 4 processors
speedup = df_1_processor['Total Time (s)'] / df_4_processors['Total Time (s)']

# Calculate Efficiency: Speedup / Number of Processors
efficiency = speedup / df_4_processors['Number of Processors']

# Add Speedup and Efficiency to df_4_processors DataFrame
df_4_processors = df_4_processors.assign(Speedup=speedup, Efficiency=efficiency)

# Print the dataframes to verify the data
print("Data for 1 processor:")
print(df_1_processor)
print("\nData for 4 processors:")
print(df_4_processors)

# Print calculated Speedup and Efficiency
print("\nCalculated Speedup:")
print(df_4_processors["Speedup"])
print("\nCalculated Efficiency:")
print(df_4_processors["Efficiency"])

# Plotting Performance Comparison
plt.figure(figsize=(10, 6))
plt.plot(df_1_processor["Matrix Size"], df_1_processor["Total Time (s)"], label='1 Processor', marker='o')
plt.plot(df_4_processors["Matrix Size"], df_4_processors["Total Time (s)"], label='4 Processors', marker='o')
plt.xlabel('Matrix Size')
plt.ylabel('Total Time (s)')
plt.title('Performance Comparison')
plt.legend()
plt.grid(True)
plt.savefig('performance_comparison.png')

# Plotting Speedup
plt.figure(figsize=(10, 6))
plt.plot(df_4_processors["Matrix Size"], df_4_processors["Speedup"], label='Speedup', marker='o')
plt.xlabel('Matrix Size')
plt.ylabel('Speedup')
plt.title('Speedup for 4 Processors')
plt.legend()
plt.grid(True)
plt.savefig('speedup_4_processors.png')

# Plotting Efficiency
plt.figure(figsize=(10, 6))
plt.plot(df_4_processors["Matrix Size"], df_4_processors["Efficiency"], label='Efficiency', marker='o')
plt.xlabel('Matrix Size')
plt.ylabel('Efficiency')
plt.title('Efficiency for 4 Processors')
plt.legend()
plt.grid(True)
plt.savefig('efficiency_4_processors.png')
