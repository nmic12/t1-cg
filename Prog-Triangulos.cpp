// PUCRS - Fundamentos de Computacao Grafica - Trabalho 1, 2026/2.
// Evolucao da base OpenGL/GLUT fornecida pelo Prof. Marcio Sarroglia Pinho.
// Geometria, grade e consultas ficam em modulos sem dependencia de OpenGL.
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "Consulta.h"
#include "Experimentos.h"
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

Configuracao config;
std::vector<Triangulo> Triangulos;
GradeUniforme grade;
ResultadoConsulta resultado;
Ponto2D PontoClicado{0,0};
Modo modo;
bool mostraEnvelopes=false, mostraGrade=true, mostraCandidatos=true, mostraAprovados=true;
bool validado=false;
int largura=1200, altura=760;
// Janela de desenho ocupa a parte abaixo do painel de informacoes.
int vx=0,vy=0,vw=1200,vh=600;

void ConsultaAtual() {
    resultado=ConsultaPonto(Triangulos,grade,PontoClicado,modo);
    // Validacao compara os indices, nao apenas a quantidade. Fora do cronometro.
    auto referencia=ConsultaPonto(Triangulos,grade,PontoClicado,{false,false});
    validado=resultado.encontrados==referencia.encontrados;
    std::cout<<"\n--- Consulta: "<<NomeModo(modo)<<" ---\nPonto: ("<<PontoClicado.x<<", "<<PontoClicado.y<<")\n";
    std::cout<<"Total: "<<Triangulos.size()<<" | Candidatos da grade: ";
    if(modo.grade) std::cout<<resultado.candidatosGrade.size(); else std::cout<<"desativada";
    std::cout<<" | Aprovados pelo envelope: ";
    if(modo.envelopes) std::cout<<resultado.aprovadosEnvelope.size(); else std::cout<<"desativado";
    std::cout<<"\nTestes de envelope: "<<resultado.testesEnvelope<<" | Testes detalhados: "<<resultado.testesDetalhados
             <<" | Encontrados: "<<resultado.encontrados.size()<<"\nTempo da consulta: "<<std::fixed<<std::setprecision(3)
             <<resultado.microssegundos<<" us | Indices iguais a forca bruta: "<<(validado?"SIM":"ERRO")<<'\n';
}
void RecriaCenario() {
    Triangulos=GeraTriangulos(config);
    grade.constroi(Triangulos,config.colunas,config.linhas);
    std::cout<<"Cenario: N="<<config.quantidade<<", semente="<<config.semente<<", tamanho="<<config.tamanho
             <<", grade="<<config.colunas<<'x'<<config.linhas<<'\n';
    ConsultaAtual();
}
void Retangulo(Envelope2D e,bool preenchido) {
    glBegin(preenchido?GL_QUADS:GL_LINE_LOOP);
    glVertex2d(e.min.x,e.min.y); glVertex2d(e.max.x,e.min.y);
    glVertex2d(e.max.x,e.max.y); glVertex2d(e.min.x,e.max.y); glEnd();
}
void DesenhaTriangulo(int i,bool preenchido) {
    glBegin(preenchido?GL_TRIANGLES:GL_LINE_LOOP);
    for(auto p:Triangulos[i].vertices) glVertex2d(p.x,p.y);
    glEnd();
}
void Texto(int x,int y,const std::string& texto) {
    glRasterPos2i(x,y);
    for(unsigned char c:texto) glutBitmapCharacter(GLUT_BITMAP_8_BY_13,c);
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(vx,vy,vw,vh);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); glOrtho(-10,10,-5,5,-1,1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    // A celula pode ser mostrada mesmo quando seu filtro esta desligado.
    int celula=grade.indice(PontoClicado);
    if(mostraGrade && celula>=0) {
        glColor3f(0.91f,0.92f,1.0f); Retangulo(grade.limitesCelula(celula),true);
    }
    if(mostraGrade) {
        glColor3f(0.80f,0.82f,0.87f);
        auto d=grade.dominio(); glBegin(GL_LINES);
        for(int x=0;x<=grade.colunas();++x) {
            double a=d.min.x+(d.max.x-d.min.x)*x/grade.colunas();
            glVertex2d(a,d.min.y); glVertex2d(a,d.max.y);
        }
        for(int y=0;y<=grade.linhas();++y) {
            double a=d.min.y+(d.max.y-d.min.y)*y/grade.linhas();
            glVertex2d(d.min.x,a); glVertex2d(d.max.x,a);
        }
        glEnd();
    }
    glColor3f(0.75f,0.77f,0.79f);
    for(std::size_t i=0;i<Triangulos.size();++i) DesenhaTriangulo(static_cast<int>(i),false);
    if(mostraEnvelopes) {
        glColor3f(0.57f,0.33f,0.70f);
        for(const auto& t:Triangulos) Retangulo(t.envelope,false);
    }
    glLineWidth(2);
    if(mostraCandidatos && modo.grade) {
        glColor3f(0.1f,0.40f,0.85f);
        for(int i:resultado.candidatosGrade) DesenhaTriangulo(i,false);
    }
    if(mostraAprovados && modo.envelopes) {
        glColor3f(0.95f,0.48f,0.08f);
        for(int i:resultado.aprovadosEnvelope) DesenhaTriangulo(i,false);
    }
    // Preenchimento transparente permite perceber sobreposicoes.
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f,0.70f,0.30f,0.22f);
    for(int i:resultado.encontrados) DesenhaTriangulo(i,true);
    glDisable(GL_BLEND);
    glColor3f(0.0f,0.45f,0.12f);
    for(int i:resultado.encontrados) DesenhaTriangulo(i,false);
    glLineWidth(1);
    glColor3f(0.85f,0.05f,0.10f); glPointSize(9);
    glBegin(GL_POINTS); glVertex2d(PontoClicado.x,PontoClicado.y); glEnd();

    // Painel em coordenadas de pixels, fora da area interativa do cenario.
    glViewport(0,0,largura,altura);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); glOrtho(0,largura,0,altura,-1,1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(0.12f,0.16f,0.22f);
    std::ostringstream s;
    s<<"Modo: "<<NomeModo(modo)<<" | N="<<config.quantidade<<" | Seed="<<config.semente<<" | Tamanho="<<config.tamanho
     <<" | Grade="<<config.colunas<<'x'<<config.linhas; Texto(12,altura-20,s.str());
    s.str(""); s.clear();
    s<<std::fixed<<std::setprecision(3)<<"Ponto ("<<PontoClicado.x<<", "<<PontoClicado.y<<") | Consulta: "<<resultado.microssegundos
     <<" us | Validacao: "<<(validado?"OK":"DIVERGENCIA"); Texto(12,altura-40,s.str());
    s.str(""); s.clear(); s<<"Grade: ";
    if(modo.grade) s<<resultado.candidatosGrade.size(); else s<<"off";
    s<<" | Envelope: "; if(modo.envelopes) s<<resultado.aprovadosEnvelope.size(); else s<<"off";
    s<<" | Testes detalhados: "<<resultado.testesDetalhados<<" | Encontrados: "<<resultado.encontrados.size(); Texto(12,altura-60,s.str());
    Texto(12,altura-80,"1: bruta  2: envelopes  3: grade  4: ambos | F: filtro envelope  S: filtro grade");
    Texto(12,altura-100,"E: exibir envelopes  G: exibir grade  C: candidatos azuis  A: aprovados laranja");
    Texto(12,altura-120,"Mouse/arrastar/setas: ponto | V: comparar 4 modos | R: recriar seed | N: proxima seed");
    Texto(12,altura-140,"+/-: quantidade  [ ]: grade  , .: tamanho | Verde: resultado  Roxo: envelope | Esc: sair");
    glutSwapBuffers();
}
void reshape(int w,int h) {
    largura=std::max(1,w); altura=std::max(1,h);
    int disponivel=std::max(1,altura-165);
    vw=std::min(largura,2*disponivel); vh=std::max(1,vw/2);
    vx=(largura-vw)/2; vy=(disponivel-vh)/2;
    glutPostRedisplay();
}
void MoveMouse(int x,int y) {
    int yy=altura-1-y;
    if(x<vx || x>=vx+vw || yy<vy || yy>=vy+vh) return;
    // Mesma projecao usada por display; independente da matriz do painel.
    PontoClicado={-10+20.0*(x-vx)/std::max(1,vw-1),-5+10.0*(yy-vy)/std::max(1,vh-1)};
    ConsultaAtual(); glutPostRedisplay();
}
void Mouse(int botao,int estado,int x,int y) {
    if(botao==GLUT_LEFT_BUTTON && estado==GLUT_DOWN) MoveMouse(x,y);
}
void ComparaModos() {
    auto referencia=ConsultaPonto(Triangulos,grade,PontoClicado,{false,false});
    for(int n=1;n<=4;++n) {
        auto m=ModoNumero(n); auto r=ConsultaPonto(Triangulos,grade,PontoClicado,m);
        std::cout<<NomeModo(m)<<": "<<r.testesDetalhados<<" testes, "<<r.microssegundos<<" us, "
                 <<(r.encontrados==referencia.encontrados?"indices iguais":"DIVERGENCIA")<<". IDs:";
        for(int i:r.encontrados) std::cout<<' '<<i;
        std::cout<<'\n';
    }
}
void keyboard(unsigned char key,int,int) {
    bool consulta=false,recria=false,reconstroi=false;
    switch(key) {
        case 27: std::exit(0);
        case '1': case '2': case '3': case '4': modo=ModoNumero(key-'0'); consulta=true; break;
        case 'f': case 'F': modo.envelopes=!modo.envelopes; consulta=true; break;
        case 's': case 'S': modo.grade=!modo.grade; consulta=true; break;
        case 'e': case 'E': mostraEnvelopes=!mostraEnvelopes; break;
        case 'g': case 'G': mostraGrade=!mostraGrade; break;
        case 'c': case 'C': mostraCandidatos=!mostraCandidatos; break;
        case 'a': case 'A': mostraAprovados=!mostraAprovados; break;
        case 'v': case 'V': ComparaModos(); break;
        case 'r': case 'R': recria=true; break;
        case 'n': case 'N': ++config.semente; recria=true; break;
        case '+': case '=': config.quantidade=std::min(100000,config.quantidade*2); recria=true; break;
        case '-': config.quantidade=std::max(1,config.quantidade/2); recria=true; break;
        case ']': config.colunas=std::min(128,config.colunas*2); config.linhas=std::min(128,config.linhas*2); reconstroi=true; break;
        case '[': config.colunas=std::max(1,config.colunas/2); config.linhas=std::max(1,config.linhas/2); reconstroi=true; break;
        case '.': config.tamanho=std::min(10.0,config.tamanho*1.5); recria=true; break;
        case ',': config.tamanho=std::max(0.01,config.tamanho/1.5); recria=true; break;
        default: break;
    }
    if(recria) RecriaCenario();
    else {
        if(reconstroi) { grade.constroi(Triangulos,config.colunas,config.linhas); consulta=true; }
        if(consulta) ConsultaAtual();
    }
    glutPostRedisplay();
}
void arrow_keys(int key,int,int) {
    switch(key) {
        case GLUT_KEY_LEFT: PontoClicado.x-=0.1; break;
        case GLUT_KEY_RIGHT: PontoClicado.x+=0.1; break;
        case GLUT_KEY_UP: PontoClicado.y+=0.1; break;
        case GLUT_KEY_DOWN: PontoClicado.y-=0.1; break;
        default: return;
    }
    PontoClicado.x=std::max(-10.0,std::min(10.0,PontoClicado.x));
    PontoClicado.y=std::max(-5.0,std::min(5.0,PontoClicado.y));
    ConsultaAtual(); glutPostRedisplay();
}
void Ajuda() {
    std::cout<<"Uso: ./BasicoOpenGL [--n 100] [--seed 42] [--size 1.5] [--grid 10] [--modo 1]\n"
             <<"     ./BasicoOpenGL --benchmark resultados.csv\n"
             <<"Modos: 1 bruta, 2 envelopes, 3 grade, 4 grade + envelopes.\n"
             <<"O benchmark nao abre janela. Use make test para os testes geometricos.\n";
}
int main(int argc,char** argv) {
    try {
        for(int i=1;i<argc;++i) {
            std::string a=argv[i];
            if(a=="--help") { Ajuda(); return 0; }
            if(i+1>=argc) throw std::invalid_argument("Falta valor para "+a);
            std::string v=argv[++i]; std::size_t usados=0;
            if(a=="--benchmark") {
                if(argc!=3) throw std::invalid_argument("Use --benchmark caminho.csv sem outras opcoes.");
                return ExecutaExperimentos(v);
            }
            if(a=="--size") config.tamanho=std::stod(v,&usados);
            else if(a=="--seed") {
                unsigned long long n=std::stoull(v,&usados);
                if(v[0]=='-' || n>4294967295ULL) throw std::invalid_argument("Semente fora do intervalo uint32.");
                config.semente=static_cast<std::uint32_t>(n);
            } else {
                int n=std::stoi(v,&usados);
                if(a=="--n") config.quantidade=n;
                else if(a=="--grid") {config.colunas=n;config.linhas=n;}
                else if(a=="--modo") { if(n<1 || n>4) throw std::invalid_argument("Modo entre 1 e 4."); modo=ModoNumero(n); }
                else throw std::invalid_argument("Opcao desconhecida: "+a);
            }
            if(usados!=v.size()) throw std::invalid_argument("Valor invalido: "+v);
        }
        RecriaCenario();
        // Nossas opcoes ja foram consumidas; GLUT recebe somente argv[0].
        int argcGlut=1; char* argvGlut[]={argv[0],nullptr}; glutInit(&argcGlut,argvGlut);
        glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB); glutInitWindowSize(largura,altura);
        glutCreateWindow("Trabalho 1 - Inclusao de ponto em triangulos");
        glClearColor(0.98f,0.98f,0.99f,1);
        glutDisplayFunc(display); glutReshapeFunc(reshape); glutKeyboardFunc(keyboard);
        glutSpecialFunc(arrow_keys); glutMouseFunc(Mouse); glutMotionFunc(MoveMouse);
        // Sem callback idle: triangulos estaticos, redesenho somente por eventos.
        glutMainLoop();
    } catch(const std::exception& e) { std::cerr<<"Erro: "<<e.what()<<'\n'; return 1; }
    return 0;
}
