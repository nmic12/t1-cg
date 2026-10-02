# Como entender e apresentar o trabalho

## 1. Uma consulta, quatro caminhos

Leia `ConsultaPonto()` em Consulta.cpp. Ela recebe os mesmos triângulos, o mesmo ponto e duas opções booleanas. Sem filtros, visita todos os índices. Com grade, visita somente os índices da célula. Com envelopes, rejeita candidatos fora do AABB. O teste detalhado é sempre a decisão final.

Exercício: pressione V em um ponto. Explique por que os IDs encontrados precisam ser iguais, embora as quantidades de testes sejam diferentes.

## 2. Ponto em triângulo

`orientacao(A,B,P)` calcula `(Bx-Ax)*(Py-Ay) - (By-Ay)*(Px-Ax)`. O sinal informa de que lado da aresta orientada AB está P. Para um triângulo não degenerado, o interior fica do mesmo lado das três arestas: três sinais não negativos ou três sinais não positivos. Essa regra funciona para vértices em sentido horário ou anti-horário.

A orientação mede área orientada, não distância. Para a tolerância da borda usamos distância até segmentos. Isso evita comparar diretamente uma área com um epsilon em unidades de comprimento. Triângulos colineares/quase degenerados precisam de tratamento próprio: o teste de sinais isolado pode aceitar pontos distantes na reta.

Exercício: para A=(0,0), B=(4,0), C=(0,4), calcule os sinais para P=(1,1), Q=(3,3) e R=(2,2). R está na hipotenusa e deve ser aceito.

## 3. Envelope AABB

O envelope usa os menores e maiores X/Y dos três vértices. Estar fora dele elimina o triângulo. Estar dentro produz apenas um candidato.

A tolerância também expande o envelope: um ponto quase sobre a borda que o teste detalhado aceita não pode ser descartado antes. A grade é construída com esse envelope expandido.

Exercício: Q=(3,3) está no envelope [0,4] × [0,4], mas não no triângulo do exercício anterior. Isso demonstra por que o teste detalhado é indispensável.

## 4. Grade uniforme

Dividimos o domínio em linhas e colunas. Uma célula guarda um vetor de índices, por exemplo [2,7,9], e não cópias dos vértices. A construção inclui o triângulo em TODAS as células cobertas pelo envelope expandido. Guardá-lo apenas na célula do centro perderia consultas em outras partes do triângulo.

A conversão da posição para célula usa `floor((x-xmin)/largura * colunas)` e a expressão equivalente em Y. O limite máximo é ajustado para a última célula. Pontos fora do domínio não têm candidatos. A ordem crescente de inserção faz todos os modos retornarem IDs na mesma ordem.

Exercício: use grade 1 × 1 e compare com grade 16 × 16. Explique por que a primeira tende a acrescentar custo sem reduzir candidatos. Explique também por que muitas células e triângulos grandes aumentam o armazenamento.

## 5. Eventos e visualização

`main()` registra os callbacks. `Mouse()` e as setas mudam o ponto. `keyboard()` muda modos e parâmetros. `ConsultaAtual()` calcula e valida; `display()` apenas desenha o resultado guardado. As teclas de exibição só pedem redesenho.

OpenGL desenha no domínio lógico; o mouse chega em pixels. `MoveMouse()` desconta o viewport e inverte Y. `reshape()` calcula uma área com proporção 2:1 e reserva o painel. Não dependemos da matriz do OpenGL para converter o mouse, pois ela também é usada para desenhar o painel.

## 6. Experimento e tempo

`std::chrono::steady_clock` mede o trecho da consulta, incluindo criação dos vetores de resultado e candidatos. A construção da grade, comparação de referência, impressão e desenho ficam fora. O custo observado pelo usuário inclui também a validação automática; portanto o tempo do painel não é o tempo total do callback.

O benchmark fixa cenários e posições para os quatro modos, aquece cada caminho, repete cinco vezes e alterna a ordem. Médias pequenas são sensíveis ao relógio, cache e escalonamento: não conclua velocidade apenas pela redução de testes. O CSV mostra tanto candidatos/testes como tempo.

## Perguntas para praticar antes da apresentação

- Por que os envelopes nunca podem rejeitar um ponto que o teste detalhado aceita?
- Por que o programa não para ao encontrar o primeiro triângulo?
- Qual a diferença entre a tecla E e a tecla F?
- Como se garante que recriar uma semente produz o mesmo cenário?
- Por que os contadores reiniciam a cada consulta?
- Por que comparar somente a quantidade de resultados é insuficiente?
- O que acontece nos limites da grade e nos vértices dos triângulos?
- Quais custos não estão no tempo da consulta e por quê?
- Quando a grade pode custar mais que a força bruta?
