#ifndef GEOMETRYLOADER_HPP
#define GEOMETRYLOADER_HPP

#include <TopoDS_Shape.hxx>
#include <TDocStd_Document.hxx>

#include <map>

using PartsMap = std::map<std::string, TopoDS_Shape>;

class GeometryLoader final {
public:
    GeometryLoader() = default;

    ~GeometryLoader() = default;

    PartsMap GetPartsMap() { return this->_shapesMap; };

    void ImportGeometryFromSTEP(const std::string &filename);

private:
    static std::string UniqueObjectName(const std::string &prefix, const PartsMap &objectMap);

    Handle(TDocStd_Document) _dataFrame;
    PartsMap _shapesMap;
};

#endif
