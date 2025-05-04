add_executable(002_NavierStokesExample 002_NavierStokesExample.cpp)
target_compile_options(002_NavierStokesExample PRIVATE ${PETSC_CFLAGS_OTHER})
target_link_libraries(002_NavierStokesExample PRIVATE
        ${PETSC_LINK_LIBRARIES}
        MPI::MPI_CXX
)


add_executable(001_StokesExample 001_StokesExample.cpp)
target_compile_options(001_StokesExample PRIVATE ${PETSC_CFLAGS_OTHER})
target_link_libraries(001_StokesExample PRIVATE
        ${PETSC_LINK_LIBRARIES}
        MPI::MPI_CXX
)