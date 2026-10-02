#ifndef TRIANGULO_H
#define TRIANGULO_H
#include <array>
#include "Geometria.h"

// Dados geometricos separados de OpenGL. A geometria existe somente aqui;
// a grade e os resultados de consultas guardam indices para estes objetos.
class Triangulo {
public:
    std::array<Ponto2D,3> vertices;
    Envelope2D envelope;
    Triangulo(Ponto2D a, Ponto2D b, Ponto2D c);
    bool PontoNoEnvelope(Ponto2D p) const;
    bool PontoNoTriangulo(Ponto2D p) const;
};
#endif
