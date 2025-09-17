file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.h
    src/*.cpp
)

file(GLOB_RECURSE UI_SRC ui/*.ui)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

source_group("Forms" FILES ${UI_SRC})

add_library(history-widgets STATIC
    ${SRC_FILES}
    ${UI_SRC}
)

target_link_libraries(history-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    core
    dialogs
    diff-widgets
    main-widgets
    ref-widgets
)

target_include_directories(history-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
