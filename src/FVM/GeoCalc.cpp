#include "GeoCalc.hpp"
#include "FvmMesh.hpp"
#include "Globals.hpp"

Vector3 GeoCrossVector(const Vector3 &v1, const Vector3 &v2) {
    Vector3 vec;

    vec.x = v1.y * v2.z - v1.z * v2.y;
    vec.y = v1.z * v2.x - v1.x * v2.z;
    vec.z = v1.x * v2.y - v1.y * v2.x;

    return vec;
}

Vector3 GeoCalcNormal(const Vector3 &n1, const Vector3 &n2, const Vector3 &n3) {
    const Vector3 v1 = GeoSubVectorVector(n2, n1);
    const Vector3 v2 = GeoSubVectorVector(n3, n1);

    Vector3 rv = GeoCrossVector(v1, v2);

    rv = GeoNormalizeVector(rv);

    return rv;
}

Vector3 GeoSubVectorVector(const Vector3 &n2, const Vector3 &n1) {
    Vector3 rv;

    rv.x = n2.x - n1.x;
    rv.y = n2.y - n1.y;
    rv.z = n2.z - n1.z;

    return rv;
}

Vector3 GeoNormalizeVector(const Vector3 &v1) {
    Vector3 rv = v1;

    const double length = GeoMagVector(v1);

    constexpr double minNormalLength = 0.000000000001f;

    if (length < minNormalLength) {
        rv.x = 1.0;
        rv.y = 0.0;
        rv.z = 0.0;

        return rv;
    }

    const double factor = 1.0 / length;

    rv.x *= factor;
    rv.y *= factor;
    rv.z *= factor;

    return rv;
}

Vector3 GeoCalcCentroid3(const Vector3 &n1, const Vector3 &n2, const Vector3 &n3) {
    Vector3 rv;

    rv.x = (n1.x + n2.x + n3.x) / 3.0f;
    rv.y = (n1.y + n2.y + n3.y) / 3.0f;
    rv.z = (n1.z + n2.z + n3.z) / 3.0f;

    return rv;
}

Vector3 GeoCalcCentroid4(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3, const Vector3 &n4) {
    Vector3 rv;

    rv.x = (n1.x + n2.x + n3.x + n4.x) / 4.0f;
    rv.y = (n1.y + n2.y + n3.y + n4.y) / 4.0f;
    rv.z = (n1.z + n2.z + n3.z + n4.z) / 4.0f;

    return rv;
}

Vector3 GeoCalcCentroid6(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3,
    const Vector3 &n4, const Vector3 &n5, const Vector3 &n6) {
    Vector3 rv;

    rv.x = (n1.x + n2.x + n3.x + n4.x + n5.x + n6.x) / 6.0f;
    rv.y = (n1.y + n2.y + n3.y + n4.y + n5.y + n6.y) / 6.0f;
    rv.z = (n1.z + n2.z + n3.z + n4.z + n5.z + n6.z) / 6.0f;

    return rv;
}

Vector3 GeoCalcCentroid8(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3,
    const Vector3 &n4, const Vector3 &n5, const Vector3 &n6,
    const Vector3 &n7, const Vector3 &n8) {
    Vector3 rv;

    rv.x = (n1.x + n2.x + n3.x + n4.x + n5.x + n6.x + n7.x + n8.x) / 8.0f;
    rv.y = (n1.y + n2.y + n3.y + n4.y + n5.y + n6.y + n7.y + n8.y) / 8.0f;
    rv.z = (n1.z + n2.z + n3.z + n4.z + n5.z + n6.z + n7.z + n8.z) / 8.0f;

    return rv;
}

Vector3 GeoMultScalarVector(const double s, const Vector3 &n1) {
    Vector3 rv;

    rv.x = s * n1.x;
    rv.y = s * n1.y;
    rv.z = s * n1.z;

    return rv;
}

double GeoDotVectorVector(const Vector3 &n2, const Vector3 &n1) {
    return n2.x * n1.x + n2.y * n1.y + n2.z * n1.z;
}

double GeoMagVector(const Vector3 &v1) {
    return sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
}

double GeoCalcTriArea(const Vector3 &n1, const Vector3 &n2, const Vector3 &n3) {
    Vector3 c[3];
    Vector3 sum;

    c[0] = GeoCrossVector(n1, n2);
    c[1] = GeoCrossVector(n2, n3);
    c[2] = GeoCrossVector(n3, n1);

    sum.x = c[0].x + c[1].x + c[2].x;
    sum.y = c[0].y + c[1].y + c[2].y;
    sum.z = c[0].z + c[1].z + c[2].z;

    auto [x, y, z] = GeoCalcNormal(n1, n2, n3);

    return (0.5 * LABS(x * sum.x + y * sum.y + z * sum.z));
}

double GeoCalcQuadArea(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3, const Vector3 &n4) {
    double area = 0.0;

    area += GeoCalcTriArea(n1, n2, n3);
    area += GeoCalcTriArea(n1, n3, n4);

    return area;
}

double GeoCalcTetraVolume(const Vector3 &n1, const Vector3 &n2, const Vector3 &n3, const Vector3 &n4) {
    return LABS(
               (n2.x - n1.x) * ((n3.y - n1.y) * (n4.z - n1.z) - (n4.y - n1.y) * (n3.z - n1.z)) -
               (n3.x - n1.x) * ((n2.y - n1.y) * (n4.z - n1.z) - (n4.y - n1.y) * (n2.z - n1.z)) +
               (n4.x - n1.x) * ((n2.y - n1.y) * (n3.z - n1.z) - (n3.y - n1.y) * (n2.z - n1.z))) / 6.0f;
}

double GeoCalcHexaVolume(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3,
    const Vector3 &n4, const Vector3 &n5, const Vector3 &n6,
    const Vector3 &n7, const Vector3 &n8) {
    double volume = 0.0;

    const Vector3 c = GeoCalcCentroid8(n1, n2, n3, n4, n5, n6, n7, n8);

    volume += GeoCalcTetraVolume(c, n1, n2, n3);
    volume += GeoCalcTetraVolume(c, n3, n4, n1);
    volume += GeoCalcTetraVolume(c, n8, n7, n6);
    volume += GeoCalcTetraVolume(c, n5, n6, n8);
    volume += GeoCalcTetraVolume(c, n6, n7, n3);
    volume += GeoCalcTetraVolume(c, n6, n2, n3);
    volume += GeoCalcTetraVolume(c, n4, n1, n5);
    volume += GeoCalcTetraVolume(c, n4, n5, n8);
    volume += GeoCalcTetraVolume(c, n1, n5, n6);
    volume += GeoCalcTetraVolume(c, n1, n6, n2);
    volume += GeoCalcTetraVolume(c, n8, n7, n3);
    volume += GeoCalcTetraVolume(c, n8, n3, n4);

    return volume;
}

double GeoCalcPrismVolume(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3,
    const Vector3 &n4, const Vector3 &n5, const Vector3 &n6) {
    double volume = 0.0;

    volume += GeoCalcTetraVolume(n1, n2, n3, n6);
    volume += GeoCalcTetraVolume(n2, n4, n5, n6);
    volume += GeoCalcTetraVolume(n1, n2, n4, n6);
    volume += GeoCalcTetraVolume(n1, n2, n3, n4);
    volume += GeoCalcTetraVolume(n2, n4, n5, n3);
    volume += GeoCalcTetraVolume(n4, n5, n6, n3);
    volume *= 0.5;

    return volume;
}
