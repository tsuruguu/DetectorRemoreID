# nrf54_brain/CMakeLists.txt
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(remote_id_brain)

# Dodanie folderów include
target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

# Integracja Edge Impulse (ml_model) [5, 6]
# Pamiętaj o dodaniu wyeksportowanej biblioteki EI do projektu
add_subdirectory(ml_model)
target_link_libraries(app PUBLIC ei_model)

# Dodanie plików źródłowych
target_sources(app PRIVATE
    src/main.cpp
    src/data_aggregator.cpp
    src/ui_manager.cpp
)