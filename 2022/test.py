import sys

def count_visible_trees(grid):
  # Initialize a counter for the number of visible trees
  num_visible = 0

  # Initialize a list to store the maximum height for each row and column
  max_heights = [0] * len(grid)

  # Iterate over each row and column of the grid
  for i in range(len(grid)):
    for j in range(len(grid[i])):
      # Check if the current tree has a height greater than the maximum height for its row or column
      if grid[i][j] > max_heights[i] or grid[i][j] > max_heights[j]:
        # If it does, increment the counter and update the maximum height for its row and column
        num_visible += 1
        max_heights[i] = grid[i][j]
        max_heights[j] = grid[i][j]

  return num_visible

with open(sys.argv[1]) as f:
  rows = f.readlines()

# Initialize the grid as a list of lists
grid = []

# Iterate over each row in the input
for row in rows:
  # Split the row into a list of strings
  grid_row = row.split()

  # Convert the strings into integers
  grid_row = list(map(int, grid_row))

  # Add the row to the grid
  grid.append(grid_row)

print(count_visible_trees(grid))
