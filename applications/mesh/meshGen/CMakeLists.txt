add_executable(meshGen
        meshGen.cpp
)

include_directories(
        ${THIRD_PARTY_DIR}
        ${PROJECT_DIR}/src/Globals
)

target_link_libraries(meshGen PUBLIC
        ${PETSC_LINK_LIBRARIES}
        MPI::MPI_CXX
        Globals
)


target_compile_options(meshGen PUBLIC
        ${PETSC_CFLAGS_OTHER}
)

target_include_directories(meshGen PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PETSC_INCLUDE_DIRS}
        ${PROJECT_DIR}/src/Globals
)


