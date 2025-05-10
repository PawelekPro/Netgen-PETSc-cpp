#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <array>
#include <string>
#include <vector>

namespace Material {
    struct General {
        float psi = 0.0f;
        float density = 0.0f;
        float viscosity = 0.0f;
        float surfaceTension = 0.0f;
    };

    struct Thermal {
        float specificHeat = 0.0f;
        float thermalConductivity = 0.0f;
        float boundaryThermalConductivity = 0.0f;
    };

    struct Mechanical {
        std::string elastmod;
        float poissonRatio = 0.0f;
    };
}


struct FvmMaterial {
    std::string label;
    int Id = -1;

    Material::General general;
    Material::Thermal thermal;
    Material::Mechanical mechanical;
};

class MaterialsBase {
public:
    void ReadFile(const std::string &filename);

    FvmMaterial GetMaterial(const std::string &label) const;

    FvmMaterial GetMaterial(int id);

private:
    std::vector<FvmMaterial> _libMaterials;
};

#endif
