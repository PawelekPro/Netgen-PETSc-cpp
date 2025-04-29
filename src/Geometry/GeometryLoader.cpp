#include "GeometryLoader.hpp"

#include <XCAFApp_Application.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <XCAFDoc_DocumentTool.hxx>

#include <filesystem>

//----------------------------------------------------------------------------
std::string GeometryLoader::UniqueObjectName(
    const std::string &prefix, const PartsMap &objectMap) {
    int i = 1;
    std::string uniqueName;
    while (true) {
        std::stringstream stringStream;
        stringStream << prefix << std::setfill('0') << std::setw(3) << i;
        uniqueName = stringStream.str();

        if (!objectMap.contains(uniqueName)) {
            break;
        }
        i++;
    }
    return uniqueName;
}

//----------------------------------------------------------------------------
void GeometryLoader::ImportGeometryFromSTEP(const std::string &filename) {
    if (!std::filesystem::exists(filename)) {
        const auto message = "File " + filename + " can not be found.";
        const auto errorCode = std::make_error_code(std::errc::no_such_file_or_directory);
        throw std::filesystem::filesystem_error(message, errorCode);
    }

    auto baseName = std::filesystem::path{filename}.stem().generic_string();
    this->_dataFrame = Handle(TDocStd_Document){};

    auto app = XCAFApp_Application::GetApplication();
    app->NewDocument("MDTV-XCAF", this->_dataFrame);
    STEPCAFControl_Reader cafReader{};

    // Reading colors mode
    cafReader.SetColorMode(true);
    // Reading layers information mode
    cafReader.SetLayerMode(true);
    // Reading names from step file mode
    cafReader.SetNameMode(true);

    IFSelect_ReturnStatus result = cafReader.ReadFile(filename.c_str());
    if (result != IFSelect_RetDone) {
        auto message = "Error while reading file:" + filename;
        auto errorCode = std::make_error_code(std::errc::device_or_resource_busy);
        throw std::filesystem::filesystem_error(message, errorCode);
    }

    if (!cafReader.Transfer(this->_dataFrame)) {
        auto message = "Error while reading file:" + filename;
        auto errorCode = std::make_error_code(std::errc::device_or_resource_busy);
        throw std::filesystem::filesystem_error(message, errorCode);
    }

    auto &reader = cafReader.Reader();
    this->_shapesMap = PartsMap{};
    auto shapeTool = XCAFDoc_DocumentTool::ShapeTool(
        this->_dataFrame->Main());

    Standard_Integer numberOfShapes = reader.NbShapes();
    if (numberOfShapes == 0) {
        auto message = "No shapes found in given STEP file.";
        auto errorCode = std::make_error_code(std::errc::device_or_resource_busy);
        throw std::filesystem::filesystem_error(message, errorCode);
    }

    for (auto i = 1; i <= numberOfShapes; i++) {
        auto shape = reader.Shape(i);
        std::string uniqueName = UniqueObjectName("Shape", _shapesMap);
        this->_shapesMap[uniqueName] = shape;
    }
    std::filesystem::path filePath(filename);
    std::filesystem::path stepName = filePath.filename();
    auto message = "STEP file: " + stepName.string() + " loaded successfully.";
}


