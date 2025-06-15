add_library(MeshCore
        NetgenPluginMesher.cpp
        NetgenPluginLibWrapper.cpp
        MeshComputeError.hpp
        MeshParametersCompute.cpp
        MeshGenerator.cpp
        MeshAlgorithm.cpp
        MeshObject.cpp
        MeshWriter.cpp
        MeshInfo.cpp
)

if (WIN32)
    target_link_libraries(MeshCore PUBLIC
            ${OpenCASCADE_LIBRARIES}
            ${NETGEN_LIBRARIES}
            ${VTK_LIBRARIES}
    )
else ()
    target_link_libraries(MeshCore PUBLIC
            ${OCC_LIBRARIES}
            ${NETGEN_LIBRARIES}
            ${VTK_LIBRARIES}
    )

endif ()

include_directories(
        ${NETGEN_INCLUDE_DIRS}
        ${PROJECT_DIR}/src/Globals
)

target_compile_definitions(
        MeshCore PUBLIC
        NETGENPLUGIN_EXPORTS NETGEN_V6
)

target_include_directories(MeshCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
