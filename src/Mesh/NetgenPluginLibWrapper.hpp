#ifndef NETGENPLUGINLIBWRAPPER_HPP
#define NETGENPLUGINLIBWRAPPER_HPP

#include "NetgenPluginDefs.hpp"

#include <fstream>
#include <memory>

namespace nglib {
#include <nglib.h>
}

namespace netgen {
    class OCCGeometry;
    class Mesh;
}


struct NETGENPLUGIN_EXPORT NetgenPluginLibWrapper {
    NetgenPluginLibWrapper();

    ~NetgenPluginLibWrapper();

    void SetMesh(nglib::Ng_Mesh *mesh);

    [[nodiscard]] nglib::Ng_Mesh *ngMesh() const { return static_cast<nglib::Ng_Mesh *>(static_cast<void *>(_ngMesh)); }

    static int GenerateMesh(
        netgen::OCCGeometry &occGeom, int startWith,
        int endWith, netgen::Mesh *&ngMesh);

    int GenerateMesh(netgen::OCCGeometry &occGeom, const int startWith, const int endWith) {
        return GenerateMesh(occGeom, startWith, endWith, _ngMesh);
    }

    static int &InstanceCounter();

    static void CalcLocalH(netgen::Mesh *ngMesh);

    bool _isComputeOk;
    netgen::Mesh *_ngMesh;

private:
    static std::string GetOutputFileName();

    void RemoveOutputFile();

private:
    std::ostream *_ngcout;
    std::ostream *_ngcerr;
    std::streambuf *_coutBuffer;
    std::string _outputFileName;
};


#endif
