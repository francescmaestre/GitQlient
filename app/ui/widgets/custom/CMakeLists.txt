file(GLOB SRC_FILES
    include/custom-widgets/*.h
    src/*.cpp
)

add_library(custom-widgets STATIC
    ${SRC_FILES}
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
