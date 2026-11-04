
cmake_minimum_required(VERSION 3.20)

project(WebScraper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Create executable first
add_executable(WebScraper
    WebScraper.cpp
    WebScraper.h
    MySocket.cpp
)

# Link OpenSSL
target_link_libraries(WebScraper
    PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)