file(GLOB SRC_FILES
    include/*.h
    src/*.cpp
    src/*.ui
)

add_library(mainWidgets STATIC
    ${SRC_FILES}
)

target_include_directories(mainWidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(mainWidgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    core
    dialogs

    AuxiliarCustomWidgets
    diff
    commits
    history
    config
    QPinnableTabWidget
)
