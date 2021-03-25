file(GLOB SOURCES
    include/*.h
    src/*.cpp
)

find_package(OpenAL REQUIRED)

add_library(asound ${SOURCES})

target_link_libraries(asound ${OPENAL_LIBRARY})
target_include_directories(asound PRIVATE ${OPENAL_INCLUDE_DIR})
target_include_directories(asound PUBLIC include)

set_target_properties( asound
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}../../lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}../../lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}../../bin"
)

target_link_libraries(asound Qt5::Core)
