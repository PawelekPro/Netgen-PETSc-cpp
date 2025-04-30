add_executable(1_navierStokesMMS NavierStokes_Example.cpp)
target_compile_options(1_navierStokesMMS PRIVATE ${PETSC_CFLAGS_OTHER})
target_link_libraries(1_navierStokesMMS PRIVATE
        ${PETSC_LINK_LIBRARIES}
        MPI::MPI_CXX
)