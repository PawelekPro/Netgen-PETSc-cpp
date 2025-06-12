add_executable(meshGen
        meshGen.cpp
)

include_directories(
        ${THIRD_PARTY_DIR}
        ${PROJECT_DIR}/src/Globals
)

target_link_libraries(meshGen PUBLIC
        Globals
        Model
        MeshCore
)

target_include_directories(meshGen PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_DIR}/src/Globals
        ${PROJECT_DIR}/src/Model
        ${PROJECT_DIR}/src/Mesh
)


