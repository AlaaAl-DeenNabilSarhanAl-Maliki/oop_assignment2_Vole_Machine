file(GLOB CLI_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# add_executable(vole_cli main.cpp ${CLI_SRC})

# target_include_directories(vole_cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(vole_cli PRIVATE VoleCore) 