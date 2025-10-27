cmake_minimum_required(VERSION 3.16)
project(PBL2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static libs
set(SFML_STATIC_LIBRARIES TRUE)

# Tìm SFML
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window Network CONFIG)

# Source
file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

# Executable
add_executable(app ${SRC_FILES})

set_target_properties(app PROPERTIES WIN32_EXECUTABLE TRUE)

# Chỉ rõ thư mục include/ để tìm UI.h
target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link với SFML
target_link_libraries(app PRIVATE SFML::Graphics SFML::Window SFML::Network)
