# Compilacao apenas dos modulos do Trabalho 1. Outros exemplos ficam preservados.
CXX ?= c++
CXXFLAGS ?= -std=c++14 -O2 -g -Wall -Wextra -pedantic
CPPFLAGS += -DGL_SILENCE_DEPRECATION
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDLIBS = -framework OpenGL -framework GLUT
else
LDLIBS = -lGL -lglut
endif
FONTES = Triangulo.cpp Consulta.cpp Experimentos.cpp Prog-Triangulos.cpp
OBJETOS = $(FONTES:.cpp=.o)
.PHONY: all test test-interface run clean Darwin Linux
all: BasicoOpenGL
Darwin Linux: all
BasicoOpenGL: $(OBJETOS)
	$(CXX) $(CXXFLAGS) $(OBJETOS) $(LDLIBS) -o $@
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@
Testes: Testes.cpp Triangulo.cpp Consulta.cpp Consulta.h Triangulo.h Geometria.h
	$(CXX) $(CXXFLAGS) Testes.cpp Triangulo.cpp Consulta.cpp -o $@
test: Testes
	./Testes
TestesInterface: TestesInterface.cpp $(FONTES) Consulta.h Triangulo.h Geometria.h Experimentos.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) TestesInterface.cpp Triangulo.cpp Consulta.cpp Experimentos.cpp $(LDLIBS) -o $@
test-interface: TestesInterface
	./TestesInterface
run: BasicoOpenGL
	./BasicoOpenGL
clean:
	rm -f $(OBJETOS) $(OBJETOS:.o=.d) BasicoOpenGL Testes TestesInterface
-include $(OBJETOS:.o=.d)
