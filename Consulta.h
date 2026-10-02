#ifndef CONSULTA_H
#define CONSULTA_H
#include <cstdint>
#include <vector>
#include "Triangulo.h"

struct Configuracao {
    int quantidade=100, colunas=10, linhas=10;
    std::uint32_t semente=42;
    double tamanho=1.5;
};
struct Modo { bool grade=false, envelopes=false; };
const char* NomeModo(Modo modo);
Modo ModoNumero(int numero);
std::vector<Triangulo> GeraTriangulos(const Configuracao& config);

class GradeUniforme {
    Envelope2D dominio_;
    int colunas_=1, linhas_=1;
    std::vector<std::vector<int>> celulas_;
public:
    void constroi(const std::vector<Triangulo>& triangulos, int colunas, int linhas);
    int indice(Ponto2D p) const;
    const std::vector<int>& candidatos(Ponto2D p) const;
    Envelope2D limitesCelula(int indice) const;
    Envelope2D dominio() const { return dominio_; }
    int colunas() const { return colunas_; }
    int linhas() const { return linhas_; }
    std::size_t referencias() const;
};
struct ResultadoConsulta {
    std::vector<int> candidatosGrade, aprovadosEnvelope, encontrados;
    std::size_t testesEnvelope=0, testesDetalhados=0;
    int celula=-1;
    double microssegundos=0;
};
ResultadoConsulta ConsultaPonto(const std::vector<Triangulo>& triangulos,
                               const GradeUniforme& grade, Ponto2D p, Modo modo);
#endif
