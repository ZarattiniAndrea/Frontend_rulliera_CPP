cmake_minimum_required(VERSION 3.16)

# Nome del progetto e del linguaggio
project(Semaforo_QML_CPP LANGUAGES C CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cerco tutti i file sorgenti .cpp nella cartella src
file(GLOB SOURCES "src/*.cpp") # in questo modo aggiungo tutti i file .cpp in src

# Aggiungo il file oggetto sqlite3.c
set(SQLITE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c")
set(SQLITE_HDR "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h")

# Aggiungo i file per la libreria AZModbus
file(GLOB AZMODBUS_SOURCES "AZModbus/*.cpp") # Trova tutti i file .cpp in AZModbus
list(APPEND SOURCES ${AZMODBUS_SOURCES}) # Aggiungo i file trovati alla lista dei sorgenti

#Forzo la compilazione come linguaggio C
set_source_files_properties(${SQLITE_SRC} PROPERTIES LANGUAGE C)

# Creo l'eseguibile usando i file trovati
add_executable(Semaforo_QML_CPP ${SOURCES} ${SQLITE_SRC})

# includo directory per sqlite3.h
target_include_directories(Semaforo_QML_CPP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
# includo directory per AZModbus
target_include_directories(Semaforo_QML_CPP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/AZModbus")

target_link_libraries(Semaforo_QML_CPP PRIVATE ws2_32)

message(STATUS "SOURCES = ${SOURCES}")
message(STATUS "SQLITE_SRC = ${SQLITE_SRC}")