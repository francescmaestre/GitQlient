file(GLOB SRC_FILES
    include/commit-widgets/*.h
    src/*.cpp
    src/*.ui
)


add_library(commit-widgets STATIC
    ${SRC_FILES}
)

target_include_directories(commit-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(commit-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    core
    dialogs
)
