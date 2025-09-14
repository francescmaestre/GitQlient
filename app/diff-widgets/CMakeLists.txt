file(GLOB SRC_FILES
    include/diff-widgets/*.h
    src/*.h
    src/*.cpp
    src/*.ui
)

add_library(diff-widgets STATIC
    ${SRC_FILES}
)

target_link_libraries(diff-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    commit-widgets
    core
    dialogs
    main-widgets
    AuxiliarCustomWidgets
)

target_include_directories(diff-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
