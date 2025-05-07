#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <numbers>

enum class FieldIndex : int {
    U = 0, // Velocity - u
    V = 1, // Velocity - v
    W = 2, // Velocity - w
    P = 3, // Pressure
    T = 4, // Temperature
    S = 5 // Indicator function
};

constexpr int nPhi = 6; // Number of design variables

constexpr int LOGICAL_TRUE = 1;
constexpr int LOGICAL_FALSE = 0;
constexpr int LOGICAL_ERROR = -1;

constexpr double PI = std::numbers::pi;
constexpr double SMALL = 1e-15;
constexpr double VSMALL = 1e-300;
constexpr double GREAT = 1e+15;
constexpr double VGREAT = 1e+300;

constexpr int MAX_FACES = 6;

template<typename T>
constexpr T LMAX(const T &a, const T &b) { return (a > b) ? a : b; }

template<typename T>
constexpr T LMIN(const T &a, const T &b) { return (a < b) ? a : b; }

template<typename T>
constexpr T LABS(const T &x) { return (x < 0) ? -x : x; }

template<typename T>
constexpr int LSGN(const T &x) { return (x < 0) ? -1 : 1; }

inline int verbose = 0;
inline int pchecks = 0;

class MeshException final : public std::exception {
    int errorCode_;
    std::string message_;
public:
    MeshException(std::string message, const int code)
        : errorCode_(code), message_(std::move(message)) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    int code() const noexcept {
        return errorCode_;
    }
};

#endif
