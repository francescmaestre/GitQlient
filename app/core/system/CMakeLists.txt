include(${CMAKE_SOURCE_DIR}/cmake/GitQlientLibrary.cmake)

set(DEPS
  Qt::Core
  Qt::Network
  Qt::Concurrent
  QLogger
  git
  cache
  graph
)

create_gitqlient_library(system
    DEPENDENCIES
    ${DEPS}
)
