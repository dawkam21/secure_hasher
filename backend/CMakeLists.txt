set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)

# nazwa projektu
project(CrowServer)

# ustawienie standardu C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Szukanie wątków (odpowiednik -lpthread)
find_package(Threads REQUIRED)

# 2. Szukanie OpenSSL (odpowiednik -lcrypto / -lssl)
find_package(OpenSSL REQUIRED)

# 3. Jeśli masz Crow jako bibliotekę systemową lub nagłówkową
# Jeśli używasz wersji "header-only", upewnij się, że ścieżka do crow_all.h jest znana
include_directories(/usr/local/include) # Zmień, jeśli masz Crow w innym miejscu

# Dodanie pliku wykonywalnego (twoja_nazwa_pliku.cpp)
add_executable(app server.cpp)

# Łączenie bibliotek z Twoim programem
target_link_libraries(app 
    PRIVATE 
    Threads::Threads 
    OpenSSL::SSL 
    OpenSSL::Crypto
)