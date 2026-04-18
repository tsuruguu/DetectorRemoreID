# common/lib/opendroneid/CMakeLists.txt
zephyr_library_named(opendroneid_lib)

# POPRAWIONA ŚCIEŻKA: wchodzimy głębiej do folderu libopendroneid
set(ODID_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libopendroneid/libopendroneid)

zephyr_library_include_directories(${ODID_SRC_DIR})

# POPRAWIONE NAZWY PLIKÓW: zgodnie z Twoim drzewem tree
zephyr_library_sources(
    ${ODID_SRC_DIR}/opendroneid.c
)

# Eksportujemy katalog nagłówków
zephyr_include_directories(${ODID_SRC_DIR})