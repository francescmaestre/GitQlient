file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.cpp
)

file(GLOB_RECURSE UI_SRC
    ui/*.ui)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

source_group("Forms" FILES ${UI_SRC})

add_library(custom-widgets STATIC
    ${SRC_FILES}
    ${UI_SRC}
    ${RESOURCES}
)

target_include_directories(custom-widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(custom-widgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    PRIVATE
    QLogger
)
