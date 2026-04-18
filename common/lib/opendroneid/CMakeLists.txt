# common/lib/opendroneid/CMakeLists.txt
zephyr_library_named(opendroneid_lib)

# Ścieżka do plików źródłowych biblioteki opendroneid-core-c
set(ODID_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libopendroneid)

zephyr_library_include_directories(${ODID_SRC_DIR})

# Dodajemy tylko niezbędne pliki do parsowania ramek 
zephyr_library_sources(
    ${ODID_SRC_DIR}/opendroneid_core.c
    ${ODID_SRC_DIR}/opendroneid_version.c
)

# Eksportujemy katalog include dla reszty projektu
zephyr_include_directories(${ODID_SRC_DIR})