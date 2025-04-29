add_library(Model
        Model.cpp
)


target_link_libraries(Model PUBLIC
        GeometryCore
        MeshCore
)

target_include_directories(Model PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_DIR}/src/Geometry
        ${PROJECT_DIR}/src/Mesh
)
