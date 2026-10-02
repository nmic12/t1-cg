# Windows/MinGW-w64: depende do freeglut incluido na base.
CXX = g++
CXXFLAGS = -std=c++14 -O2 -g -Wall -Wextra -Iinclude
LDLIBS = -Llib/x64 -lfreeglut -lopengl32
FONTES = Triangulo.cpp Consulta.cpp Experimentos.cpp Prog-Triangulos.cpp
OBJETOS = $(FONTES:.cpp=.o)
all: ProgOpenGL.exe
ProgOpenGL.exe: $(OBJETOS)
	$(CXX) $(OBJETOS) $(LDLIBS) -o $@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
Testes.exe: Testes.cpp Consulta.cpp Triangulo.cpp
	$(CXX) $(CXXFLAGS) Testes.cpp Consulta.cpp Triangulo.cpp -o $@
test: Testes.exe
	./Testes.exe
-include $(OBJETOS:.o=.d)
