cmake_minimum_required(VERSION 3.20)
project("Chessodex")

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

file(GLOB_RECURSE CHSX_SRC_FILES "src/*.cpp")
file(GLOB_RECURSE CHSX_HDR_FILES "src/*.h")

add_executable(Chessodex ${CHSX_SRC_FILES} ${CHSX_HDR_FILES})

# Codex is using C++20.
set_property(TARGET Chessodex PROPERTY CXX_STANDARD 20)

# Link Codex.
add_subdirectory("vendor/Codex" ${CMAKE_BINARY_DIR}/Codex)
set(CODEX_INCLUDE_DIRS "vedor/Codex/Codex/include")
set(CODEX_LIBRARIES CodexEngine)
target_include_directories(Chessodex PRIVATE ${CODEX_INCLUDE_DIRS})
target_link_libraries(Chessodex CodexEngine)

# Installation.
install(TARGETS Chessodex DESTINATION bin)
