file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.cpp
)

add_library(
    system
    STATIC
    ${SRC_FILES}
    ${UI_SRC}
)

target_link_libraries(system
    PUBLIC
    Qt::Core
    Qt::Network
    QLogger
    git
    cache
)

target_include_directories(system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(BUILD_TESTING)
    target_include_directories(system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()
