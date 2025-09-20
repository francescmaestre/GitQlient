include(${CMAKE_SOURCE_DIR}/cmake/GitQlientLibrary.cmake)

set(DEPS
  Qt::Core
  Qt::Network
  QLogger
  git
  cache
)

create_gitqlient_library(system
    DEPENDENCIES
    ${DEPS}
)
