# Works with Qt6 or Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)   # needed: the panels use Q_OBJECT, and the headers are in Include/
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

file(GLOB VOLE_GUI_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB VOLE_GUI_HEADERS CONFIGURE_DEPENDS Include/*.h)

# Headers are listed explicitly so AUTOMOC finds them even though they live in Include/.
add_executable(VoleGui ${VOLE_GUI_SOURCES} ${VOLE_GUI_HEADERS})

target_include_directories(VoleGui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)

target_link_libraries(VoleGui PRIVATE VoleCore Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32)
    set_target_properties(VoleGui PROPERTIES WIN32_EXECUTABLE ON)   # no console window
endif()

if(MSVC)
    target_compile_options(VoleGui PRIVATE /W4)
else()
    target_compile_options(VoleGui PRIVATE -Wall -Wextra)
endif()