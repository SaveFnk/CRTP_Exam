cmake_minimum_required(VERSION 3.0)

project(CRTP_Project)

add_executable(server server.c)
#target_compile_definitions(server 8080)


add_executable(main main.c)
#target_compile_definitions(main 4)
