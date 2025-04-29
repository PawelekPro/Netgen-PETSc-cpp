#ifndef MESHALGORITHM_HPP
#define MESHALGORITHM_HPP

#include <string>

class MeshAlgorithm {
public:
    explicit MeshAlgorithm();

    ~MeshAlgorithm() = default;

public:
    enum AlgorithmDim { PARAM_ALG, ALG_0D, ALG_1D, ALG_2D, ALG_3D };

    enum Fineness { VeryCoarse, Coarse, Moderate, Fine, VeryFine, UserDefined };

    Fineness fineness{Moderate};
    bool secondOrder{};
    bool quadAllowed{};

    // Mesh size
    double maxSize{}, minSize{};
    double growthRate{};
    std::string meshSizeFile;
    double nbSegPerRadius{};
    double nbSegPerEdge{};

    // Optimizer
    bool optimize{};
    int nbSurfOptSteps{};
    int nbVolOptSteps{};
    double elemSizeWeight{};
    int worstElemMeasure{};

    // Insider
    bool surfaceCurvature{};
    bool useDelauney{};
    bool checkOverlapping{};
    bool checkChartBoundary{};

public:
    static Fineness GetDefaultFineness() { return Moderate; }

    void SetDim(AlgorithmDim algDim);
    int GetDim() const { return _dim; }

    [[nodiscard]] bool Is3DAlgorithm() const;

    [[nodiscard]] bool Is2DAlgorithm() const;

    [[nodiscard]] bool Is1DAlgorithm() const;

private:
    int _error; //! MeshComputeError
    AlgorithmDim _dim; //! Dimensions
};


#endif
