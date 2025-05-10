#include "Material.hpp"

#include <algorithm>
#include <iostream>
#include <tinyxml2/tinyxml2.h>

#include "Globals.hpp"
#include "petsc.h"

void MaterialsBase::ReadFile(const std::string &filename) {
    PetscPrintf(
        PETSC_COMM_WORLD, "\nReading material file: %s ...\n", filename.c_str());
    using namespace tinyxml2;

    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to read file: " << filename << std::endl;
        return;
    }

    const XMLElement *root = doc.FirstChildElement("materials");
    if (!root) {
        std::cerr << "No <materials> root element found." << std::endl;
        return;
    }

    std::vector<FvmMaterial> tempMaterials;

    for (const XMLElement *matElem = root->FirstChildElement("material"); matElem;
         matElem = matElem->NextSiblingElement("material")) {
        FvmMaterial mat;

        mat.Id = matElem->IntAttribute("id", -1);
        mat.label = matElem->Attribute("name") ? matElem->Attribute("name") : "unknown";

        for (const XMLElement *prop = matElem->FirstChildElement("property"); prop;
             prop = prop->NextSiblingElement("property")) {
            std::string label = prop->Attribute("label") ? prop->Attribute("label") : "";
            std::string valueStr = prop->GetText() ? prop->GetText() : "";
            float value = std::stof(valueStr);

            if (label == "compressibility") mat.general.psi = value;
            else if (label == "density") mat.general.density = value;
            else if (label == "viscosity") mat.general.viscosity = value;
            else if (label == "specificHeat") mat.thermal.specificHeat = value;
            else if (label == "thermalConductivity") mat.thermal.thermalConductivity = value;
            else if (label == "surfaceTension") mat.general.surfaceTension = value;
            else if (label == "poissonRatio") mat.mechanical.poissonRatio = value;
        }

        tempMaterials.push_back(mat);
    }

    // Sort by materials id
    std::ranges::sort(tempMaterials,
                      [](const FvmMaterial &a, const FvmMaterial &b) {
                          return a.Id < b.Id;
                      });

    _libMaterials = std::move(tempMaterials);
}


FvmMaterial MaterialsBase::GetMaterial(const std::string &label) const {
    for (const auto &mat: _libMaterials) {
        if (mat.label == label) return mat;
    }
    throw FvmException(
        "Material with label" + label + " not found.\n",
        LOGICAL_ERROR);
}

FvmMaterial MaterialsBase::GetMaterial(const int id) {
    return _libMaterials[id];
}
