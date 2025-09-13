file(GLOB SRC_FILES
    include/*.h
    src/*.cpp
    src/*.ui
)

add_library(ReferencesWidgets STATIC
    ${SRC_FILES}
)

target_link_libraries(ReferencesWidgets
    PUBLIC
    Qt::Core
    Qt::Widgets
    aux_widgets
)

target_include_directories(ReferencesWidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
