# nrf54_brain/sysbuild.cmake
set(SB_CONFIG_BOOTLOADER_MCUBOOT y CACHE BOOL "Enable MCUboot")

# Dodanie obrazu dla koprocesora RISC-V (FLPR)
set(FLPR_IMAGE_NAME hcsr04_coproc)
add_ext_image(
    NAME ${FLPR_IMAGE_NAME}
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src_flpr
    BOARD nrf54l15dk/nrf54l15/cpuflpr
)