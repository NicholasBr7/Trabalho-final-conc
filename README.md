# Trabalho-final-conc
Repositório para o trabalho final de programação concorrente 2024.2. Algoritmo de Eliminação Gaussiana implementado de forma concorrente.

## Forma de execução: 

### Gerar Matriz

Compila-se o arquivo geraMatriz.c para criar uma matriz com n linhas e (n+1) colunas, por se tratar de uma matriz aumentada.
Com isso, gera-se um arquivo de saída chamado "matriz.txt" com inteiros entre -100 e 100.

Entrada: ./geraMatriz nLinhas
Saída: arquivo txt "matriz.txt"

### Executar Eliminação Gaussiana Sequencial

Compila-se o arquivo elimSeq.c para realizar a eliminação gaussiana da matriz aumentada fornecida.
Com isso, gera-se um arquivo de saída com a matriz simplificada e um conjunto solução de valores.

Entrada: ./seq matriz.txt saidaSeq.txt
Saída: arquivo txt "saidaSeq.txt"
Console: tempos de inicialização, processamento e finalização

### Executar Eliminação Gaussiana Concorrente

Compila-se o arquivo elimConc.c para realizar a eliminação gaussiana concorrente usando threads.
Com isso, gera-se um arquivo de saída com a matriz simplificada e um conjunto solução de valores.

Entrada: ./seq matriz.txt saidaConc.txt nThreads
Saída: arquivo txt "saidaConc.txt"
Console: tempos de inicialização, processamento e finalização

### Executar Código de Corretude

Passo adicional para verificação das contas caso desejado:

Compila-se o arquivo corretude.c para verificar se o conjunto de solução encontrado está, de fato, correto.
Realiza o processo de checagem com margem de 0,1.
Com isso, caso o conjunto solução esteja correto, não aparecem avisos, caso contrário, aparece mensagem de erro no console.

Entrada: ./corretude matriz.txt saidaConc.txt nThreads
Saída: 
Console: "Erro.", caso haja algum erro na conta.
