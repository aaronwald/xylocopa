cmake_minimum_required (VERSION 3.20)
project(xylocopa)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# installed with vcpkg - keep before other deps
# find_package(google_cloud_cpp_bigtable CONFIG REQUIRED)
find_package(libwebsockets CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(yaml CONFIG REQUIRED)
find_package(mosquitto REQUIRED)

# end finds (before fetchcontent)

# BEGIN mosquitto
find_package(mosquitto REQUIRED)
add_executable(bug "src/main/mqtt.cpp")
target_include_directories(bug PRIVATE ${MOSQUITTO_INCLUDE_DIRS})
target_include_directories(bug PRIVATE ${rapidjson_SOURCE_DIR}/include)
target_link_libraries(bug PRIVATE ${MOSQUITTO_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
# END mosquitto