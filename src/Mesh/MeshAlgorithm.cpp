#include "MeshAlgorithm.hpp"
#include "MeshComputeError.hpp"

//----------------------------------------------------------------------------
MeshAlgorithm::MeshAlgorithm()
	: _error(COMPERR_OK)
	  , _dim(ALG_3D) {
	fineness = Moderate;
	secondOrder = false;
	quadAllowed = false;
	maxSize = 1000;
	minSize = 0;
	growthRate = 0.3;
	nbSegPerRadius = 2;
	nbSegPerEdge = 1;
	optimize = true;
	nbSurfOptSteps = 5;
	nbVolOptSteps = 5;
	elemSizeWeight = 0.2;
	worstElemMeasure = 2;
	surfaceCurvature = true;
	useDelauney = true;
	checkOverlapping = true;
	checkChartBoundary = true;
}

//----------------------------------------------------------------------------
bool MeshAlgorithm::Is3DAlgorithm() const { return _dim == ALG_3D; }

//----------------------------------------------------------------------------
bool MeshAlgorithm::Is2DAlgorithm() const { return _dim == ALG_2D; }

//----------------------------------------------------------------------------
bool MeshAlgorithm::Is1DAlgorithm() const { return _dim == ALG_1D; }

//----------------------------------------------------------------------------
void MeshAlgorithm::SetDim(const AlgorithmDim algDim) { _dim = algDim; }
