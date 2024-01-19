cmake_minimum_required(VERSION 3.22.1)

project(Serveur)

# Spécifiez le chemin vers le répertoire Commune
set(COMMON_PATH ../Commune)

# Ajoutez les répertoires d'inclusion
include_directories(headers ${COMMON_PATH}/headers)
include_directories(headers)
set(SOURCES
    src/main.c
    src/Jouer.c
)

set(HEADERS
    headers/Jouer.h
    headers/serveur.h
)

# Spécifiez le répertoire où se trouve la bibliothèque Commune
link_directories(${COMMON_PATH}/build)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(Serveur ${SOURCES} ${HEADERS})

# Lier la bibliothèque Commune sans le drapeau -l
target_link_libraries(Serveur PRIVATE Commune)
