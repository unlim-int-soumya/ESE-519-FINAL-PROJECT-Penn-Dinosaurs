cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(Dino_CODE C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(DinoGame)

pico_enable_stdio_usb(DinoGame 1)
pico_enable_stdio_uart(DinoGame 1)

pico_generate_pio_header(DinoGame ${CMAKE_CURRENT_LIST_DIR}/hsync.pio)
pico_generate_pio_header(DinoGame ${CMAKE_CURRENT_LIST_DIR}/vsync.pio)
pico_generate_pio_header(DinoGame ${CMAKE_CURRENT_LIST_DIR}/rgb.pio)

target_sources(DinoGame PRIVATE dino.c vga_graphics.c pwmsnd.c)

target_link_libraries(DinoGame PRIVATE pico_stdlib hardware_pio hardware_dma hardware_adc hardware_irq pico_time hardware_timer hardware_clocks hardware_pwm)

pico_add_extra_outputs(DinoGame)