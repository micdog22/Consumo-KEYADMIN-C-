cmake_minimum_required(VERSION 3.20)
project(MicKeyAdminClient LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MicKeyAdminClient
    src/main.cpp
    src/HttpClient.cpp
    src/KeyAdminClient.cpp
)
if (WIN32)
  target_link_libraries(MicKeyAdminClient PRIVATE winhttp)
endif()
