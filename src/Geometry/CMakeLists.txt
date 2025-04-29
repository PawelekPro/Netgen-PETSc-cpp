add_library(GeometryCore
        GeometryLoader.cpp
        GeometryObject.cpp
)

if (WIN32)
    target_link_libraries(GeometryCore PUBLIC
            ${OpenCASCADE_LIBRARIES})
else ()
    target_link_libraries(GeometryCore PUBLIC
            ${OCC_LIBRARIES})
endif ()

target_include_directories(GeometryCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)