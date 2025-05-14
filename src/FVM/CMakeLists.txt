add_library(Fvm
        Globals.hpp
        FvmMesh.cpp
        FvmParam.cpp
        GeoCalc.cpp
        BndCond.cpp
        FvmMaterial.cpp
        FvmMeshToVtk.cpp
        FvmVar.cpp
        FvmSetup.cpp
        FvmVector.cpp
        ${THIRD_PARTY_DIR}/tinyxml2/tinyxml2.cpp
)

include_directories(
        ${THIRD_PARTY_DIR}
)

target_link_libraries(Fvm PUBLIC
        ${PETSC_LINK_LIBRARIES}
        ${VTK_LIBRARIES}
        MPI::MPI_CXX
        MeshCore
)


target_compile_options(Fvm PUBLIC
        ${PETSC_CFLAGS_OTHER}
)

target_include_directories(Fvm PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_DIR}/src/Mesh
        ${NETGEN_INCLUDE_DIRS}
        ${PETSC_INCLUDE_DIRS}
)
