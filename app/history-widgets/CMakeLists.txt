file(GLOB SRC_FILES
    include/history-widgets/*.h
    src/*.h
    src/*.cpp
    src/*.ui
)

add_library(history-widgets STATIC
    ${SRC_FILES}
)

target_link_libraries(history-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    core
    dialogs
    main-widgets
    ref-widgets
    AuxiliarCustomWidgets
)

target_include_directories(history-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
