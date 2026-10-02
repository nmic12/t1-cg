#!/usr/bin/env python3
"""Resume o CSV do benchmark com biblioteca padrao; nao requer pandas."""
import csv
import statistics
import sys
from collections import defaultdict
from pathlib import Path

origem = Path(sys.argv[1] if len(sys.argv)>1 else 'resultados.csv')
grupos = defaultdict(list)
with origem.open(newline='') as f:
    for linha in csv.DictReader(f):
        if linha['validado'] != '1':
            raise RuntimeError('Consulta divergente no CSV')
        chave = (int(linha['n']), float(linha['tamanho']), int(linha['grade']), int(linha['modo']))
        grupos[chave].append(linha)
destino = origem.with_name('resumo-resultados.csv')
with destino.open('w',newline='') as f:
    w = csv.writer(f)
    w.writerow(['n','tamanho','grade','modo','amostras','tempo_medio_us','tempo_mediano_us','grade_media','envelope_medio','testes_medios','encontrados_medios'])
    for chave, linhas in sorted(grupos.items()):
        tempos = [float(r['tempo_us']) for r in linhas]
        def media(campo):
            valores = [float(r[campo]) for r in linhas if r[campo] != '']
            return statistics.mean(valores) if valores else ''
        w.writerow([*chave,len(linhas),statistics.mean(tempos),statistics.median(tempos),media('candidatos_grade'),media('aprovados_envelope'),media('testes_detalhados'),media('encontrados')])
print(f'{sum(map(len,grupos.values()))} consultas; resumo gravado em {destino}')
