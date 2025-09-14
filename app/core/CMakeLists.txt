file(GLOB SRC_FILES
    include/core/cache/*.h
    include/core/graph/*.h
    include/core/system/*.h
    src/cache/*.cpp
    src/graph/*.cpp
    src/system/*.cpp
)

add_library(
    core
    STATIC
    ${SRC_FILES}
)

target_link_libraries(core
    PUBLIC
    Qt::Core
    Qt::Network
    QLogger
    git
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
