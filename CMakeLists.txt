cmake_minimum_required(VERSION 3.15)
project(pose-bootcamp-code-quality-tutorial)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
macro(fetch_content_from_submodule DEPNAME RELPATH)
  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${RELPATH}/CMakeLists.txt")
    message(FATAL_ERROR "The git submodule ${RELPATH} is missing.\nTry running `git submodule update --init`.")
  endif()
  FetchContent_Declare(${DEPNAME}
    URL "${PROJECT_SOURCE_DIR}/${RELPATH}"
  )
  FetchContent_MakeAvailable(${DEPNAME})
endmacro()

fetch_content_from_submodule(Catch2 vendor/Catch2)

add_executable(address_book src/main.cpp src/address_book.cpp src/synchronization.cpp)
target_include_directories(address_book PRIVATE vendor)

add_executable(address_book_tests test/address_book_tests.cpp src/address_book.cpp src/synchronization.cpp)
target_include_directories(address_book_tests PRIVATE src vendor)
target_link_libraries(address_book_tests Catch2::Catch2WithMain)
