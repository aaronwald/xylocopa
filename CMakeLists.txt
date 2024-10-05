cmake_minimum_required (VERSION 3.20)
project(xylocopa)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# BEGIN rapidjson
FetchContent_Declare(
  rapidjson
  GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
  GIT_SHALLOW    ON
#  GIT_TAG        v1.1.0
)
FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
endif()
# END rapidjson

# installed with vcpkg - keep before other deps
# find_package(google_cloud_cpp_bigtable CONFIG REQUIRED)
find_package(libwebsockets CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(yaml CONFIG REQUIRED)
find_package(mosquitto REQUIRED)
find_package(fmt CONFIG REQUIRED)

# end finds (before fetchcontent)

# BEGIN mosquitto
find_package(mosquitto REQUIRED)
add_executable(bug "src/main/mqtt.cpp")
target_include_directories(bug PRIVATE ${MOSQUITTO_INCLUDE_DIRS})
target_include_directories(bug PRIVATE ${rapidjson_SOURCE_DIR}/include)
target_link_libraries(bug PRIVATE ${MOSQUITTO_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto fmt::fmt)
# END mosquitto

# BEGIN mosquitto
find_package(mosquitto REQUIRED)
add_executable(send "src/main/mqttsend.cpp")
target_include_directories(send PRIVATE ${MOSQUITTO_INCLUDE_DIRS})
target_include_directories(send PRIVATE ${rapidjson_SOURCE_DIR}/include)
target_link_libraries(send PRIVATE ${MOSQUITTO_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto fmt::fmt)
# END mosquitto
