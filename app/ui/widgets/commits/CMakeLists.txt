file(GLOB SRC_FILES
    include/commit-widgets/*.h
    src/*.cpp
)

file(GLOB_RECURSE UI_SRC
    src/*.ui)

source_group("Forms" FILES ${UI_SRC})

add_library(commit-widgets STATIC
    ${SRC_FILES}
    ${UI_SRC}
)

target_include_directories(commit-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(commit-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    core
    dialogs
)
