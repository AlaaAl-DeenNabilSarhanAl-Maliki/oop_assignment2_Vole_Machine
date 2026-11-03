file(GLOB CORE_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(VoleCore STATIC ${CORE_SRC})

target_include_directories(VoleCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)