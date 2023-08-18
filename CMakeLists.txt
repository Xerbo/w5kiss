include(pico_sdk_import.cmake)
pico_add_library(w5kiss)

target_sources(w5kiss INTERFACE
    src/w5kiss.c
    src/w5kiss_netif.c
    ${PICO_SDK_PATH}/src/rp2_common/pico_lwip/lwip_nosys.c
)

target_include_directories(w5kiss INTERFACE
    include/
    ${PICO_SDK_PATH}/src/rp2_common/pico_lwip/include/
    ${PICO_SDK_PATH}/src/rp2_common/pico_async_context/include/
)

target_link_libraries(w5kiss INTERFACE
    pico_lwip
    pico_rand
    hardware_spi
)
