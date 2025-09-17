file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.h
    src/*.cpp
)

file(GLOB_RECURSE UI_SRC ui/*.ui)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

source_group("Forms" FILES ${UI_SRC})

add_library(ref-widgets STATIC
    ${SRC_FILES}
    ${UI_SRC}
)

target_link_libraries(ref-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    dialogs
)

target_include_directories(ref-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
