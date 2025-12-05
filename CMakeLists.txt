cmake_minimum_required(VERSION 3.16)

# Nome del progetto e del linguaggio
project(Semaforo_QML_CPP LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cerco tutti i file .cpp nella cartella src
file(GLOB SOURCES "*.cpp")

# Aggiungo il file oggetto sqlite3.c
set(SQLITE_OBJ "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c")

# Creo l'eseguibile usando i file trovati
add_executable(Semaforo_QML_CPP ${SOURCES} ${SQLITE_OBJ})

# includo directory per sqlite3.h
target_include_directories(Semaforo_QML_CPP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
