file(GLOB VOLE_CORE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB VOLE_CORE_HEADERS CONFIGURE_DEPENDS Include/*.h)

add_library(VoleCore STATIC ${VOLE_CORE_SOURCES} ${VOLE_CORE_HEADERS})

target_include_directories(VoleCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)

if(MSVC)
    target_compile_options(VoleCore PRIVATE /W4)
else()
    target_compile_options(VoleCore PRIVATE -Wall -Wextra)
endif()