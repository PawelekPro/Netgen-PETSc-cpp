#include "FvmMesh.hpp"

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
}

void FvmMeshContainer::BuildFvmMesh(const std::shared_ptr<MeshObject> &meshObject) {
    PetscPrintf(PETSC_COMM_WORLD, "\nCreating FVM mesh...\n");

    // // NODES
    // nodesNb = static_cast<int>(meshObject->GetNP());
    // nodes.reserve(nodesNb);
    //
    // for (int i = 1; i <= nodesNb; ++i) {
    //     const auto &p = meshObject->Point(i);
    //     Vector3 fvmNode;
    //     fvmNode.x = p(0);
    //     fvmNode.y = p(1);
    //     fvmNode.z = p(2);
    //     nodes.push_back(fvmNode);
    // }
    //
    // // FACES
    // facesNb = meshObject->GetNSE();
    // faces.reserve(facesNb);
    // for (const auto &ngFace: meshObject->SurfaceElements()) {
    //     Face fvmFace;
    //     fvmFace.index = ngFace.GetIndex();
    //     fvmFace.type = ConvertNetgenElementType(ngFace.GetType());
    //
    //     fvmFace.nodesNb = ngFace.GetNP();
    //     fvmFace.node.reserve(fvmFace.nodesNb);
    //     for (int vid: ngFace.Vertices()) {
    //         fvmFace.node.push_back(vid);
    //     }
    // }
    //
    // // ELEMENTS
    // elementsNb = static_cast<int>(meshObject->GetNE());
    // elements.reserve(elementsNb);
    // for (int i = 1; i <= elementsNb; ++i) {
    //     FvmMesh::Element fvmElement;
    //     const auto &elem = meshObject->VolumeElement(i);
    //     std::cout << i << std::endl;
    //     fvmElement.index = elem.GetIndex();
    //
    //     const ElementType elType = ConvertNetgenElementType(elem.GetType());
    //     if (elType == ElementType::UNKNOWN) {
    //         throw std::runtime_error("Unknown Element Type");
    //     }
    //     fvmElement.type = elType;
    //     fvmElement.nodesNb = elem.GetNP();
    //     fvmElement.node.reserve(fvmElement.nodesNb);
    //     for (int vid: elem.Vertices()) {
    //         fvmElement.node.push_back(vid);
    //     }
    //
    //     elements.push_back(fvmElement);
    //     // fvmElement.facesNb = elem.GetNFaces();
    //     // fvmElement.face.reserve(fvmElement.facesNb);
    //     //
    //     // for (int faceId = 0; faceId < fvmElement.facesNb; ++faceId) {
    //     //     fvmElement.face.push_back(elem.GetFace(faceId));
    //     // }
    // }

    // NODES
    nodesNb = static_cast<int>(meshObject->GetNP());
    nodes.reserve(nodesNb);

    for (int i = 1; i <= nodesNb; ++i) {
        const auto &p = meshObject->Point(i);
        nodes.push_back({p(0), p(1), p(2)});
    }

    // ELEMENTS
    elementsNb = meshObject->GetNE();
    elements.resize(elementsNb);

    for (int i = 1; i <= elementsNb; ++i) {
        const auto &elem = meshObject->VolumeElement(i);
        FvmMesh::Element &fvmElement = elements[i - 1];

        fvmElement.index = i;
        fvmElement.type = ConvertNetgenElementType(elem.GetType());
        fvmElement.nodesNb = elem.GetNP();
        fvmElement.node.assign(elem.Vertices().begin(), elem.Vertices().end());
        fvmElement.facesNb = elem.GetNFaces();
        fvmElement.face.resize(fvmElement.facesNb, -1); // will fill later
    }

    // FACES
    facesNb = meshObject->GetNSE();
    faces.clear();
    faces.reserve(facesNb);

    int faceIndex = 0;
    std::map<std::set<int>, int> faceMap; // unique set of nodes -> face index

    for (int e = 0; e < elementsNb; ++e) {
        const auto &elem = meshObject->VolumeElement(e + 1);
        for (int f = 0; f < elem.GetNFaces(); ++f) {
            auto faceVerts = GetFaceVertices(elem, f);
            std::set<int> faceKey(faceVerts.begin(), faceVerts.end());

            auto it = faceMap.find(faceKey);
            if (it == faceMap.end()) {
                // New face
                FvmMesh::Face face;
                face.index = faceIndex;
                face.node.assign(faceVerts.begin(), faceVerts.end());
                face.nodesNb = static_cast<int>(face.node.size());
                face.element = e;
                face.type = ConvertNetgenElementType(elem.GetType()); // approximate
                faceMap[faceKey] = faceIndex;

                faces.push_back(face);
                elements[e].face[f] = faceIndex;
                ++faceIndex;
            } else {
                // Existing face (shared with another element)
                int sharedFaceIndex = it->second;
                faces[sharedFaceIndex].pair = e;
                elements[e].face[f] = sharedFaceIndex;
            }
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
