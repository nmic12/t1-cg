#include "Consulta.h"
#include <chrono>
#include <numeric>
#include <random>
#include <stdexcept>

const char* NomeModo(Modo m) {
    if(m.grade) return m.envelopes ? "grade + envelopes" : "grade";
    return m.envelopes ? "envelopes" : "forca bruta";
}
Modo ModoNumero(int n) { return {n>=3,n==2 || n==4}; }
std::vector<Triangulo> GeraTriangulos(const Configuracao& c) {
    if(c.quantidade<1 || c.quantidade>100000 || !std::isfinite(c.tamanho) || c.tamanho<0.01 || c.tamanho>10)
        throw std::invalid_argument("Use N entre 1 e 100000 e tamanho entre 0.01 e 10.");
    // Conversao explicita dos inteiros do mt19937, reproduzivel entre bibliotecas.
    std::mt19937 gerador(c.semente);
    auto uniforme=[&](double a,double b) { return a+(b-a)*(static_cast<double>(gerador())/4294967296.0); };
    std::vector<Triangulo> t;
    t.reserve(c.quantidade);
    const double pi=3.14159265358979323846;
    for(int i=0;i<c.quantidade;++i) {
        double raio=uniforme(0.5,1.0)*c.tamanho*0.5;
        double x=uniforme(-10+raio,10-raio), y=uniforme(-5+raio,5-raio);
        double angulo=uniforme(0,2*pi);
        Ponto2D v[3];
        for(int k=0;k<3;++k) {
            double a=angulo+k*2*pi/3+uniforme(-0.30,0.30);
            double r=raio*uniforme(0.65,1.0);
            v[k]={x+r*std::cos(a),y+r*std::sin(a)};
        }
        t.emplace_back(v[0],v[1],v[2]);
    }
    return t;
}
void GradeUniforme::constroi(const std::vector<Triangulo>& t,int colunas,int linhas) {
    if(colunas<1 || linhas<1 || colunas>128 || linhas>128)
        throw std::invalid_argument("Grade deve ter de 1 a 128 linhas/colunas.");
    colunas_=colunas; linhas_=linhas;
    dominio_={{-10,-5},{10,5}};
    for(const auto& tri:t) {
        dominio_.min.x=std::min(dominio_.min.x,tri.envelope.min.x);
        dominio_.min.y=std::min(dominio_.min.y,tri.envelope.min.y);
        dominio_.max.x=std::max(dominio_.max.x,tri.envelope.max.x);
        dominio_.max.y=std::max(dominio_.max.y,tri.envelope.max.y);
    }
    dominio_.min.x-=TOLERANCIA; dominio_.min.y-=TOLERANCIA;
    dominio_.max.x+=TOLERANCIA; dominio_.max.y+=TOLERANCIA;
    celulas_.assign(colunas_*linhas_,{});
    // Insere em TODAS as celulas do envelope expandido, nunca so no centro.
    // Pode gerar candidatos extras, mas nao perde triangulos que cruzam celulas.
    for(std::size_t i=0;i<t.size();++i) {
        const auto& e=t[i].envelope;
        int inicio=indice({e.min.x-TOLERANCIA,e.min.y-TOLERANCIA});
        int fim=indice({e.max.x+TOLERANCIA,e.max.y+TOLERANCIA});
        for(int y=inicio/colunas_;y<=fim/colunas_;++y)
            for(int x=inicio%colunas_;x<=fim%colunas_;++x)
                celulas_[y*colunas_+x].push_back(static_cast<int>(i));
    }
}
int GradeUniforme::indice(Ponto2D p) const {
    if(!dominio_.contem(p,0)) return -1;
    int x=static_cast<int>(std::floor((p.x-dominio_.min.x)/(dominio_.max.x-dominio_.min.x)*colunas_));
    int y=static_cast<int>(std::floor((p.y-dominio_.min.y)/(dominio_.max.y-dominio_.min.y)*linhas_));
    // A borda maxima pertence a ultima celula.
    x=std::max(0,std::min(colunas_-1,x)); y=std::max(0,std::min(linhas_-1,y));
    return y*colunas_+x;
}
const std::vector<int>& GradeUniforme::candidatos(Ponto2D p) const {
    static const std::vector<int> vazio;
    int i=indice(p);
    return i<0 ? vazio : celulas_[i];
}
Envelope2D GradeUniforme::limitesCelula(int i) const {
    if(i<0 || i>=static_cast<int>(celulas_.size())) throw std::out_of_range("Celula invalida");
    double dx=(dominio_.max.x-dominio_.min.x)/colunas_, dy=(dominio_.max.y-dominio_.min.y)/linhas_;
    int x=i%colunas_, y=i/colunas_;
    return {{dominio_.min.x+x*dx,dominio_.min.y+y*dy},
            {dominio_.min.x+(x+1)*dx,dominio_.min.y+(y+1)*dy}};
}
std::size_t GradeUniforme::referencias() const {
    std::size_t n=0; for(const auto& c:celulas_) n+=c.size(); return n;
}
ResultadoConsulta ConsultaPonto(const std::vector<Triangulo>& t,const GradeUniforme& g,Ponto2D p,Modo m) {
    const auto inicio=std::chrono::steady_clock::now();
    ResultadoConsulta r;
    if(m.grade) { r.celula=g.indice(p); r.candidatosGrade=g.candidatos(p); }
    auto testa=[&](int i) {
        if(m.envelopes) {
            ++r.testesEnvelope;
            if(!t[i].PontoNoEnvelope(p)) return;
            r.aprovadosEnvelope.push_back(i);
        }
        ++r.testesDetalhados;
        if(t[i].PontoNoTriangulo(p)) r.encontrados.push_back(i);
    };
    if(m.grade) { for(int i:r.candidatosGrade) testa(i); }
    else { for(std::size_t i=0;i<t.size();++i) testa(static_cast<int>(i)); }
    r.microssegundos=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-inicio).count();
    return r;
}
