#include "Triangulo.h"
Triangulo::Triangulo(Ponto2D a, Ponto2D b, Ponto2D c) : vertices{{a,b,c}} {
    envelope = {{std::min({a.x,b.x,c.x}),std::min({a.y,b.y,c.y})},
                {std::max({a.x,b.x,c.x}),std::max({a.y,b.y,c.y})}};
}
bool Triangulo::PontoNoEnvelope(Ponto2D p) const { return envelope.contem(p); }
bool Triangulo::PontoNoTriangulo(Ponto2D p) const {
    const auto a=vertices[0], b=vertices[1], c=vertices[2];
    const double maiorLado=std::sqrt(std::max({distancia2(a,b),distancia2(b,c),distancia2(c,a)}));
    // Triangulos colineares ou quase degenerados representam seus segmentos.
    // O teste de sinais sozinho aceitaria pontos distantes na mesma reta.
    if (std::abs(orientacao(a,b,c)) > TOLERANCIA*maiorLado) {
        double d1=orientacao(a,b,p), d2=orientacao(b,c,p), d3=orientacao(c,a,p);
        bool negativo=d1<0 || d2<0 || d3<0;
        bool positivo=d1>0 || d2>0 || d3>0;
        if (!(negativo && positivo)) return true;
    }
    // Faixa de tolerancia por distancia: evita misturar area com distancia.
    // Essa faixa cabe no AABB expandido pela mesma TOLERANCIA.
    return distanciaSegmento2(p,a,b)<=TOLERANCIA*TOLERANCIA ||
           distanciaSegmento2(p,b,c)<=TOLERANCIA*TOLERANCIA ||
           distanciaSegmento2(p,c,a)<=TOLERANCIA*TOLERANCIA;
}
