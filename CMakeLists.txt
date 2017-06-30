cmake_minimum_required(VERSION 2.8)
project(logs-reader)

include_directories(".")

file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

add_compile_options(-std=c++11)

add_executable(logs-reader ${HEADERS} ${SOURCES})

# Boost
find_package(Boost 1.40 COMPONENTS program_options system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Threads
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(PUBLIC logs-reader "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(logs-reader "${CMAKE_THREAD_LIBS_INIT}")
endif()


target_link_libraries(logs-reader ${Boost_LIBRARIES})
