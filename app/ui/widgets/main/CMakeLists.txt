file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.h
    src/*.cpp
)

file(GLOB_RECURSE UI_SRC ui/*.ui)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

source_group("Forms" FILES ${UI_SRC})

add_library(main-widgets STATIC
    ${SRC_FILES}
    ${UI_SRC}
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
    QPinnableTabWidget
)
