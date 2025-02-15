# Install dependencies
git submodule add -f https://github.com/google/googletest.git third_party/googletest

# Build the code with Cmake, -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE to enable clangd
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -B ./build
cmake --build ./build
