include(${CMAKE_SOURCE_DIR}/cmake/GitQlientLibrary.cmake)

create_gitqlient_library(dialogs
    HAS_UI
    DEPENDENCIES
        Qt::Core
        Qt::Widgets
        Qt::Network
        core
        custom-widgets
        git
)
