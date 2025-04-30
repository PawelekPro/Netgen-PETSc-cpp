add_library(NetgenToDMPlex
        NetgenToDMPlex.cpp
        PETScNavierStokes.cpp
)

target_link_libraries(NetgenToDMPlex PUBLIC
        ${PETSC_LINK_LIBRARIES}
        MPI::MPI_CXX
        MeshCore
)


target_compile_options(NetgenToDMPlex PUBLIC
        ${PETSC_CFLAGS_OTHER}
)

target_include_directories(NetgenToDMPlex PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_DIR}/src/Mesh
        ${NETGEN_INCLUDE_DIRS}
        ${PETSC_INCLUDE_DIRS}
)
