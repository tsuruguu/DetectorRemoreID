# common/lib/opendroneid/CMakeLists.txt
zephyr_library_named(opendroneid_lib)

set(ODID_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libopendroneid/libopendroneid)

# DODAJEMY wifi.c - tam jest funkcja odid_message_process_pack
zephyr_library_sources(
    ${ODID_SRC_DIR}/opendroneid.c
    ${ODID_SRC_DIR}/wifi.c
)

zephyr_library_include_directories(${ODID_SRC_DIR})
# Eksportujemy katalog nagłówków dla aplikacji
target_include_directories(app PUBLIC ${ODID_SRC_DIR})