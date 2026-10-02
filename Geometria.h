#ifndef GEOMETRIA_H
#define GEOMETRIA_H
#include <algorithm>
#include <cmath>

// Coordenadas do cenario em double. A tolerancia e uma distancia em unidades
// do mundo, adequado ao dominio deste trabalho: [-10,10] x [-5,5].
constexpr double TOLERANCIA = 1e-8;
struct Ponto2D { double x = 0, y = 0; };
struct Envelope2D {
    Ponto2D min, max;
    bool contem(Ponto2D p, double margem = TOLERANCIA) const {
        return p.x >= min.x-margem && p.x <= max.x+margem &&
               p.y >= min.y-margem && p.y <= max.y+margem;
    }
};
inline double orientacao(Ponto2D a, Ponto2D b, Ponto2D p) {
    return (b.x-a.x)*(p.y-a.y)-(b.y-a.y)*(p.x-a.x);
}
inline double distancia2(Ponto2D a, Ponto2D b) {
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
}
inline double distanciaSegmento2(Ponto2D p, Ponto2D a, Ponto2D b) {
    double comprimento2 = distancia2(a,b);
    if (comprimento2 == 0) return distancia2(p,a);
    double t = ((p.x-a.x)*(b.x-a.x)+(p.y-a.y)*(b.y-a.y))/comprimento2;
    t = std::max(0.0, std::min(1.0,t));
    return distancia2(p,{a.x+t*(b.x-a.x), a.y+t*(b.y-a.y)});
}
#endif
