#include "Experimentos.h"
#include "Consulta.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>

int ExecutaExperimentos(const std::string& caminho) {
    std::ofstream csv(caminho);
    if(!csv) throw std::runtime_error("Nao foi possivel criar "+caminho);
    csv << "n,seed,tamanho,grade,colunas,linhas,ponto_id,x,y,modo,candidatos_grade,aprovados_envelope,testes_envelope,testes_detalhados,encontrados,tempo_us,validado,referencias_grade,area_media\n";
    csv << std::setprecision(12);
    std::size_t consultas=0;
    // Mesmo cenario e mesmos pontos para os quatro modos. Variacao fatorial.
    for(int n:{10,1000,10000}) for(unsigned seed:{42u,2026u}) for(double tamanho:{0.4,6.0}) {
        Configuracao config; config.quantidade=n; config.semente=seed; config.tamanho=tamanho;
        auto t=GeraTriangulos(config);
        double area=0; for(const auto& tri:t) area+=std::abs(orientacao(tri.vertices[0],tri.vertices[1],tri.vertices[2]))*0.5;
        area/=t.size();
        std::vector<Ponto2D> pontos={{0,0},{-10,-5},{10,5},{-10,5},{10,-5},{11,6}};
        for(int i=0;i<3;++i) pontos.push_back(t[0].vertices[i]);
        pontos.push_back({(t[0].vertices[0].x+t[0].vertices[1].x)*0.5,(t[0].vertices[0].y+t[0].vertices[1].y)*0.5});
        std::mt19937 rng(9123);
        for(int i=0;i<20;++i) {
            double x=-10+20*(static_cast<double>(rng())/4294967296.0);
            double y=-5+10*(static_cast<double>(rng())/4294967296.0);
            pontos.push_back({x,y});
        }
        for(int grade:{1,8,32}) {
            GradeUniforme g; g.constroi(t,grade,grade);
            for(std::size_t p=0;p<pontos.size();++p) {
                auto referencia=ConsultaPonto(t,g,pontos[p],{false,false}).encontrados;
                // Aquecimento fora da medicao registrada.
                for(int m=1;m<=4;++m) ConsultaPonto(t,g,pontos[p],ModoNumero(m));
                // Rotacao da ordem reduz o vies de executar sempre um modo antes.
                for(int repeticao=0;repeticao<5;++repeticao) for(int k=0;k<4;++k) {
                    int modo=(k+repeticao)%4+1;
                    auto m=ModoNumero(modo); auto r=ConsultaPonto(t,g,pontos[p],m);
                    if(r.encontrados!=referencia) throw std::runtime_error("Divergencia no experimento!");
                    csv<<n<<','<<seed<<','<<tamanho<<','<<grade<<','<<grade<<','<<grade<<','<<p<<','<<pontos[p].x<<','<<pontos[p].y<<','<<modo<<',';
                    if(m.grade) csv<<r.candidatosGrade.size();
                    csv<<','; if(m.envelopes) csv<<r.aprovadosEnvelope.size();
                    csv<<','<<r.testesEnvelope<<','<<r.testesDetalhados<<','<<r.encontrados.size()<<','<<r.microssegundos<<",1,"<<g.referencias()<<','<<area<<'\n';
                    ++consultas;
                }
            }
        }
    }
    csv.flush();
    if(!csv) throw std::runtime_error("Falha ao gravar CSV");
    std::cout<<"Experimentos: "<<consultas<<" consultas validadas. CSV: "<<caminho<<'\n';
    return 0;
}
