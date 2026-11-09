file(GLOB CLI_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB CLI_HEADERS CONFIGURE_DEPENDS Include/*.h)

add_executable(vole_console ${CLI_SOURCES} ${CLI_HEADERS})
target_include_directories(vole_console PRIVATE Include)
target_link_libraries(vole_console PRIVATE vole_core)