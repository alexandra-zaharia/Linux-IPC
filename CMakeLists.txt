cmake_minimum_required(VERSION 3.10)
project(Linux_IPC C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(${CMAKE_SOURCE_DIR})
add_executable(server utils.c unix_domain_sockets/server.c)
add_executable(client utils.c unix_domain_sockets/client.c)
install(TARGETS server client DESTINATION unix_domain_sockets)