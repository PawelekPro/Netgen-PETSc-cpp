cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(NetgenPETScPlugin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
find_package(PkgConfig)

MESSAGE("PROJECT NAME: ${PROJECT_NAME}")
MESSAGE("PROJECT DIRECTORY: ${PROJECT_DIR}")

# OPENCASCADE
if (WIN32)
    find_package(OpenCASCADE REQUIRED)
    if (OpenCASCADE_INCLUDE_DIR)
        include_directories(SYSTEM ${OpenCASCADE_INCLUDE_DIR})
        link_directories(${OpenCASCADE_LIBRARY_DIR})
    endif ()
else ()
    find_package(OCC REQUIRED)
    set(OCC_INCLUDE_DIR "/opt/opencascade/include/opencascade")
    if (OCC_INCLUDE_DIR)
        include_directories(SYSTEM ${OCC_INCLUDE_DIR})
        link_directories(${OCC_LIBRARY_DIR})
    endif ()
endif ()

if (NOT OCC_FOUND AND NOT OpenCASCADE_FOUND)
    MESSAGE(FATAL_ERROR "OPENCASCADE NOT FOUND.")
else ()
    message(STATUS "-------------------------------------------------")
    message(STATUS "OpenCASCADE LIBRARY FOUND")
    MESSAGE(STATUS "${OCC_TYPE} VERSION: ${OCC_VERSION_STRING}")
    MESSAGE(STATUS "${OCC_TYPE} include directory: ${OCC_INCLUDE_DIR}")
    MESSAGE(STATUS "${OCC_TYPE} shared libraries directory: ${OCC_LIBRARY_DIR}")
endif ()

# PETSC LIBRARY
if (PKG_CONFIG_FOUND)
    pkg_check_modules(PETSC PETSc)
endif ()

if (PETSC_FOUND)
    list(APPEND COMPILE_OPTIONS ${PETSC_CFLAGS})
    include_directories(${PETSC_INCLUDE_DIRS})
    set(LINK_FLAGS "${LINK_FLAGS} ${PETSC_LDFLAGS}")
    list(APPEND LIBRARIES ${PETSC_LINK_LIBRARIES})
    set(CMAKE_REQUIRED_FLAGS ${PETSC_CFLAGS})
    set(CMAKE_REQUIRED_INCLUDES "${PETSC_INCLUDE_DIRS}")
    message(STATUS "-------------------------------------------------")
    message(STATUS "PETSc LIBRARY FOUND")
    message(STATUS "PETSc version: ${PETSC_VERSION}")
    message(STATUS "PETSc include directory: ${PETSC_INCLUDE_DIRS}")
    message(STATUS "PETSc libraries: ${PETSC_LINK_LIBRARIES}")
    message(STATUS "-------------------------------------------------")
else ()
    message(FATAL_ERROR "PETSc NOT FOUND")
endif ()

# MPI
find_package(MPI REQUIRED)
if (${MPI_FOUND})
    message(STATUS "MPI FOUND")
    message(STATUS "MPI include path: ${MPI_INCLUDE_PATH}")
    message(STATUS "MPI C compiler: ${MPI_C_COMPILER}")
    message(STATUS "MPI libraries: ${MPI_C_LIBRARIES}")
    message(STATUS "-------------------------------------------------")
else ()
    message(FATAL_ERROR "MPI NOT FOUND")
endif ()

# NETGEN
find_package(Netgen REQUIRED)
if (NETGEN_VERSION)
    set(NETGEN_LIBRARIES nglib ngcore)
    MESSAGE(STATUS "NETGEN FOUND")
    MESSAGE(STATUS "NETGEN VERSION: ${NETGEN_VERSION_MAJOR}.${NETGEN_VERSION_MINOR}.${NETGEN_VERSION_PATCH}")
    MESSAGE(STATUS "NETGEN include directory: ${NETGEN_INCLUDE_DIRS}")
    MESSAGE(STATUS "NETGEN lib directory: ${NETGEN_LIBRARY_DIR}")
    MESSAGE(STATUS "-------------------------------------------------------------------------------")
else ()
    message(FATAL_ERROR "NETGEN not found!")
endif ()

add_subdirectory(src)

include_directories(
        ${PROJECT_DIR}/ThirdParty
        ${PROJECT_DIR}/src/Geometry
        ${PROJECT_DIR}/src/Mesh
        ${PROJECT_DIR}/src/Model
)


add_executable(NetgenPETScPlugin NetgenPETScPlugin.cpp)
#target_compile_options(NetgenPETScPlugin PRIVATE ${PETSC_CFLAGS_OTHER})
target_link_libraries(NetgenPETScPlugin PRIVATE
        GeometryCore
        MeshCore
        Model
)