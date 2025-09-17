file(GLOB SRC_FILES
    include/dialogs/*.h
    src/*.h
    src/*.cpp
)

file(GLOB_RECURSE UI_SRC
    src/*.ui)

source_group("Forms" FILES ${UI_SRC})

add_library(dialogs STATIC
    ${SRC_FILES}
    ${UI_SRC}
)

target_link_libraries(dialogs
    PUBLIC
    Qt::Core
    Qt::Widgets
    Qt::Network
    core
    custom-widgets
    git
)

target_include_directories(dialogs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
