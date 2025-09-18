include(${CMAKE_SOURCE_DIR}/cmake/GitQlientLibrary.cmake)

create_gitqlient_library(custom-widgets
    HAS_UI
    DEPENDENCIES
        Qt::Core
        Qt::Widgets
        QLogger
)
