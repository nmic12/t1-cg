# Relatório — Inclusão de ponto em triângulos com aceleração

**Disciplina:** Fundamentos de Computação Gráfica — 2026/2  
**Autores:** preencher antes da entrega  
**Vídeo (até 2 minutos):** inserir link após gravar  
**Data dos experimentos:** 01/10/2026

Este relatório descreve a implementação e medições efetivamente executadas. Ainda faltam identificação dos autores, link do vídeo e conferência visual final na sessão gráfica do aluno. A técnica escolhida foi grade uniforme com envelopes AABB; confirmar sua correspondência com as técnicas vistas em aula.

## 1. Problema e solução

O programa armazena as coordenadas globais dos três vértices de cada triângulo e identifica todos os que contêm um ponto. O ponto é controlado por clique, arrasto ou setas. O cenário é gerado por números pseudoaleatórios com semente explícita. Optou-se pela geração aleatória, uma das alternativas do enunciado; não há leitura de cenários por arquivo.

Os quatro modos são: força bruta; envelopes e teste detalhado; grade e teste detalhado; grade, envelopes e teste detalhado. Eles compartilham os mesmos dados e o mesmo teste detalhado. Os filtros são independentes das opções de visualização.

## 2. Geometria e tolerância

Um ponto dentro de um triângulo não degenerado fica do mesmo lado de suas três arestas orientadas. O teste verifica os sinais de produtos vetoriais 2D. A orientação dos vértices pode ser horária ou anti-horária. Pontos na borda são aceitos. Uma tolerância de 1e-8 unidades de mundo é aplicada por distância até os segmentos; o domínio do programa é [-10,10] × [-5,5].

Triângulos colineares ou com área orientada menor ou igual à tolerância vezes seu maior lado são tratados pela união de seus segmentos. Isso evita o falso positivo que ocorreria ao usar somente sinais em um triângulo degenerado. Os envelopes e a indexação espacial são expandidos pela mesma tolerância para não eliminar os pontos próximos da borda que o teste detalhado aceita.

## 3. Aceleração e estruturas

Cada AABB armazena os extremos X/Y do triângulo. A grade é uma matriz de listas de índices. Cada triângulo é inserido em todas as células cobertas pelo seu AABB expandido. Não se copia a geometria para as células. Esse método é conservador: células podem conter candidatos extras, que os testes posteriores descartam.

A grade é construída ao criar o cenário e reconstruída apenas ao alterar o cenário ou suas dimensões. A consulta determina uma única célula para o ponto, com tratamento da borda máxima. Um triângulo não aparece duplicado em uma mesma lista. A força bruta serve de referência, comparando todos os IDs encontrados, não só a contagem. Essa validação é feita fora da região cronometrada.

## 4. Visualização e controles

Os contornos cinza mostram o cenário; azul indica candidatos da grade; laranja indica aprovação pelo envelope; verde destaca os triângulos encontrados; roxo mostra os envelopes e vermelho marca o ponto. A célula que contém o ponto tem fundo destacado. O preenchimento final é transparente para permitir ver sobreposições. As cores das etapas posteriores prevalecem no contorno.

As teclas 1 a 4 selecionam os modos. F/S alternam filtros; E/G/C/A controlam as exibições. V compara todos os modos no ponto corrente. A janela e o console exibem posição, total de triângulos, candidatos, testes detalhados, encontrados e tempo. Contadores e listas reiniciam a cada consulta. Etapas desativadas aparecem como off/desativadas, não como aprovações iguais a zero.

A visualização apenas consome o resultado armazenado. Mover o ponto ou alterar o modo realiza nova consulta; redesenhar a janela não refaz o algoritmo. O mouse é convertido de pixels para coordenadas do mundo usando a área de desenho e a inversão de Y, inclusive após redimensionamento.

## 5. Método experimental

- Quantidades: 10, 1.000 e 10.000 triângulos.
- Sementes: 42 e 2026.
- Escalas geométricas: 0.4 e 6.0; a área média real de cada cenário está no CSV.
- Grades: 1 × 1, 8 × 8 e 32 × 32.
- Trinta posições por cenário: centro, quatro cantos, um ponto externo, três vértices e o meio de uma aresta do primeiro triângulo, mais vinte pontos pseudoaleatórios de semente fixa.
- Quatro modos e cinco repetições para cada configuração/posição, com aquecimento prévio e rotação da ordem de execução.

Total: 36 configurações de cenário/grade × 30 posições × 4 modos × 5 repetições = **21.600 consultas medidas**. Os mesmos cenários e posições são reutilizados entre os modos. A geração usa mt19937 com conversão explícita de seus inteiros para coordenadas, permitindo reproduzir a geometria.

Ambiente medido: macOS em Intel x86_64, Apple Clang 14, C++14, otimização -O2. O tempo usa std::chrono::steady_clock, em microssegundos, e inclui a coleta das listas de candidatos/resultados. Não inclui criação do cenário, construção da grade, validação de referência, impressão ou desenho. O programa interativo também executa a referência a cada consulta, de modo que o tempo total de resposta inclui trabalho adicional não representado pelo cronômetro do algoritmo selecionado.

A construção e o armazenamento da grade têm custo: o CSV registra seu número de referências, mas não cronometra a construção. Não se deve inferir desempenho total de aplicação apenas a partir do tempo de consulta. São medições locais curtas, sujeitas a cache, relógio e escalonamento do sistema; a média não deve ser interpretada como garantia de velocidade em outra máquina.

## 6. Resultados medidos

As tabelas abaixo agregam as duas sementes, trinta posições e cinco repetições: 300 medições por linha. Tempos em microssegundos. As quantidades de candidatos e testes são médias e podem ser fracionárias. “—” significa etapa desativada.

### N=10; escala=0.4; grade=1 × 1

| Modo | Candidatos grade | Aprovados envelope | Testes detalhados | Tempo médio (µs) | Mediana (µs) |
|---|---:|---:|---:|---:|---:|
| Força bruta | — | — | 10.00 | 0.39 | 0.39 |
| Envelopes | — | 0.13 | 0.13 | 0.17 | 0.15 |
| Grade | 9.67 | — | 9.67 | 0.58 | 0.50 |
| Grade + envelopes | 9.67 | 0.13 | 0.13 | 0.27 | 0.25 |

### N=10.000; escala=0.4; grade=32 × 32

| Modo | Candidatos grade | Aprovados envelope | Testes detalhados | Tempo médio (µs) | Mediana (µs) |
|---|---:|---:|---:|---:|---:|
| Força bruta | — | — | 10000.00 | 261.45 | 256.82 |
| Envelopes | — | 1.92 | 1.92 | 100.05 | 97.57 |
| Grade | 18.85 | — | 18.85 | 1.02 | 0.98 |
| Grade + envelopes | 18.85 | 1.92 | 1.92 | 0.69 | 0.58 |

### N=10.000; escala=6; grade=1 × 1

| Modo | Candidatos grade | Aprovados envelope | Testes detalhados | Tempo médio (µs) | Mediana (µs) |
|---|---:|---:|---:|---:|---:|
| Força bruta | — | — | 10000.00 | 265.51 | 260.90 |
| Envelopes | — | 606.92 | 606.92 | 137.62 | 148.66 |
| Grade | 9666.67 | — | 9666.67 | 255.55 | 258.50 |
| Grade + envelopes | 9666.67 | 606.92 | 606.92 | 134.79 | 148.67 |

### N=10.000; escala=6; grade=32 × 32

| Modo | Candidatos grade | Aprovados envelope | Testes detalhados | Tempo médio (µs) | Mediana (µs) |
|---|---:|---:|---:|---:|---:|
| Força bruta | — | — | 10000.00 | 277.70 | 261.77 |
| Envelopes | — | 606.92 | 606.92 | 140.77 | 149.98 |
| Grade | 790.50 | — | 790.50 | 30.05 | 25.76 |
| Grade + envelopes | 790.50 | 606.92 | 606.92 | 30.26 | 26.22 |

### Discussão

No cenário com 10.000 triângulos pequenos e grade 32 × 32, a média passou de 10.000 testes detalhados para 1,92 no modo combinado, e o tempo médio medido passou de 261,45 µs para 0,69 µs. No cenário de apenas 10 triângulos e grade 1 × 1, a grade isolada levou 0,58 µs contra 0,39 µs da força bruta: a estrutura não compensou. Para triângulos grandes e grade 32 × 32, adicionar envelopes reduziu os testes de 790,50 para 606,92, mas o tempo médio ficou próximo (30,05 µs com grade e 30,26 µs combinado); a pequena diferença não sustenta uma vantagem de velocidade do filtro adicional nesta execução.

Com muitos triângulos pequenos, a grade fina reduz fortemente os candidatos: a maioria dos objetos está longe da célula consultada. Combinar o envelope reduz ainda mais os testes detalhados, mas acrescenta um teste simples por candidato. A tabela permite avaliar se essa troca compensa no cenário específico.

A grade 1 × 1 não faz seleção espacial dentro do cenário. Ela acrescenta a obtenção/cópia da lista de candidatos ao trabalho que a força bruta já fazia. O ponto externo ainda pode ser descartado pela grade; por isso a média de candidatos nessa configuração pode ficar abaixo de N, mesmo sem benefício para consultas internas.

Triângulos grandes cobrem muitas células e se sobrepõem. Assim, uma grade fina precisa armazenar mais referências e cada consulta conserva mais candidatos. O ganho é menor do que nos cenários pequenos. Com N reduzido, os custos fixos e a resolução temporal têm maior peso, e uma redução de testes não implica uma redução proporcional do tempo.

Todos os resultados acelerados foram iguais à lista de IDs da força bruta nas 21.600 medições. A comparação comprova equivalência para os casos executados; não substitui a análise do algoritmo nem constitui prova para todos os números representáveis.

## 7. Verificação

- Compilação dos módulos sem avisos com -Wall -Wextra -pedantic.
- 1.014.299 verificações em testes geométricos, de sementes e equivalência entre os modos.
- Casos explícitos: interior, exterior, três vértices, arestas, orientação invertida, tolerância, degenerados, sobreposições, cenário vazio e fronteiras de células.
- Mesma suíte executada com AddressSanitizer e UndefinedBehaviorSanitizer sem erros detectados.
- Callbacks testados sem janela: modos 1–4, F/S, exibições, mouse, redimensionamento, quantidade, grade e sementes.
- Argumentos inválidos são rejeitados com erro.

**Limitação de validação visual:** a inicialização GLUT falhou no ambiente de execução automatizado tanto para o programa novo quanto para o executável anterior, que o aluno já havia executado com sucesso no próprio terminal. Os testes dos callbacks não verificam a renderização real. A nova janela ainda deve ser conferida manualmente antes da entrega. Linux e Windows não foram testados.

## 8. Correspondência com o enunciado

| Requisito | Implementação/evidência |
|---|---|
| N triângulos com vértices globais | vector<Triangulo>, vertices em double |
| Geração com semente reutilizável | --seed, R, N, mt19937 |
| Controle do ponto e coordenadas | Clique, arrasto, setas, painel e console |
| Envelopes | AABB por triângulo, teste e exibição independentes |
| Subdivisão espacial | Grade uniforme com listas de índices |
| Quatro configurações | Teclas 1–4, F/S e ModoNumero |
| Resultado final detalhado | PontoNoTriangulo em todos os modos |
| Arestas, vértices, tolerância | Teste de orientação e distância a segmentos |
| Estruturas construídas com o cenário | RecriaCenario e GradeUniforme::constroi |
| Categorias visuais e célula | Cores e teclas E/G/C/A |
| Contadores por consulta | ResultadoConsulta reinicializado |
| Tempo de consulta | steady_clock sem desenho/console/referência |
| Comparação com força bruta | Igualdade de vetores de IDs em tempo de execução e experimentos |
| Experimentos variados e reproduzíveis | Experimentos.cpp, resultados.csv, resumo-resultados.csv |
| Organização | Geometria, consulta, experimentos e visualização em módulos |
| Vídeo e identificação dos autores | Pendentes; roteiro fornecido |

## 9. Reproduzir

```bash
make
make test
make test-interface
./BasicoOpenGL --n 1000 --seed 42 --size 1.5 --grid 16 --modo 4
./BasicoOpenGL --benchmark resultados.csv
python3 resumir_resultados.py resultados.csv
```

Os números poderão variar na nova execução. O CSV detalhado e seu resumo acompanham este relatório. Para entender e defender a implementação, consultar GUIA-ESTUDO.md. Inserir o link do vídeo e os autores antes de compactar a entrega final em ZIP.
