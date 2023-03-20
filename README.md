# Matrícula

Sistema de Matrícula Automático para seleção de disciplinas com maior aproveitamento.

Dado um conjunto de turmas $C$, pretendemos encontrar o melhor conjunto de turmas $T \subseteq C$ que maximize $|T|$, considerando os conflitos
- **Código:** Seja $u,v \in C$, se $u.código$ é igual à $v.código$, então existe um conflito entre $u$ e $v$.
- **Horário:** Seja $u,v \in C$, se $u.dia$ é igual à $v.dia$ e 
$$u.início < v.início \text{ }  AND \text{ } u.fim > v.inicio$$
$$OR$$
$$u.início < v.fim \text{ }  AND \text{ } u.fim > v.fim$$
$$OR$$
$$u.início > v.início \text{ }  AND \text{ } u.fim < v.fim,$$
então existe um conflito entre $u$ e $v$.

## Entrada
A entrada é realizada através do arquivo `disciplinas.txt`, onde é necessário inserir o conjunto de turmas $C = (Disciplina,Turma,Nome,Dia,Início,Fim)$.
*Observação:* É imprescindível a conversão do horário para minutos e acrescimo de disciplinas que possuem aula em mais de um dia da semana, por exemplo:

```
Disciplina: GCC218
Turma: 10A
Nome: Algoritmos em Grafos
Dia: Quinta-Feira e Sexta-Feira
Início: 17h e 15h
Fim: 18h40 e 16h40
```

Conversão:
```
GCC218,10A,Algoritmos em Grafos,Quinta-Feira,1020,1120
GCC218,10A,Algoritmos em Grafos,Sexta-Feira,900,1000
```