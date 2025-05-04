#ifndef FVMMESH_HPP
#define FVMMESH_HPP

#include "MeshObject.hpp"

#include <vector>

namespace FvmMesh {
    enum class ElementType {
        UNKNOWN,
        BEAM,
        TRIANGLE,
        QUADRANGLE,
        TETRAHEDRON,
        HEXAHEDRON,
        PRISM
    };

    struct Vector3 {
        double x = 0.0, y = 0.0, z = 0.0;
    };

    struct Face {
        int index = -1;

        ElementType type{};
        int nodesNb = 0;
        std::vector<int> node;

        int element = -1;

        Vector3 cFace;
        int pair = -1;

        Vector3 n;

        Vector3 A;
        double Aj = 0.0;

        Vector3 d;
        double dj = 0.0;
        double kj = 0.0;

        Vector3 rpl;
        Vector3 rnl;

        int physReg = -1;
        int elemReg = -1;
        int partition = -1;

        int ghost = 0;
        int bc = 0;
    };

    struct Element {
        int index = -1;

        ElementType type{};

        Vector3 normal;
        Vector3 cElement;

        int nodesNb = 0;
        std::vector<int> node;

        int facesNb = 0;
        std::vector<int> face;

        double dp = 0.0;
        double Lp = 0.0;
        double Ap = 0.0;
        double Vp = 0.0;

        int phyReg = -1;
        int elemReg = -1;
        int partition = -1;

        int process = 0;
        int bc = 0;

        std::vector<double> b;
        std::vector<double> c;
        std::vector<double> d;
    };

    struct Plane {
        Vector3 normal;
        double D = 0.0;
    };
}

class FvmMeshContainer {
public:
    explicit FvmMeshContainer(const std::shared_ptr<MeshObject> &meshObject);

    ~FvmMeshContainer() = default;

private:
    void BuildFvmMesh(const std::shared_ptr<MeshObject> &meshObject);

public:
    int nodesNb = 0;
    int facesNb = 0;
    int elementsNb = 0;
    int patchesNb = 0;

    int outPatchesNb = 0;

    int trisNb = 0;
    int quadsNb = 0;
    int tetrasNb = 0;
    int hexasNb = 0;
    int prismNb = 0;

    std::vector<FvmMesh::Vector3> nodes;
    std::vector<FvmMesh::Face> faces;
    std::vector<FvmMesh::Face> patches;
    std::vector<FvmMesh::Element> elements;

    int ghostsNb = 0;
    std::vector<int> ghosts;

    bool nodCorrelationAllocated = false;
    bool eleCorrelationAllocated = false;

    std::vector<int> nodCorrelation;
    std::vector<int> eleCorrelation;

    void FreeMemory();
};

#endif
