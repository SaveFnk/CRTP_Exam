cmake_minimum_required(VERSION 3.0)

project(CRTP_Project)

add_executable(server src/server.c)

add_executable(main src/main.c src/actor.c src/producer.c src/consumer.c src/client.c)