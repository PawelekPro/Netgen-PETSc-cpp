#include "GeometryObject.hpp"

//----------------------------------------------------------------------------
void GeometryObject::ImportSTEP(const std::string &filePath) {
    GeometryLoader loader;
    loader.ImportGeometryFromSTEP(filePath);
    this->_shapesMap = std::move(loader.GetPartsMap());
}
