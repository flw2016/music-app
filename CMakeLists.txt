cmake_minimum_required(VERSION 3.10.0)
project(app-for-music VERSION 0.1.0 LANGUAGES C CXX)

find_package(SQLite3 REQUIRED)

add_executable(app-for-music main.cpp)
target_link_libraries(app-for-music PRIVATE SQLite::SQLite3)