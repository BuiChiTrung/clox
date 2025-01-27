# Set the directory where your .cpp files are located
set(SOURCE_DIR ".")

# Collect all .cpp files in the directory
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/ast/*.cpp" "${SOURCE_DIR}/utils/*.cpp")

# Create a library from all the .cpp files
add_library(clox ${SOURCES})

target_include_directories(clox PUBLIC ${PROJECT_SOURCE_DIR})
