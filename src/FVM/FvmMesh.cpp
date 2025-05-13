#include "FvmMesh.hpp"
#include "GeoCalc.hpp"
#include "FvmParam.hpp"
#include "Globals.hpp"

#include "petscksp.h"

using namespace FvmMesh;
using namespace netgen;

ElementType ConvertNetgenElementType(const int ngElemType) {
    switch (ngElemType) {
        case SEGMENT:
        case SEGMENT3: return ElementType::BEAM;

        case TRIG: return ElementType::TRIANGLE;
        case QUAD: return ElementType::QUADRANGLE;
        case TRIG6: return ElementType::TRIANGLE;

        case QUAD6:
        case QUAD8: return ElementType::QUADRANGLE;

        case TET:
        case TET10:
        case PYRAMID: return ElementType::TETRAHEDRON;

        case PRISM:
        case PRISM12:
        case PRISM15: return ElementType::PRISM;
        case PYRAMID13: return ElementType::TETRAHEDRON;

        case HEX:
        case HEX20:
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
    this->SetProcNumber(meshObject->GetProcNumber());

    this->BuildFvmMesh(meshObject);
    this->ComputeVolumes();
    this->ComputeFaces();
    this->ComputeMeshProperties();
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
        elementsNb = static_cast<int>(meshObject->GetNE());
        elements.resize(elementsNb);

        for (int i = 1; i <= elementsNb; ++i) {
            const auto &elem = meshObject->VolumeElement(i);
            FvmMesh::Element &fvmElement = elements[i - 1];

            fvmElement.index = i - 1;
            fvmElement.type = ConvertNetgenElementType(elem.GetType());
            fvmElement.nodesNb = elem.GetNP();
            fvmElement.nodes.assign(elem.Vertices().begin(), elem.Vertices().end());
            fvmElement.facesNb = elem.GetNFaces();
            fvmElement.faces.resize(fvmElement.facesNb, -1);
            if (IsParallel())
                fvmElement.procId = meshObject->vol_partition[i - 1];
        }
    } else {
        // Surface mesh
        elementsNb = static_cast<int>(meshObject->GetNSE());
        elements.resize(elementsNb);

        for (int i = 1; i <= elementsNb; ++i) {
            const auto &elem = meshObject->SurfaceElement(i);
            FvmMesh::Element &fvmElement = elements[i - 1];

            fvmElement.index = i - 1;
            fvmElement.type = ConvertNetgenElementType(elem.GetType());
            fvmElement.nodesNb = elem.GetNP();
            fvmElement.nodes.assign(elem.Vertices().begin(), elem.Vertices().end());
            fvmElement.facesNb = 1; // surface elements have only one face
            fvmElement.faces.resize(fvmElement.facesNb, -1);

            // Get physical geometry region index
            fvmElement.phyReg = elem.GetIndex();
            if (IsParallel())
                fvmElement.procId = meshObject->surf_partition[i];
        }
    }

    // PHYSICAL GEOMETRY REGIONS
    std::map<std::set<int>, std::pair<int, int> > boundaryFaceRegions;
    for (int i = 0; i < meshObject->GetNSE(); ++i) {
        const auto &se = meshObject->SurfaceElement(i + 1);
        std::set<int> seVerts(se.Vertices().begin(), se.Vertices().end());

        int regionIndex = se.GetIndex();
        int partitionId = 1; // default partition ID
        if (IsParallel())
            partitionId = meshObject->surf_partition[i];

        boundaryFaceRegions[seVerts] = std::make_pair(regionIndex, partitionId);
    }

    // FACES
    faces.clear();
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

                    // Get physical geometry region index
                    auto bfit = boundaryFaceRegions.find(faceKey);
                    if (bfit != boundaryFaceRegions.end()) {
                        face.physReg = bfit->second.first;
                        face.procId = bfit->second.second;
                    } else {
                        // volume physical region receives an ID equivalent to the number of surfaces + 1
                        face.physReg = meshObject->GetNFD() + elem.GetIndex();
                    }

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
        for (int e = 0; e < static_cast<int>(meshObject->GetNSE()); ++e) {
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

    int maxIndex = elements.front().index;
    for (auto &patch: patches) {
        if (patch.index > maxIndex) {
            patch.bc = BndCondType::PROCESSOR;
        }
    }

    patchesNb = static_cast<int>(patches.size());

    _physicalSurfaceRegions = meshObject->GetSurfaceRegions();
    _physicalVolumeRegions = meshObject->GetVolumeRegions();
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
            const Vector3 &n4 = nodes[face.nodes[3] - 1];

            face.Aj = GeoCalcQuadArea(n1, n2, n3, n4);
            face.cVec = GeoCalcCentroid4(n1, n2, n3, n4);
            face.nVec = GeoCalcNormal(n1, n2, n3);
        }

        face.aVec = GeoMultScalarVector(face.Aj, face.nVec);
        face.rpl = GeoSubVectorVector(
            face.cVec, GeoMultScalarVector(
                GeoDotVectorVector(
                    GeoSubVectorVector(face.cVec, elements[face.owner].cVec), face.nVec), face.nVec));

        if (face.pair != -1) {
            const int neighbour = face.pair;

            Vector3 diff = GeoSubVectorVector(elements[neighbour].cVec, elements[face.owner].cVec);
            // Normal should point to neighbour - flip normal if necessary
            if (GeoDotVectorVector(diff, face.nVec) < 0) {
                face.nVec = GeoMultScalarVector(-1.0, face.nVec);
            }

            face.rnl = GeoSubVectorVector(
                face.cVec, GeoMultScalarVector(
                    GeoDotVectorVector(
                        GeoSubVectorVector(face.cVec, elements[neighbour].cVec), face.nVec), face.nVec));

            face.dVec = GeoSubVectorVector(face.rnl, face.rpl);
            face.dj = GeoMagVector(face.dVec);
            if (face.dj == 0) {
                PetscPrintf(PETSC_COMM_WORLD, "\nError: Problem with mesh\n");
                throw FvmException("Invalid mesh (Direction vector length == 0)", LOGICAL_ERROR);
            }
        } else {
            face.dVec = GeoSubVectorVector(face.cVec, face.rpl);
            face.dj = GeoMagVector(face.dVec);
            if (face.dj == 0) {
                PetscPrintf(PETSC_COMM_WORLD, "\nError: Problem with mesh\n");
                throw FvmException("Invalid mesh (Direction vector length == 0)", LOGICAL_ERROR);
            }
        }

        // face.elemReg = -1;
        face.bc = BndCondType::NONE;
    }

    for (auto &patch: patches) {
        if (patch.type == ElementType::TRIANGLE) {
            const Vector3 &n1 = nodes[patch.nodes[0] - 1];
            const Vector3 &n2 = nodes[patch.nodes[1] - 1];
            const Vector3 &n3 = nodes[patch.nodes[2] - 1];

            patch.Aj = GeoCalcTriArea(n1, n2, n3);
            patch.cVec = GeoCalcCentroid3(n1, n2, n3);
            patch.nVec = GeoCalcNormal(n1, n2, n3);
        }
    }
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

    auto [minIt, maxIt] = std::minmax_element(elements.begin(), elements.end(),
                                              [](const FvmMesh::Element &a, const FvmMesh::Element &b) {
                                                  return a.Vp < b.Vp;
                                              });

    if (minIt != elements.end() && maxIt != elements.end()) {
        double minVp = minIt->Vp;
        double maxVp = maxIt->Vp;

        std::cout << "Min Vp: " << minVp << ", Max Vp: " << maxVp << std::endl;
    }
}

void FvmMeshContainer::ComputeMeshProperties() {
    tetrasNb = 0;
    hexasNb = 0;
    prismNb = 0;
    totalVolume = 0.0;
    for (const auto &el: elements) {
        totalVolume += el.Vp;

        switch (el.type) {
            case ElementType::TETRAHEDRON:
                ++tetrasNb;
                break;
            case ElementType::HEXAHEDRON:
                ++hexasNb;
                break;
            case ElementType::PRISM:
                ++prismNb;
            default: continue;
        }
    }

    trisNb = 0;
    quadsNb = 0;
    for (const auto &face: faces) {
        switch (face.type) {
            case ElementType::TRIANGLE:
                ++trisNb;
                break;
            case ElementType::QUADRANGLE:
                ++quadsNb;
                break;
            default: continue;
        }
    }

    totalArea = 0.0;
    for (const auto &patch: patches) {
        totalArea += patch.Aj;
    }

    PetscPrintf(PETSC_COMM_WORLD, "\nFVM MESH PROPERTIES:\n");
    PetscPrintf(PETSC_COMM_WORLD, "Total surface area: \t%.3E %s^2\n",
                totalArea, fvmParameter.ulength.c_str());
    PetscPrintf(PETSC_COMM_WORLD, "Total volume: \t\t\t%.3E %s^3\n",
                totalVolume, fvmParameter.ulength.c_str());
    PetscPrintf(PETSC_COMM_WORLD, "Mesh statistics:\n");
    PetscPrintf(PETSC_COMM_WORLD, "  Nodes: \t\t\t\t%d\n", nodesNb);
    PetscPrintf(PETSC_COMM_WORLD, "  Faces: \t\t\t\t%d\n", facesNb);
    PetscPrintf(PETSC_COMM_WORLD, "  Patches: \t\t\t\t%d\n", patchesNb);
    PetscPrintf(PETSC_COMM_WORLD, "  Elements: \t\t\t%d\n", elementsNb);

    PetscPrintf(PETSC_COMM_WORLD, "Element types:\n");
    PetscPrintf(PETSC_COMM_WORLD, "  Tetrahedrons: \t\t%d\n", tetrasNb);
    PetscPrintf(PETSC_COMM_WORLD, "  Hexahedrons: \t\t\t%d\n", hexasNb);
    PetscPrintf(PETSC_COMM_WORLD, "  Prisms: \t\t\t\t%d\n", prismNb);
    PetscPrintf(PETSC_COMM_WORLD, "  Triangles: \t\t\t%d\n", trisNb);
    PetscPrintf(PETSC_COMM_WORLD, "  Quadrangles: \t\t\t%d\n", quadsNb);
}

int FvmMeshContainer::GetSurfacesRegionsNumber() const {
    return static_cast<int>(_physicalSurfaceRegions.size());
}

int FvmMeshContainer::GetVolumesRegionsNumber() const {
    return static_cast<int>(_physicalVolumeRegions.size());
}

FvmMesh::Vector3 FvmMeshContainer::GetNode(const int index) const {
    return nodes[index];
}

std::string FvmMeshContainer::GetBoundaryLabel(const int index) const {
    return _physicalSurfaceRegions.at(index);
}
