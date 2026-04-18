# nrf7002_sniffer/CMakeLists.txt
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf7002_sniffer)

# 1. DODANO: Musisz wskazać CMake, gdzie znajduje się folder z biblioteką, 
# aby mógł przetworzyć tamtejszy plik CMakeLists.txt
add_subdirectory(../common/lib/opendroneid build_opendroneid)

# 2. Ścieżki do nagłówków
target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

# 3. Pliki źródłowe aplikacji
target_sources(app PRIVATE
    src/main.cpp
    src/wifi_scanner.c
    src/ble_scanner.c
)

# 4. Linkowanie bibliotek (najlepiej na samym końcu)
target_link_libraries(app PUBLIC opendroneid_lib)