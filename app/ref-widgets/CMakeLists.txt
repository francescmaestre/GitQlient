file(GLOB SRC_FILES
    include/ref-widgets/*.h
    src/*.h
    src/*.cpp
    src/*.ui
)

add_library(ref-widgets STATIC
    ${SRC_FILES}
)

target_link_libraries(ref-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    dialogs
)

target_include_directories(ref-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
