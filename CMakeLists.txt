cmake_minimum_required(VERSION 3.14)
project(EDA VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_EXTENSIONS OFF)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0" CACHE STRING "" FORCE)

include(cmake/CPM.cmake)
set(CPM_USE_LOCAL_PACKAGES OFF)
set(CPM_LOCAL_PACKAGES_ONLY OFF)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts-diagnostics-depth=100")
endif()

# Enable warnings for local code
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(OTTO_CXX_FLAGS "-Wall -Wno-long-long \
-Wno-c++14-compat -Wno-psabi \
-Wno-unknown-pragmas \
-Wno-vla \
-Wno-sign-compare \
-Wno-error=pedantic \
-Werror=unused-result \
")
# Don't remove unused-result, use [[maybe_unused]] to circumvent when necessary

elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(OTTO_CXX_FLAGS "-Wall -Wno-long-long \
-Wno-c++11-compat -Wno-c++98-compat -Wno-c++14-compat -Wno-c++17-compat -Wc++20-compat \
-Wno-shadow-field-in-constructor \
-Wno-documentation-unknown-command \
-Wno-unknown-pragmas \
-Wno-missing-braces \
-Wno-vla-extension \
-Wno-c99-extensions \
-Wno-gnu-zero-variadic-macro-arguments \
-Wno-gnu-anonymous-struct \
-Wno-nested-anon-types \
-Werror=unused-result \
")
# Don't remove unused-result, use [[maybe_unused]] to circumvent when necessary
endif()

include(packages.cmake)
add_subdirectory(list-files)
