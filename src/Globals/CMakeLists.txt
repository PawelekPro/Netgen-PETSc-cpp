add_library(Globals
        Application.cpp
)

include_directories(
        ${THIRD_PARTY_DIR})

target_link_libraries(Globals PUBLIC
        ${PETSC_LINK_LIBRARIES}
        MPI::MPI_CXX
)


target_compile_options(Globals PUBLIC
        ${PETSC_CFLAGS_OTHER}
)

target_include_directories(Globals PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PETSC_INCLUDE_DIRS}
)
