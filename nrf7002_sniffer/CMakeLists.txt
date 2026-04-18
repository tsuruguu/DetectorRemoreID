cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf7002_sniffer)

# Dodajemy pliki źródłowe
target_sources(app PRIVATE 
    src/main.cpp 
    src/wifi_scanner.c 
    src/ble_scanner.c
)

# Nagłówki
target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

# Biblioteka OpenDroneID
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../common/lib/opendroneid build_opendroneid)
target_link_libraries(app PRIVATE opendroneid_lib)