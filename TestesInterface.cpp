// Testa callbacks sem abrir janela. O desenho real exige sessao grafica.
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
static void RedesenhoSimulado() {}
#define glutPostRedisplay RedesenhoSimulado
#define main main_programa
#include "Prog-Triangulos.cpp"
#undef main
#undef glutPostRedisplay
#include <cassert>
int main() {
    std::ostringstream log;
    auto* original=std::cout.rdbuf(log.rdbuf());
    RecriaCenario(); reshape(1200,760);
    auto encontrados=resultado.encontrados;
    double tempo=resultado.microssegundos;
    for(char tecla:std::string("eEgGcCaA")) keyboard(tecla,0,0);
    assert(resultado.encontrados==encontrados && resultado.microssegundos==tempo);
    MoveMouse(vx,altura-1-vy);
    assert(PontoClicado.x==-10 && PontoClicado.y==-5);
    MoveMouse(vx+vw-1,altura-vy-vh);
    assert(PontoClicado.x==10 && PontoClicado.y==5);
    reshape(800,900); MoveMouse(vx+vw-1,altura-vy-vh);
    assert(PontoClicado.x==10 && PontoClicado.y==5);
    auto p=PontoClicado; MoveMouse(5,5); // Painel deve ignorar cliques.
    assert(PontoClicado.x==p.x && PontoClicado.y==p.y);
    PontoClicado={0,0}; ConsultaAtual();
    for(int m=1;m<=4;++m) {
        keyboard('0'+m,0,0);
        auto esperado=ModoNumero(m);
        assert(modo.grade==esperado.grade && modo.envelopes==esperado.envelopes && validado);
    }
    keyboard('f',0,0); assert(modo.grade && !modo.envelopes);
    keyboard('s',0,0); assert(!modo.grade && !modo.envelopes);
    keyboard(']',0,0); assert(grade.colunas()==20 && validado);
    keyboard('[',0,0); assert(grade.colunas()==10 && validado);
    keyboard('+',0,0); assert(Triangulos.size()==200 && validado);
    keyboard('-',0,0); assert(Triangulos.size()==100 && validado);
    auto v=Triangulos[0].vertices;
    keyboard('r',0,0); assert(distancia2(v[0],Triangulos[0].vertices[0])==0);
    keyboard('n',0,0); assert(config.semente==43 && distancia2(v[0],Triangulos[0].vertices[0])>0);
    arrow_keys(GLUT_KEY_RIGHT,0,0); assert(PontoClicado.x==0.1 && validado);
    std::cout.rdbuf(original);
    std::cout<<"OK: callbacks, quatro modos, toggles visuais, coordenadas, resize, sementes e parametros (sem janela).\n";
}
