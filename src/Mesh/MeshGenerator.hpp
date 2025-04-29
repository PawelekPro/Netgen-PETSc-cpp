#ifndef MESHGENERATOR_HPP
#define MESHGENERATOR_HPP

#include <TopoDS_Shape.hxx>

#include <memory>

class MeshAlgorithm;
class MeshObject;

class MeshGenerator {
public:
    MeshGenerator(
        const TopoDS_Shape &, const std::shared_ptr<MeshAlgorithm> &,
        const std::shared_ptr<MeshObject> &meshObject);

    ~MeshGenerator() = default;

    [[nodiscard]] int Compute();

    [[nodiscard]] std::shared_ptr<MeshObject> GetOutputMesh() const;

    static void CancelMeshGeneration();

private:
    std::shared_ptr<MeshObject> _meshObject;
    const TopoDS_Shape *_shape;
    const std::shared_ptr<MeshAlgorithm> _algorithm;
};


#endif
