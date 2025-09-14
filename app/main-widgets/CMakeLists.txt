file(GLOB SRC_FILES
    include/main-widgets/*.h
    src/*.h
    src/*.cpp
    src/*.ui
)

add_library(main-widgets STATIC
    ${SRC_FILES}
)

target_include_directories(main-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(main-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    core
    dialogs
    diff-widgets
    history-widgets

    AuxiliarCustomWidgets
    commits
    QPinnableTabWidget
)
