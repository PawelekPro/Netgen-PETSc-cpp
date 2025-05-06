#include "FvmMesh.hpp"
#include "GeoCalc.hpp"

#include "petscksp.h"

using namespace FvmMesh;
using namespace netgen;

ElementType ConvertNetgenElementType(const int ngElemType) {
    switch (ngElemType) {
        case SEGMENT: return ElementType::BEAM;
        case SEGMENT3: return ElementType::BEAM;
        case TRIG: return ElementType::TRIANGLE;
        case QUAD: return ElementType::QUADRANGLE;
        case TRIG6: return ElementType::TRIANGLE;
        case QUAD6: return ElementType::QUADRANGLE;
        case QUAD8: return ElementType::QUADRANGLE;
        case TET: return ElementType::TETRAHEDRON;
        case TET10: return ElementType::TETRAHEDRON;
        case PYRAMID: return ElementType::TETRAHEDRON;
        case PRISM: return ElementType::PRISM;
        case PRISM12: return ElementType::PRISM;
        case PRISM15: return ElementType::PRISM;
        case PYRAMID13: return ElementType::TETRAHEDRON;
        case HEX: return ElementType::HEXAHEDRON;
        case HEX20: return ElementType::HEXAHEDRON;
        case HEX7: return ElementType::HEXAHEDRON;
        default: return ElementType::UNKNOWN;
    }
}

ElementType GetSurfaceElementType(const int nodesNb) {
    switch (nodesNb) {
        case 2: return ElementType::BEAM;
        case 3: return ElementType::TRIANGLE;
        case 4: return ElementType::QUADRANGLE;
        default: return ElementType::UNKNOWN;
    }
}


std::vector<int> GetFaceVertices(const netgen::Element2d &elem) {
    const auto &verts = elem.Vertices();

    switch (elem.GetType()) {
        case TRIG: return {verts[0], verts[1], verts[2]};
        case QUAD: return {verts[0], verts[1], verts[2], verts[3]};
        default:
            throw std::runtime_error("Unsupported element type for face extraction");
    }

    throw std::runtime_error("Invalid face index for element type");
}

std::vector<int> GetFaceVertices(
    const netgen::Element &elem, const int faceIndex) {
    const auto &verts = elem.Vertices();

    switch (elem.GetType()) {
        case TET:
        case TET10:
            switch (faceIndex) {
                case 0: return {verts[0], verts[1], verts[2]};
                case 1: return {verts[0], verts[1], verts[3]};
                case 2: return {verts[1], verts[2], verts[3]};
                case 3: return {verts[2], verts[0], verts[3]};
                default: break;
            }
            break;

        case HEX:
        case HEX20:
        case HEX7:
            switch (faceIndex) {
                case 0: return {verts[0], verts[1], verts[2], verts[3]};
                case 1: return {verts[4], verts[5], verts[6], verts[7]};
                case 2: return {verts[0], verts[1], verts[5], verts[4]};
                case 3: return {verts[1], verts[2], verts[6], verts[5]};
                case 4: return {verts[2], verts[3], verts[7], verts[6]};
                case 5: return {verts[3], verts[0], verts[4], verts[7]};
                default: break;
            }
            break;

        case PYRAMID:
        case PYRAMID13:
            switch (faceIndex) {
                case 0: return {verts[0], verts[1], verts[2], verts[3]}; // base
                case 1: return {verts[0], verts[1], verts[4]}; // side
                case 2: return {verts[1], verts[2], verts[4]};
                case 3: return {verts[2], verts[3], verts[4]};
                case 4: return {verts[3], verts[0], verts[4]};
                default: break;
            }
            break;

        case PRISM:
        case PRISM12:
        case PRISM15:
            switch (faceIndex) {
                case 0: return {verts[0], verts[1], verts[2]}; // base
                case 1: return {verts[3], verts[4], verts[5]}; // top
                case 2: return {verts[0], verts[1], verts[4], verts[3]};
                case 3: return {verts[1], verts[2], verts[5], verts[4]};
                case 4: return {verts[2], verts[0], verts[3], verts[5]};
                default: break;
            }
            break;

        default:
            throw std::runtime_error("Unsupported element type for face extraction");
    }

    throw std::runtime_error("Invalid face index for element type");
}

FvmMeshContainer::FvmMeshContainer(const std::shared_ptr<MeshObject> &meshObject) {
    this->BuildFvmMesh(meshObject);
    this->ComputeFaces();
    this->ComputeVolumes();
}

void FvmMeshContainer::BuildFvmMesh(const std::shared_ptr<MeshObject> &meshObject) {
    PetscPrintf(PETSC_COMM_WORLD, "\nCreating FVM mesh...\n");

    // NODES
    nodesNb = static_cast<int>(meshObject->GetNP());
    nodes.reserve(nodesNb);

    for (int i = 1; i <= nodesNb; ++i) {
        const auto &p = meshObject->Point(i);
        nodes.push_back({p(0), p(1), p(2)});
    }

    // ELEMENTS
    const bool volumeMesh = meshObject->GetNE() != 0;

    if (volumeMesh) {
        // Volume mesh
        elementsNb = meshObject->GetNE();
        elements.resize(elementsNb);

        for (int i = 1; i <= elementsNb; ++i) {
            const auto &elem = meshObject->VolumeElement(i);
            FvmMesh::Element &fvmElement = elements[i - 1];

            fvmElement.index = i;
            fvmElement.type = ConvertNetgenElementType(elem.GetType());
            fvmElement.nodesNb = elem.GetNP();
            fvmElement.nodes.assign(elem.Vertices().begin(), elem.Vertices().end());
            fvmElement.facesNb = elem.GetNFaces();
            fvmElement.faces.resize(fvmElement.facesNb, -1); // will fill later
        }
    } else {
        // Surface mesh
        elementsNb = meshObject->GetNSE();
        elements.resize(elementsNb);

        for (int i = 1; i <= elementsNb; ++i) {
            const auto &elem = meshObject->SurfaceElement(i);
            FvmMesh::Element &fvmElement = elements[i - 1];

            fvmElement.index = i;
            fvmElement.type = ConvertNetgenElementType(elem.GetType());
            fvmElement.nodesNb = elem.GetNP();
            fvmElement.nodes.assign(elem.Vertices().begin(), elem.Vertices().end());
            fvmElement.facesNb = 1; // surface elements have only one face
            fvmElement.faces.resize(fvmElement.facesNb, -1); // will fill later
        }
    }


    // FACES
    facesNb = meshObject->GetNSE();
    faces.clear();
    faces.reserve(facesNb);

    int faceIndex = 0;
    std::map<std::set<int>, int> faceMap; // unique set of nodes -> face index
    if (volumeMesh) {
        for (int e = 0; e < elementsNb; ++e) {
            const auto &elem = meshObject->VolumeElement(e + 1);
            for (int f = 0; f < elem.GetNFaces(); ++f) {
                auto faceVerts = GetFaceVertices(elem, f);
                std::set<int> faceKey(faceVerts.begin(), faceVerts.end());

                auto it = faceMap.find(faceKey);
                if (it == faceMap.end()) {
                    FvmMesh::Face face;
                    face.index = faceIndex;
                    face.nodes.assign(faceVerts.begin(), faceVerts.end());
                    face.nodesNb = static_cast<int>(face.nodes.size());
                    face.owner = e;
                    face.type = GetSurfaceElementType(face.nodesNb);

                    faceMap[faceKey] = faceIndex;

                    faces.push_back(face);
                    elements[e].faces[f] = faceIndex;
                    ++faceIndex;
                } else {
                    // Existing face (shared with another element)
                    const int sharedFaceIndex = it->second;
                    faces[sharedFaceIndex].pair = e;
                    elements[e].faces[f] = sharedFaceIndex;
                }
            }
        }
    } else {
        for (int e = 0; e < facesNb; ++e) {
            const auto &elem = meshObject->SurfaceElement(e + 1);
            auto faceVerts = GetFaceVertices(elem);

            FvmMesh::Face face;
            face.index = faceIndex;
            face.nodes.assign(faceVerts.begin(), faceVerts.end());
            face.nodesNb = static_cast<int>(face.nodes.size());
            face.owner = e;
            face.type = GetSurfaceElementType(face.nodesNb);

            faces.push_back(face);
            elements[e].faces[0] = faceIndex; // only one face
            ++faceIndex;
        }
    }


    facesNb = static_cast<int>(faces.size());

    // PATCHES
    patches.clear();
    for (const auto &face: faces) {
        if (face.pair == -1) {
            patches.push_back(face);
        }
    }
    patchesNb = static_cast<int>(patches.size());

    PetscPrintf(
        PETSC_COMM_WORLD,
        "FVM mesh created: \nNumber of nodes: %d\nNumber of elements: %d"
        "\nNumber of faces: %d\nNumber of patches: %d\n",
        nodesNb, elementsNb, facesNb, patchesNb);
}

void FvmMeshContainer::ComputeFaces() {
    for (auto &face: faces) {
        if (face.type == ElementType::TRIANGLE) {
            const Vector3 &n1 = nodes[face.nodes[0] - 1];
            const Vector3 &n2 = nodes[face.nodes[1] - 1];
            const Vector3 &n3 = nodes[face.nodes[2] - 1];

            face.Aj = GeoCalcTriArea(n1, n2, n3);
            face.cVec = GeoCalcCentroid3(n1, n2, n3);
            face.nVec = GeoCalcNormal(n1, n2, n3);
        } else if (face.type == ElementType::QUADRANGLE) {
            const Vector3 &n1 = nodes[face.nodes[0] - 1];
            const Vector3 &n2 = nodes[face.nodes[1] - 1];
            const Vector3 &n3 = nodes[face.nodes[2] - 1];
            const Vector3 &n4 = nodes[face.nodes[2] - 1];

            face.Aj = GeoCalcQuadArea(n1, n2, n3, n4);
            face.cVec = GeoCalcCentroid4(n1, n2, n3, n4);
            face.nVec = GeoCalcNormal(n1, n2, n3);
        }
        face.physReg = -1;
        face.elemReg = -1;
        face.partition = -1;
        face.bc = BndCondType::NONE;
    }

    double sum = 0.0;
    for (auto &patch: patches) {
        if (patch.type == ElementType::TRIANGLE) {
            const Vector3 &n1 = nodes[patch.nodes[0] - 1];
            const Vector3 &n2 = nodes[patch.nodes[1] - 1];
            const Vector3 &n3 = nodes[patch.nodes[2] - 1];

            patch.Aj = GeoCalcTriArea(n1, n2, n3);
            patch.cVec = GeoCalcCentroid3(n1, n2, n3);
            patch.nVec = GeoCalcNormal(n1, n2, n3);
            sum += patch.Aj;
        }
    }
    std::cout << "Sum of patch areas: " << sum << std::endl;
}

void FvmMeshContainer::ComputeVolumes() {
    for (auto &element: elements) {
        element.Vp = 0.0;
        element.cVec = {0.0, 0.0, 0.0};

        const auto &verts = element.nodes;

        if (element.type == ElementType::TRIANGLE) {
            const Vector3 &n1 = nodes[verts[0] - 1];
            const Vector3 &n2 = nodes[verts[1] - 1];
            const Vector3 &n3 = nodes[verts[2] - 1];

            element.Vp = 0.0;
            element.cVec = GeoCalcCentroid3(n1, n2, n3);
        } else if (element.type == ElementType::QUADRANGLE) {
            const Vector3 &n1 = nodes[verts[0] - 1];
            const Vector3 &n2 = nodes[verts[1] - 1];
            const Vector3 &n3 = nodes[verts[2] - 1];
            const Vector3 &n4 = nodes[verts[3] - 1];

            element.Vp = 0.0;
            element.cVec = GeoCalcCentroid4(n1, n2, n3, n4);
        } else if (element.type == ElementType::TETRAHEDRON) {
            const auto &n1 = nodes[verts[0] - 1];
            const auto &n2 = nodes[verts[1] - 1];
            const auto &n3 = nodes[verts[2] - 1];
            const auto &n4 = nodes[verts[3] - 1];

            element.Vp = GeoCalcTetraVolume(n1, n2, n3, n4);
            element.cVec = GeoCalcCentroid4(n1, n2, n3, n4);
        } else if (element.type == ElementType::HEXAHEDRON) {
            const auto &n1 = nodes[verts[0] - 1];
            const auto &n2 = nodes[verts[1] - 1];
            const auto &n3 = nodes[verts[2] - 1];
            const auto &n4 = nodes[verts[3] - 1];
            const auto &n5 = nodes[verts[4] - 1];
            const auto &n6 = nodes[verts[5] - 1];
            const auto &n7 = nodes[verts[6] - 1];
            const auto &n8 = nodes[verts[7] - 1];

            element.Vp = GeoCalcHexaVolume(n1, n2, n3, n4, n5, n6, n7, n8);
            element.cVec = GeoCalcCentroid8(n1, n2, n3, n4, n5, n6, n7, n8);
        } else if (element.type == ElementType::PRISM) {
            const auto &n1 = nodes[verts[0] - 1];
            const auto &n2 = nodes[verts[1] - 1];
            const auto &n3 = nodes[verts[2] - 1];
            const auto &n4 = nodes[verts[3] - 1];
            const auto &n5 = nodes[verts[4] - 1];
            const auto &n6 = nodes[verts[5] - 1];

            element.Vp = GeoCalcPrismVolume(n1, n2, n3, n4, n5, n6);
            element.cVec = GeoCalcCentroid6(n1, n2, n3, n4, n5, n6);
        }
    }

    double totalVolume = 0.0;
    for (const auto &el: elements)
        totalVolume += el.Vp;

    std::cout << "Total mesh volume: " << totalVolume << std::endl;
}

void FvmMeshContainer::FreeMemory() {
    nodes.clear();
    faces.clear();
    patches.clear();
    elements.clear();
    ghosts.clear();
    nodCorrelation.clear();
    eleCorrelation.clear();

    nodesNb = facesNb = elementsNb = patchesNb = ghostsNb = 0;
    outPatchesNb = 0;
    trisNb = quadsNb = tetrasNb = hexasNb = prismNb = 0;

    nodCorrelationAllocated = false;
    eleCorrelationAllocated = false;
}
