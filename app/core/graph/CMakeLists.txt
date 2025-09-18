include(${CMAKE_SOURCE_DIR}/cmake/GitQlientLibrary.cmake)

create_gitqlient_library(graph
    DEPENDENCIES
        Qt::Core
        Qt::Network
        QLogger
        git
)
