#include "NetgenPluginMesher.hpp"
#include "MeshComputeError.hpp"
#include "MeshParametersCompute.hpp"
#include "MeshAlgorithm.hpp"
#include "NetgenPluginLibWrapper.hpp"
#include "MeshObject.hpp"
#include "MeshInfo.hpp"

#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <Standard_Failure.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <TopoDS_Edge.hxx>

#ifndef OCCGEOMETRY
#define OCCGEOMETRY
#endif
#include <occgeom.hpp>

#include <map>
#include <set>

namespace netgen {
	NETGENPLUGIN_DLL_HEADER
	extern MeshingParameters mparam;
}

TopTools_IndexedMapOfShape ShapesWithLocalSize;
std::map<int, double> VertexId2LocalSize;
std::map<int, double> EdgeId2LocalSize;
std::map<int, double> FaceId2LocalSize;
std::map<int, double> SolidId2LocalSize;

std::set<int> ShapesWithControlPoints;

//----------------------------------------------------------------------------
void setLocalSize(const TopoDS_Shape &GeomShape, const double LocalSize) {
	if (GeomShape.IsNull())
		return;
	const TopAbs_ShapeEnum GeomType = GeomShape.ShapeType();
	if (GeomType == TopAbs_COMPOUND) {
		for (TopoDS_Iterator it(GeomShape); it.More(); it.Next()) {
			setLocalSize(it.Value(), LocalSize);
		}
		return;
	}
	int key;
	if (!ShapesWithLocalSize.Contains(GeomShape))
		key = ShapesWithLocalSize.Add(GeomShape);
	else
		key = ShapesWithLocalSize.FindIndex(GeomShape);

	if (GeomType == TopAbs_VERTEX) {
		VertexId2LocalSize[key] = LocalSize;
	} else if (GeomType == TopAbs_EDGE) {
		EdgeId2LocalSize[key] = LocalSize;
	} else if (GeomType == TopAbs_FACE) {
		FaceId2LocalSize[key] = LocalSize;
	} else if (GeomType == TopAbs_SOLID) {
		SolidId2LocalSize[key] = LocalSize;
	}
}

//----------------------------------------------------------------------------
void setLocalSize(
	const TopoDS_Edge &edge, const double size, netgen::Mesh &mesh,
	const bool overrideMinH = true) {
	if (size <= std::numeric_limits<double>::min())
		return;

	Standard_Real u1, u2;
	const Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, u1, u2);

	if (curve.IsNull()) {
		const TopoDS_Iterator vIt(edge);
		if (!vIt.More())
			return;
		const gp_Pnt p = BRep_Tool::Pnt(TopoDS::Vertex(vIt.Value()));
		NetgenPluginMesher::RestrictLocalSize(
			mesh, p.XYZ(), size, overrideMinH);
	} else {
		const int nb = static_cast<int>(1.5 * MeshParametersCompute::EdgeLength(edge) / size);
		const Standard_Real delta = (u2 - u1) / nb;

		for (int i = 0; i < nb; i++) {
			const Standard_Real u = u1 + delta * i;
			gp_Pnt p = curve->Value(u);
			NetgenPluginMesher::RestrictLocalSize(
				mesh, p.XYZ(), size, overrideMinH);
			netgen::Point3d pi(p.X(), p.Y(), p.Z());
			const double resultSize = mesh.GetH(pi);

			if (resultSize - size > 0.1 * size)
				// netgen does restriction iff oldH/newH > 1.2 (localh.cpp:136)
				NetgenPluginMesher::RestrictLocalSize(
					mesh, p.XYZ(), resultSize / 1.201, overrideMinH);
		}
	}
}

//----------------------------------------------------------------------------
NetgenPluginMesher::NetgenPluginMesher(
	const TopoDS_Shape &shape, const std::shared_ptr<MeshAlgorithm> &algorithm,
	const std::shared_ptr<MeshObject> &mesh)
	: _shape(shape)
	  , _algorithm(algorithm)
	  , _optimize(true)
	  , _fineness(algorithm->fineness)
	  , _isViscousLayers2D(false)
	  , _ngMesh(mesh)
	  , _occGeom(nullptr)
	  , _selfPtr(nullptr) {
	this->SetMeshParameters();
	ShapesWithLocalSize.Clear();
	VertexId2LocalSize.clear();
	EdgeId2LocalSize.clear();
	FaceId2LocalSize.clear();
	SolidId2LocalSize.clear();

	ShapesWithControlPoints.clear();
}

//----------------------------------------------------------------------------
NetgenPluginMesher::NetgenPluginMesher(const TopoDS_Shape &shape)
	: _shape(shape)
	  , _algorithm(nullptr)
	  , _optimize(true)
	  , _fineness(MeshAlgorithm::GetDefaultFineness())
	  , _isViscousLayers2D(false)
	  , _ngMesh(std::make_shared<MeshObject>())
	  , _occGeom(nullptr)
	  , _selfPtr(nullptr) {
}

//----------------------------------------------------------------------------
NetgenPluginMesher::~NetgenPluginMesher() {
	if (_selfPtr) {
		*_selfPtr = nullptr;
	}
	_selfPtr = nullptr;
	_ngMesh = nullptr;
}

//----------------------------------------------------------------------------
void NetgenPluginMesher::SetMeshParameters() {
	netgen::MeshingParameters &mParams = netgen::mparam;
	mParams = netgen::MeshingParameters();

	mParams.maxh = _algorithm->maxSize;
	mParams.minh = _algorithm->minSize;

	mParams.segmentsperedge = _algorithm->nbSegPerEdge;
	mParams.grading = _algorithm->growthRate;

	mParams.curvaturesafety = _algorithm->nbSegPerEdge;
	mParams.secondorder = _algorithm->secondOrder;

	mParams.quad = _algorithm->quadAllowed;

	_fineness = -_algorithm->fineness;
	mParams.uselocalh = _algorithm->surfaceCurvature;

	_optimize = _algorithm->optimize;

	mParams.optsteps2d = _algorithm->nbSurfOptSteps;
	mParams.optsteps3d = _algorithm->nbVolOptSteps;
}

//----------------------------------------------------------------------------
void NetgenPluginMesher::PrepareOCCGeometry(
	netgen::OCCGeometry &occGeom, const TopoDS_Shape &shape) {
	occGeom.shape = shape;
	occGeom.changed = 1;
	occGeom.BuildFMap();
	occGeom.BuildVisualizationMesh(0.01);
	occGeom.CalcBoundingBox();

	std::cout << "\nShapes found in loaded part:" << std::endl;
	occGeom.PrintNrShapes();
	std::cout << std::endl;
}


//----------------------------------------------------------------------------
int NetgenPluginMesher::ComputeMesh() {
	NetgenPluginLibWrapper ngLib;
	netgen::MeshingParameters &mParams = netgen::mparam;
	netgen::multithread.terminate = 0;

	netgen::OCCGeometry occGeom;
	NetgenPluginMesher::PrepareOCCGeometry(occGeom, _shape);
	_occGeom = &occGeom;

	int err = MeshComputeError::COMPERR_OK;

	if (mParams.maxh == 0.0)
		mParams.maxh = occGeom.boundingbox.Diam();

	if (mParams.minh == 0.0
	    && _fineness != MeshAlgorithm::UserDefined)
		mParams.minh = MeshParametersCompute::GetDefaultMinSize(
			_shape, mParams.maxh);

	std::cout << mParams << std::endl;
	occGeom.face_maxh = mParams.maxh;

	int startWith = netgen::MESHCONST_ANALYSE;
	int endWith = netgen::MESHCONST_ANALYSE;

	try {
		netgen::Mesh *rawMesh = _ngMesh.get();
		err = NetgenPluginLibWrapper::GenerateMesh(
			occGeom, startWith, endWith, rawMesh);
		if (rawMesh != _ngMesh.get())
			_ngMesh.reset(static_cast<MeshObject *>(rawMesh));

		if (netgen::multithread.terminate)
			return MeshComputeError::COMPERR_CANCELED;
	} catch (Standard_Failure &ex) {
		std::cerr << "OpenCASCADE Exception: " << ex << std::endl;
	} catch (netgen::NgException &ex) {
		std::cerr << "Netgen Exception: " << ex.What() << std::endl;
	}

	if (!_ngMesh)
		return err;
	ngLib.SetMesh(reinterpret_cast<nglib::Ng_Mesh *>(_ngMesh.get()));

	if (err)
		return err;

	// if (!mParams.uselocalh)
	// 	_ngMesh->LocalHFunction().SetGrading(mParams.grading);

	// const TopoDS_Shape& shape = occgeo.fmap.FindKey(1);
	// setLocalSize(shape, 2);

	SetLocalSize(occGeom, *_ngMesh);

	// Compute 1D mesh
	startWith = endWith = netgen::MESHCONST_MESHEDGES;
	try {
		err = ngLib.GenerateMesh(occGeom, startWith, endWith);

		if (netgen::multithread.terminate)
			return MeshComputeError::COMPERR_CANCELED;
	} catch (Standard_Failure &ex) {
		std::cerr << "OpenCASCADE Exception: " << ex << std::endl;
	} catch (netgen::NgException &ex) {
		std::cerr << "Netgen Exception: " << ex.What() << std::endl;
	}

	if (err)
		return err;

	// Compute surface mesh
	mParams.uselocalh = true;
	startWith = netgen::MESHCONST_MESHSURFACE;
	endWith = _optimize
		          ? netgen::MESHCONST_OPTSURFACE
		          : netgen::MESHCONST_MESHSURFACE;

	try {
		err = ngLib.GenerateMesh(occGeom, startWith, endWith);

		if (netgen::multithread.terminate)
			return MeshComputeError::COMPERR_CANCELED;
	} catch (Standard_Failure &ex) {
		std::cerr << "OpenCASCADE Exception: " << ex << std::endl;
	} catch (netgen::NgException &ex) {
		std::cerr << "Netgen Exception: " << ex.What() << std::endl;
	}

	if (err)
		return err;

	if (_algorithm->Is3DAlgorithm()) {
		startWith = netgen::MESHCONST_MESHVOLUME;
		endWith = _optimize
			          ? netgen::MESHCONST_OPTVOLUME
			          : netgen::MESHCONST_MESHVOLUME;

		try {
			err = ngLib.GenerateMesh(occGeom, startWith, endWith);

			if (netgen::multithread.terminate)
				return MeshComputeError::COMPERR_CANCELED;
		} catch (Standard_Failure &ex) {
			std::cerr << "OpenCASCADE Exception: " << ex << std::endl;
		} catch (netgen::NgException &ex) {
			std::cerr << "Netgen Exception: " << ex.What() << std::endl;
		}
	} else {
		return MeshComputeError::COMPERR_OK;
	}

	if (err)
		return err;

	// auto meshInfo = MeshInfo(_ngMesh);
	// meshInfo.PrintSelf();

	return MeshComputeError::COMPERR_OK;
}

//----------------------------------------------------------------------------
void NetgenPluginMesher::CancelMeshGeneration() {
	netgen::multithread.terminate = 1;
}

//----------------------------------------------------------------------------
void NetgenPluginMesher::RestrictLocalSize(
	netgen::Mesh &ngMesh, const gp_XYZ &p, double size, const bool overrideMinH) {
	if (size <= std::numeric_limits<double>::min())
		return;

	if (netgen::mparam.minh > size) {
		if (overrideMinH) {
			ngMesh.SetMinimalH(size);
			netgen::mparam.minh = size;
		} else {
			size = netgen::mparam.minh;
		}
	}
	const netgen::Point3d pi(p.X(), p.Y(), p.Z());
	ngMesh.RestrictLocalH(pi, size);
}

//----------------------------------------------------------------------------
void NetgenPluginMesher::SetLocalSize(
	netgen::OCCGeometry &occGeom, netgen::Mesh &ngMesh) {
	// edges
	std::map<int, double>::const_iterator it;
	for (it = EdgeId2LocalSize.begin(); it != EdgeId2LocalSize.end(); ++it) {
		const int key = it->first;
		const double hi = it->second;
		const TopoDS_Shape &shape = ShapesWithLocalSize.FindKey(key);
		setLocalSize(TopoDS::Edge(shape), hi, ngMesh);
	}

	// vertices
	for (it = VertexId2LocalSize.begin(); it != VertexId2LocalSize.end();
	     ++it) {
		const int key = it->first;
		const double hi = it->second;
		const TopoDS_Shape &shape = ShapesWithLocalSize.FindKey(key);
		gp_Pnt p = BRep_Tool::Pnt(TopoDS::Vertex(shape));
		NetgenPluginMesher::RestrictLocalSize(ngMesh, p.XYZ(), hi);
	}

	// faces
	for (it = FaceId2LocalSize.begin(); it != FaceId2LocalSize.end(); ++it) {
		int key = it->first;
		double val = it->second;
		const TopoDS_Shape &shape = ShapesWithLocalSize.FindKey(key);
		const int faceNgID = occGeom.fmap.FindIndex(shape);

		if (faceNgID >= 1) {
			occGeom.SetFaceMaxH(faceNgID, val, netgen::mparam);
			for (TopExp_Explorer edgeExp(shape, TopAbs_EDGE); edgeExp.More();
			     edgeExp.Next()) {
				setLocalSize(TopoDS::Edge(edgeExp.Current()), val, ngMesh);
			}
		} else if (!ShapesWithControlPoints.contains(key)) {
			std::cerr << "UNSUPPORTED FEATURE" << std::endl;
			// std::cout << "Creating control points for face " << key
			// 		<< std::endl;
			// MGTMeshUtils::createPointsSampleFromFace(
			// 	TopoDS::Face(shape), val, ControlPoints);
			// ShapesWithControlPoints.insert(key);
		}
	}
	// ToDo: support for solids

	// if (!ControlPoints.empty()) {
	// 	std::cout << "Restricting local size for control point " << std::endl;
	// 	for (size_t i = 0; i < ControlPoints.size(); ++i)
	// 		NetgenPluginMesher::RestrictLocalSize(
	// 			ngMesh, ControlPoints[i].XYZ(), ControlPoints[i].Size());
	// }
}

