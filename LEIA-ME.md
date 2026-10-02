# Trabalho 1 — Inclusão de ponto em triângulos

Implementação em C++14/OpenGL/GLUT a partir da base do professor. Use o Makefile deste diretório: o projeto Xcode antigo não foi atualizado para os novos módulos.

## Executar no Mac

```bash
make
./BasicoOpenGL
```

Exemplo reproduzível:

```bash
./BasicoOpenGL --n 1000 --seed 42 --size 1.5 --grid 16 --modo 4
```

`--size` controla a escala dos triângulos, não sua área exata. O cenário usa X em [-10,10] e Y em [-5,5]. Os triângulos são estáticos até a criação de um novo cenário. Inicialmente há 100 triângulos, semente 42, escala 1.5 e grade 10 × 10. O ponto começa em (0,0).

## Controles

| Controle | Ação |
|---|---|
| Clique esquerdo / arrastar | Posicionar o ponto |
| Setas | Mover o ponto em passos de 0.1 |
| 1 | Força bruta |
| 2 | Envelopes + teste detalhado |
| 3 | Grade + teste detalhado |
| 4 | Grade + envelopes + teste detalhado |
| F | Alternar filtro por envelopes |
| S | Alternar filtro por grade |
| E | Exibir/esconder envelopes roxos |
| G | Exibir/esconder grade e célula do ponto |
| C | Exibir/esconder candidatos da grade em azul |
| A | Exibir/esconder aprovados pelo envelope em laranja |
| V | Comparar os quatro modos e imprimir os IDs encontrados |
| R | Recriar exatamente a semente e parâmetros atuais |
| N | Criar um cenário com a próxima semente |
| + / - | Dobrar/reduzir à metade a quantidade |
| ] / [ | Dobrar/reduzir as dimensões da grade |
| . / , | Aumentar/reduzir a escala dos triângulos |
| Esc | Sair |

O verde indica o resultado final; o vermelho marca o ponto. As cores de etapas posteriores prevalecem no contorno; desligue A para observar os candidatos azuis que também passaram pelo envelope. O preenchimento verde é transparente para visualizar sobreposições. Desativar uma exibição nunca refaz a consulta nem muda o resultado. Desativar um filtro refaz a consulta para o mesmo ponto.

Mantenha a janela larga para ler todo o painel. A área geométrica preserva proporção 2:1 e ignora cliques no painel. O terminal também mostra todas as medidas.

## Verificação e experimentos

```bash
make test
make test-interface
./BasicoOpenGL --benchmark resultados.csv
python3 resumir_resultados.py resultados.csv
```

`make test` não depende de OpenGL. `make test-interface` testa os callbacks sem criar uma janela, mas vincula as bibliotecas OpenGL/GLUT. O benchmark também não abre janela. CSV e resumo são gravados no caminho indicado e na pasta do CSV, respectivamente; repetir os comandos substitui esses resultados.

A validação automática compara a lista completa de índices com força bruta. O cronômetro da consulta NÃO inclui essa validação, desenho ou impressão. A grade é pré-construída e só muda quando o cenário ou suas dimensões mudam. A construção da grade não entra no tempo de consulta; seu número de referências é registrado para mostrar seu custo de armazenamento.

## Organização do código

- `Geometria.h`: ponto, envelope, orientação e distância até segmento.
- `Triangulo.h/.cpp`: dados dos três vértices e teste detalhado com tolerância.
- `Consulta.h/.cpp`: geração reproduzível, grade de índices, quatro modos e métricas.
- `Prog-Triangulos.cpp`: OpenGL, cores, painel e callbacks de interação.
- `Experimentos.h/.cpp`: cenários e consultas repetidas, comparação de índices e CSV.
- `Testes.cpp`: expectativas geométricas e comparação entre modos em milhares de posições.
- `TestesInterface.cpp`: controles, conversão do mouse, redimensionamento e sementes.
- `resumir_resultados.py`: resumo das medições, apenas com biblioteca padrão Python.

O projeto usa novos módulos independentes de OpenGL para a geometria. Os arquivos antigos de outros exemplos permanecem na pasta original, mas não entram no Makefile atualizado. A cópia anterior foi preservada em `base-antes-das-alteracoes.zip` nos outputs desta conversa.

## Portabilidade e limites

Compilado e testado em macOS Intel com Apple Clang 14. O Makefile tem configuração Linux com bibliotecas GL/GLUT instaladas. `Makefile.mk` mantém a configuração MinGW/freeglut da base para Windows. Linux e Windows não foram executados nesta máquina. No Windows de 64 bits, copie `freeglut_dlls/x64/freeglut.dll` para a pasta do executável antes de rodar. O pacote de fontes inclui os arquivos locais freeglut usados pelo Makefile do Windows quando disponíveis.

A tolerância é 1e-8 unidades de mundo, adequada ao domínio fixo deste programa. Para outro domínio com magnitudes muito diferentes, revise sua escala. Triângulos degenerados são tratados como seus segmentos/pontos. Os filtros usam envelopes expandidos pela mesma tolerância.

Limites de entrada: N de 1 a 100000, grade de 1 a 128 por dimensão, escala de 0.01 a 10, semente uint32. Triângulos grandes com grades muito finas podem consumir bastante memória porque cada triângulo aparece em muitas células.

## Antes da entrega

1. Confirmar que grade uniforme e AABB estão entre as técnicas estudadas em aula.
2. Conferir a janela no seu terminal: cores, teclado, mouse, redimensionamento e troca de modos.
3. Ler o guia de estudo e explicar cada módulo.
4. Gravar o vídeo de até dois minutos seguindo `ROTEIRO-VIDEO.md`.
5. Colocar os nomes e o link do vídeo em `RELATORIO.md`.
6. Gerar o ZIP final com fontes, relatório, medições e instruções. O pacote fornecido ainda tem as pendências de vídeo e autoria indicadas no relatório.
