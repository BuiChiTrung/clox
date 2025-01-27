# Install dependencies
git submodule add -f https://github.com/google/googletest.git third_party/googletest

# Build the code with Cmake
cmake -B ./build
cmake --build ./build
