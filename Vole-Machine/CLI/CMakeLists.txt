file(GLOB VOLE_CLI_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB VOLE_CLI_HEADERS CONFIGURE_DEPENDS Include/*.h src/*.h)

add_executable(VoleCli ${VOLE_CLI_SOURCES} ${VOLE_CLI_HEADERS})

# ConsoleUI.h may live in CLI/Include or next to the .cpp in CLI/src: both are searched.
target_include_directories(VoleCli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

# VoleCore exposes Core/Include publicly, so no extra include path is needed for it.
target_link_libraries(VoleCli PRIVATE VoleCore)

if(MSVC)
    target_compile_options(VoleCli PRIVATE /W4)
else()
    target_compile_options(VoleCli PRIVATE -Wall -Wextra)
endif()