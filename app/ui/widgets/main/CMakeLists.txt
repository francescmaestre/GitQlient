include(${CMAKE_SOURCE_DIR}/cmake/GitQlientLibrary.cmake)

create_gitqlient_library(main-widgets
    HAS_UI
    DEPENDENCIES
        Qt::Core
        Qt::Widgets
        core
        dialogs
        diff-widgets
        history-widgets
        QPinnableTabWidget
)
