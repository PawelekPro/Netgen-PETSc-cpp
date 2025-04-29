#ifndef MESHCOMPUTEERROR_HPP
#define MESHCOMPUTEERROR_HPP

enum MeshComputeError {
    COMPERR_OK = 0,
    COMPERR_BAD_INPUT_MESH = -1, //!< wrong mesh on lower submesh
    COMPERR_STD_EXCEPTION = -2, //!< some std exception raised
    COMPERR_OCC_EXCEPTION = -3, //!< OCC exception raised
    COMPERR_MGT_EXCEPTION = -4, //!< MGT exception raised
    COMPERR_EXCEPTION = -5, //!< other exception raised
    COMPERR_MEMORY_PB = -6, //!< std::bad_alloc exception
    COMPERR_ALGO_FAILED = -7, //!< algo failed for some reason
    COMPERR_BAD_SHAPE = -8, //!< bad geometry
    COMPERR_WARNING = -9, //!< algo reports error but sub-mesh is computed anyway
    COMPERR_CANCELED = -10, //!< compute canceled
    COMPERR_NO_MESH_ON_SHAPE = -11, //!< no mesh elements assigned to sub-shape
    COMPERR_BAD_PARAMETERS = -12, //!< incorrect hypotheses parameters
    COMPERR_LAST_ALGO_ERROR = -100, //!< terminator of mesh computation errors
};

#endif
