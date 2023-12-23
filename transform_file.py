import pandas as pd
import numpy as np
import ast

df = pd.read_csv('./input_data/puzzle_info.csv')

# Convert the 'allowed_moves' column from a string representation of a dictionary to an actual dictionary
df['allowed_moves'] = df['allowed_moves'].apply(ast.literal_eval)

# Transform the data to a simpler format
transformed_data = []

for _, row in df.iterrows():
    puzzle_type = row['puzzle_type']
    for move, numbers in row['allowed_moves'].items():
        numbers_str = ' '.join(map(str, numbers)) # Convert list of numbers to space-separated string
        transformed_data.append([puzzle_type, move, numbers_str])

# Convert to a new DataFrame
transformed_df = pd.DataFrame(transformed_data, columns=['puzzle_type', 'move', 'numbers'])

transformed_df.to_csv('./input_data/transformed_puzzle_info.csv')