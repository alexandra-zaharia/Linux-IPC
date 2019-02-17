cmake_minimum_required(VERSION 3.10)
project(Linux_IPC C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(
        ${CMAKE_SOURCE_DIR}
        routing_table_manager/include)

add_executable(server_no_multiplexing utils.c unix_domain_sockets/no_multiplexing/server.c)
add_executable(server_multiplexing
        utils.c
        unix_domain_sockets/multiplexing/server.c
        unix_domain_sockets/multiplexing/fd_set_mgmt.c)
add_executable(client utils.c unix_domain_sockets/client.c)

set(RTM_DIR routing_table_manager)
set(RTM_SRC ${RTM_DIR}/src)
add_executable(rtm_server utils.c ${RTM_SRC}/rtm_server.c ${RTM_SRC}/rtm.c ${RTM_SRC}/input.c)
add_executable(rtm_client utils.c ${RTM_SRC}/rtm_client.c ${RTM_SRC}/rtm.c)
find_library(GCDS gcds HINTS ${RTM_DIR}/lib)
target_link_libraries(rtm_server PUBLIC ${GCDS})
target_link_libraries(rtm_client PUBLIC ${GCDS})

install(TARGETS server_no_multiplexing client DESTINATION unix_domain_sockets/no_multiplexing)
install(TARGETS server_multiplexing client DESTINATION unix_domain_sockets/multiplexing)

install(TARGETS rtm_server rtm_client DESTINATION routing_table_manager)
