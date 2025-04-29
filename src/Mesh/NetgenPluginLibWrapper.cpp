#include "NetgenPluginLibWrapper.hpp"

#include <TCollection_AsciiString.hxx>

#include <filesystem>

#ifndef OCCGEOMETRY
#define OCCGEOMETRY
#endif
#include <occgeom.hpp>

namespace {
	void NOOP_Deleter(void *) { ; }
}

namespace netgen {
	NETGENPLUGIN_DLL_HEADER
	extern MeshingParameters mparam;
}

//----------------------------------------------------------------------------
NetgenPluginLibWrapper::NetgenPluginLibWrapper()
	: _ngMesh(nullptr) {
	if (InstanceCounter() == 0) {
		nglib::Ng_Init();

		netgen::testout = new std::ofstream("test.out");
		ngcore::Logger::SetGlobalLoggingLevel(ngcore::level::trace);
		ngcore::printmessage_importance = 2;
		std::cout << "PRINT MSG IMP: " << ngcore::printmessage_importance << std::endl;

		// if (!netgen::testout)
		// 	netgen::testout = new std::ofstream("test.out");
	}
	++InstanceCounter();

	_isComputeOk = false;
	_coutBuffer = nullptr;
	_ngcout = nullptr;
	_ngcerr = nullptr;

	// redirect all netgen output (mycout,myerr,cout) to _outputFileName
	_outputFileName = NetgenPluginLibWrapper::GetOutputFileName();
	_ngcout = netgen::mycout;
	_ngcerr = netgen::myerr;
	// netgen::mycout = new std::ofstream(_outputFileName.c_str());
	// std::ofstream* outFile = dynamic_cast<std::ofstream*>(netgen::mycout);

	// if (outFile && !outFile->is_open()) {
	// 	std::cerr << "Failed to open the output file: " << _outputFileName << std::endl;
	// 	return;
	// }

	netgen::myerr = netgen::mycout;
	_coutBuffer = std::cout.rdbuf();

#ifdef _DEBUG_
	std::cout << "NOTE: netgen output is redirected to file " << _outputFileName << std::endl;
#else
	std::cout.rdbuf(netgen::mycout->rdbuf());
#endif
	this->SetMesh(nglib::Ng_NewMesh());
}

//----------------------------------------------------------------------------
NetgenPluginLibWrapper::~NetgenPluginLibWrapper() {
	--InstanceCounter();

	// FIXME: This causes segmentation fault error
	// nglib::Ng_DeleteMesh(this->ngMesh());
	nglib::Ng_Exit();
	if (_coutBuffer)
		std::cout.rdbuf(_coutBuffer);

	this->RemoveOutputFile();
}

//----------------------------------------------------------------------------
int &NetgenPluginLibWrapper::InstanceCounter() {
	static int theCounter = 0;
	return theCounter;
}

//----------------------------------------------------------------------------
std::string NetgenPluginLibWrapper::GetOutputFileName() {
	std::string tmpDir = std::filesystem::temp_directory_path().string();

	if (!tmpDir.empty() && tmpDir.back() != '/' && tmpDir.back() != '\\') {
		tmpDir += '/';
	}

	TCollection_AsciiString aGenericName = tmpDir.c_str();
	aGenericName += "NETGEN_";
#ifndef WIN32
	aGenericName += getpid();
#else
	aGenericName += _getpid();
#endif
	aGenericName += "_";
	aGenericName += Abs(static_cast<Standard_Integer>(reinterpret_cast<long>(aGenericName.ToCString())));
	aGenericName += ".out";

	return aGenericName.ToCString();
}

//----------------------------------------------------------------------------
void NetgenPluginLibWrapper::RemoveOutputFile() {
	if (!_outputFileName.empty()) {
		if (_ngcout) {
			// delete netgen::mycout;
			netgen::mycout = _ngcout;
			netgen::myerr = _ngcerr;
			_ngcout = nullptr;
		}
		// ToDo: Remove temporary files
	}
}

//----------------------------------------------------------------------------
void NetgenPluginLibWrapper::SetMesh(nglib::Ng_Mesh *mesh) {
	if (_ngMesh)
		nglib::Ng_DeleteMesh(this->ngMesh());
	_ngMesh = reinterpret_cast<netgen::Mesh *>(mesh);
}

//----------------------------------------------------------------------------
int NetgenPluginLibWrapper::GenerateMesh(
	netgen::OCCGeometry &occGeom, const int startWith, const int endWith, netgen::Mesh *&ngMesh) {
	int err = 0;
	if (!ngMesh)
		ngMesh = new netgen::Mesh;

	ngMesh->SetGeometry(std::shared_ptr<netgen::NetgenGeometry>(&occGeom, &NOOP_Deleter));

	netgen::mparam.perfstepsstart = startWith;
	netgen::mparam.perfstepsend = endWith;
	std::shared_ptr<netgen::Mesh> meshPtr(ngMesh, &NOOP_Deleter);
	err = occGeom.GenerateMesh(meshPtr, netgen::mparam);

	return err;
}

//----------------------------------------------------------------------------
void NetgenPluginLibWrapper::CalcLocalH(netgen::Mesh *ngMesh) {
	ngMesh->CalcLocalH(netgen::mparam.grading);
}
