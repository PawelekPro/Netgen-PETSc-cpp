#ifndef GEOMETRYOBJECT_HPP
#define GEOMETRYOBJECT_HPP

#include "GeometryLoader.hpp"

#include <string>

class GeometryObject {
public:
    GeometryObject() = default;

    ~GeometryObject() = default;

    const PartsMap &GetShapesMap() const { return this->_shapesMap; };

    void ImportSTEP(const std::string &filePath);

private:
    PartsMap _shapesMap;
};


#endif //GEOMETRYOBJECT_HPP
