#include "Consulta.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <random>

int verificacoes=0;
void exige(bool cond,const char* texto) {
    ++verificacoes;
    if(!cond) throw std::runtime_error(texto);
}
void compara(const std::vector<Triangulo>& t,const GradeUniforme& g,Ponto2D p) {
    auto base=ConsultaPonto(t,g,p,{false,false});
    exige(base.testesDetalhados==t.size(),"Contador da forca bruta");
    for(int n=2;n<=4;++n) {
        auto m=ModoNumero(n); auto r=ConsultaPonto(t,g,p,m);
        exige(r.encontrados==base.encontrados,"Indices divergem da forca bruta");
        exige(r.testesDetalhados<=t.size(),"Testes excederam total");
        if(m.envelopes) {
            exige(r.testesDetalhados==r.aprovadosEnvelope.size(),"Contador de aprovados");
            exige(r.testesEnvelope==(m.grade?r.candidatosGrade.size():t.size()),"Contador de envelopes");
        }
    }
}
int main() {
    try {
        Triangulo t({0,0},{4,0},{0,4}), reverso({0,4},{4,0},{0,0});
        for(auto p:std::vector<Ponto2D>{{1,1},{0,0},{4,0},{0,4},{2,2},{2,0},{0,2}}) {
            exige(t.PontoNoTriangulo(p),"Ponto interno ou na borda rejeitado");
            exige(reverso.PontoNoTriangulo(p),"Orientacao inversa rejeitada");
        }
        exige(!t.PontoNoTriangulo({3,3}),"Ponto no envelope mas fora do triangulo");
        exige(!t.PontoNoTriangulo({-1,0}),"Externo aceito");
        exige(t.PontoNoTriangulo({2,-TOLERANCIA*0.5}),"Tolerancia da aresta");
        exige(!t.PontoNoTriangulo({2,-TOLERANCIA*2}),"Fora da tolerancia");
        Triangulo linha({0,0},{1,0},{2,0}), ponto({1,1},{1,1},{1,1});
        exige(linha.PontoNoTriangulo({1.5,0}),"Segmento degenerado");
        exige(!linha.PontoNoTriangulo({3,0}),"Reta infinita aceita por degenerado");
        exige(!linha.PontoNoTriangulo({1,1}),"Fora de segmento degenerado");
        exige(ponto.PontoNoTriangulo({1,1}),"Vertice repetido");
        exige(!ponto.PontoNoTriangulo({2,2}),"Degenerado aceitou qualquer ponto");
        std::vector<Triangulo> cena={t,reverso,linha,ponto,{{-10,-5},{10,-5},{0,5}}};
        for(int n:{1,2,8,32,128}) {
            GradeUniforme g; g.constroi(cena,n,n);
            for(auto p:std::vector<Ponto2D>{{1,1},{0,0},{4,0},{0,4},{2,2},{3,3},{11,6},{-10,-5},{10,-5},{0,5},{2,-TOLERANCIA*0.5},{-10-TOLERANCIA*0.5,-5}})
                compara(cena,g,p);
            for(int i=0;i<n*n;++i) {
                auto e=g.limitesCelula(i);
                compara(cena,g,e.min); compara(cena,g,e.max);
            }
            auto r=ConsultaPonto(cena,g,{1,1},ModoNumero(4));
            exige(r.encontrados.size()==4,"Sobreposicoes e ponto degenerado");
            auto vazio=ConsultaPonto(cena,g,{20,20},ModoNumero(4));
            exige(vazio.encontrados.empty() && vazio.testesDetalhados==0,"Consulta deve reiniciar resultados");
        }
        // Cenarios reproduziveis, pontos aleatorios, vertices e meios de arestas.
        for(unsigned seed:{0u,42u,2026u}) for(double tamanho:{0.1,1.5,10.0}) {
            Configuracao c; c.quantidade=200; c.semente=seed; c.tamanho=tamanho;
            auto v=GeraTriangulos(c), repetido=GeraTriangulos(c);
            for(std::size_t i=0;i<v.size();++i) for(int k=0;k<3;++k)
                exige(distancia2(v[i].vertices[k],repetido[i].vertices[k])==0,"Semente nao reproduzivel");
            for(int n:{1,8,32}) {
                GradeUniforme g; g.constroi(v,n,n);
                std::mt19937 rng(91);
                for(int i=0;i<300;++i) {
                    double x=-11+22.0*rng()/4294967296.0, y=-6+12.0*rng()/4294967296.0;
                    compara(v,g,{x,y});
                }
                for(const auto& tri:v) for(int k=0;k<3;++k) {
                    auto a=tri.vertices[k],b=tri.vertices[(k+1)%3];
                    compara(v,g,a); compara(v,g,{(a.x+b.x)*0.5,(a.y+b.y)*0.5});
                    compara(v,g,{a.x+TOLERANCIA*0.5,a.y});
                }
            }
        }
        GradeUniforme vazia; vazia.constroi({},4,4); compara({},vazia,{0,0});
        bool rejeitou=false;
        try { vazia.constroi({},0,2); } catch(const std::invalid_argument&) { rejeitou=true; }
        exige(rejeitou,"Grade invalida deveria ser rejeitada");
        std::cout<<"OK: "<<verificacoes<<" verificacoes; bordas, vertices, tolerancia, degenerados, sobreposicoes, grade e sementes.\n";
    } catch(const std::exception& e) { std::cerr<<"FALHOU: "<<e.what()<<'\n'; return 1; }
}
