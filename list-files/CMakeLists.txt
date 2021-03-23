set(CMAKE_CXX_STANDARD 20)

set(sources)

set(libs fmt cpp-terminal CLI11 git2)

add_executable(list-files ${sources} main.cpp)
target_link_libraries(list-files PRIVATE ${libs})

install(TARGETS list-files DESTINATION bin)
