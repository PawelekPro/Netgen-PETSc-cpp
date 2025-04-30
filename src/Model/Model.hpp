#ifndef MODEL_HPP
#define MODEL_HPP

#include <memory>

class MeshAlgorithm;
class MeshObject;
class GeometryObject;

class Model {
public:
    Model();

    ~Model() = default;

    void ImportSTEP(const std::string &filePath) const;

    void SetMeshAlgorithm(const std::shared_ptr<MeshAlgorithm> &algorithm);

    void GenerateMesh();

    void SaveMeshToFile(const std::string &filePath) const;

    std::shared_ptr<MeshObject> GetMeshObject() const { return _mesh; };

private:
    std::shared_ptr<GeometryObject> _geometry;

    std::shared_ptr<MeshAlgorithm> _meshAlgorithm;
    std::shared_ptr<MeshObject> _mesh;
};


#endif //MODEL_HPP
