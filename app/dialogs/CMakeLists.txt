file(GLOB SRC_FILES
    include/*.h
    src/*.cpp
    src/*.ui
)

add_library(GitQlientDialogs STATIC
    ${SRC_FILES}
)

target_link_libraries(GitQlientDialogs
    PUBLIC
    Qt::Core
    Qt::Widgets
    aux_widgets
)

target_include_directories(GitQlientDialogs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
