cmake_minimum_required(VERSION 3.10)
project(InventoryManager)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for SQLite
include_directories(${CMAKE_SOURCE_DIR}/Dependencies/sqlite/include)

# Add executable
add_executable(inventory_manager
    InventoryManager/src/Application.cpp
    InventoryManager/src/DatabaseManager.cpp
    InventoryManager/src/InventoryManager.cpp
)

# Link SQLite library
target_link_libraries(inventory_manager ${CMAKE_SOURCE_DIR}/Dependencies/sqlite/lib/sqlite3.lib)

# Copy SQLite DLL to the build output directory on Windows
if(WIN32)
    add_custom_command(TARGET inventory_manager POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/Dependencies/sqlite/lib/sqlite3.dll"
            "$<TARGET_FILE_DIR:inventory_manager>"
    )
endif()
