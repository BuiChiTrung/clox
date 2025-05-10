#!/bin/bash

# Create the output directory if it doesn't exist
mkdir -p demo/out

# Iterate over each file in the demo folder
for file in demo/*.lox; do
    echo "Running $file..."
    if [[ -f "$file" ]]; then
        # Get the base name of the file
        base_name=$(basename "$file")
        # Run the file and write the output to the corresponding file in demo/out
        build/main "$file" >"demo/out/$base_name.out"
    fi
done
