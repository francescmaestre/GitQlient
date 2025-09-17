file(GLOB SRC_FILES
    include/diff-widgets/*.h
    src/*.h
    src/*.cpp
)

file(GLOB_RECURSE UI_SRC
    src/*.ui)

source_group("Forms" FILES ${UI_SRC})

add_library(diff-widgets STATIC
    ${SRC_FILES}
    ${UI_SRC}
)

target_link_libraries(diff-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    commit-widgets
    core
    dialogs
    main-widgets
)

target_include_directories(diff-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
