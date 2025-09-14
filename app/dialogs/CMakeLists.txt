file(GLOB SRC_FILES
    include/dialogs/*.h
    src/*.h
    src/*.cpp
    src/*.ui
)

add_library(dialogs STATIC
    ${SRC_FILES}
)

target_link_libraries(dialogs
    PUBLIC
    Qt::Core
    Qt::Widgets
    Qt::Network
    core

    AuxiliarCustomWidgets
    git
)

target_include_directories(dialogs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
