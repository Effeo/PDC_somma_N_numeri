import pandas as pd
import matplotlib.pyplot as plt

'''
# Load the data
df = pd.read_csv('tempi_matrici_rettangolari.csv', names=['strategy', 'dimension', 'NThreads', 'Tempo (s)'])

# Group the data by 'strategy' and 'NThreads'
groups = df.groupby(['strategy', 'NThreads'])

# For each group
for (strategy, nthreads), group in groups:
    # Create a plot
    plt.figure()
    plt.plot(group['dimension'], group['Tempo (s)'])
    plt.xlabel('Dimension')
    plt.ylabel('Time')
    plt.title(f'Strategy {strategy}, NThreads {nthreads}')
    plt.legend([f'Strategy {strategy}, NThreads {nthreads}'])

    # Save the plot as a PNG image
    plt.savefig(f'Strategy_{strategy}_NThreads_{nthreads}.png')
'''

'''
# Load the data
df = pd.read_csv('tempi_matrici_quadrate.csv', names=['strategy', 'dimension', 'NThreads', 'Tempo (s)'])

# Group the data by 'NThreads'
groups = df.groupby('NThreads')

# For each group
for nthreads, group in groups:
    # Create a plot
    plt.figure()
    
    # Group by strategy and plot each one
    strategy_groups = group.groupby('strategy')
    for strategy, strategy_group in strategy_groups:
        plt.plot(strategy_group['dimension'], strategy_group['Tempo (s)'], label=f'Strategy {strategy}')
    
    plt.xlabel('Dimension')
    plt.ylabel('Time')
    plt.title(f'NThreads {nthreads}')
    plt.legend()

    # Save the plot as a PNG image
    plt.savefig(f'Comparison_NThreads_{nthreads}.png')
'''

'''
# Load the data
df = pd.read_csv('tempi_matrici_quadrate.csv')

# Calculate the sequential time for each strategy and dimension
df_sequential = df[df['NThreads'] == 1].set_index(['strategy', 'dimension'])['Tempo (s)']
print(df_sequential)
print(df[['strategy', 'dimension']].drop_duplicates())

# Define functions to calculate speedup and scaled speedup
def calculate_speedup(row):
    if (row['strategy'], row['dimension']) in df_sequential.index:
        return df_sequential.loc[row['strategy'], row['dimension']] / row['Tempo (s)']
    else:
        return None  # or some other value indicating missing data

def calculate_scaled_speedup(row):
    if (row['strategy'], row['dimension']) in df_sequential.index:
        return df_sequential.loc[row['strategy'], row['dimension']] / (row['Tempo (s)'] * row['NThreads'])
    else:
        return None  # or some other value indicating missing data

# Calculate the speedup and scaled speedup
df['Speedup'] = df.apply(calculate_speedup, axis=1)
df['Scaled Speedup'] = df.apply(calculate_scaled_speedup, axis=1)

# Group the data by 'NThreads'
groups = df.groupby('NThreads')

# For each group
for nthreads, group in groups:
    # Create a plot for speedup
    plt.figure()
    strategy_groups = group.groupby('strategy')
    for strategy, strategy_group in strategy_groups:
        plt.plot(strategy_group['dimension'], strategy_group['Speedup'], label=f'Strategy {strategy}')
    plt.xlabel('Dimension')
    plt.ylabel('Speedup')
    plt.title(f'NThreads {nthreads}')
    plt.legend()
    plt.savefig(f'Speedup_NThreads_{nthreads}.png')

    # Create a plot for scaled speedup
    plt.figure()
    for strategy, strategy_group in strategy_groups:
        plt.plot(strategy_group['dimension'], strategy_group['Scaled Speedup'], label=f'Strategy {strategy}')
    plt.xlabel('Dimension')
    plt.ylabel('Scaled Speedup')
    plt.title(f'NThreads {nthreads}')
    plt.legend()
    plt.savefig(f'Scaled_Speedup_NThreads_{nthreads}.png')
'''

'''
# Load the data
df = pd.read_csv('tempi_matrici_rettangolari.csv')

# Group the data by 'strategy'
strategy_groups = df.groupby('strategy')

# For each group
for strategy, group in strategy_groups:
    # Create a new plot
    plt.figure()
    
    # Plot 'NThreads' on the x-axis and 'Tempo (s)' on the y-axis
    plt.plot(group['NThreads'], group['Tempo (s)'])
    
    # Set the title and labels
    plt.title(f'Strategy {strategy}')
    plt.xlabel('NThreads')
    plt.ylabel('Tempo (s)')
    
    # Save the plot as an image
    plt.savefig(f'Strategy_{strategy}.png')
'''
# Load the data
df = pd.read_csv('tempi_matrici_quadrate.csv')

# Calculate the sequential time for each strategy and dimension
df_sequential = df[df['NThreads'] == 1].set_index(['strategy', 'dimension'])['Tempo (s)']

# Define function to calculate speedup
def calculate_speedup(row):
    if (row['strategy'], row['dimension']) in df_sequential.index:
        return df_sequential.loc[row['strategy'], row['dimension']] / row['Tempo (s)']
    else:
        return None  # or some other value indicating missing data

# Define function to calculate efficiency
def calculate_efficiency(row):
    speedup = calculate_speedup(row)
    if speedup is not None:
        return speedup / row['NThreads']
    else:
        return None  # or some other value indicating missing data


# Calculate the efficiency
df['Efficiency'] = df.apply(calculate_efficiency, axis=1)

# Group the data by 'NThreads'
groups = df.groupby('NThreads')

# For each group
for nthreads, group in groups:
    # Create a plot for efficiency
    plt.figure()
    strategy_groups = group.groupby('strategy')
    for strategy, strategy_group in strategy_groups:
        plt.plot(strategy_group['dimension'], strategy_group['Efficiency'], label=f'Strategy {strategy}')
    plt.xlabel('Dimension')
    plt.ylabel('Efficiency')
    plt.title(f'NThreads {nthreads}')
    plt.legend()
    plt.savefig(f'Efficiency_NThreads_{nthreads}.png')