cmake_minimum_required(VERSION 3.15)
project(thread_pool_toplevel VERSION 0.0.1 DESCRIPTION "Thread Pool library")

add_library(thread_pool thread-pool.cpp thread-pool.hpp)

set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)

set_target_properties(thread_pool PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(thread_pool PROPERTIES SOVERSION 1)

target_include_directories(thread_pool PRIVATE .)


install(TARGETS thread_pool
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

enable_testing()