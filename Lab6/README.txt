Coloquei um exemplo de saída para mostrar que a lógica está sendo respeitada:

Thread 1: Terminei de ler
Thread 1: Terminei de ler
Thread 1: Terminei de ler
Thread 5: Terminei de ler
Thread 5: Terminei de ler
Thread 5: Terminei de ler
Thread 4: Quero escrever mas preciso esperar 1 leitor(es) // escritor entra na fila
Thread 0: Terminei de ler                                 // o único leitor terminou
Thread 0: Quero ler mas tem 1 escritor(es) na frente      // escritor começa a escrever e já trava o primeiro leitor
Thread 3: Quero ler mas tem 1 escritor(es) na frente
Thread 2: Quero ler mas tem 1 escritor(es) na frente
Thread 7: Terminei de escrever
Thread 7: Terminei de escrever
Thread 0: Quero ler mas tem 1 escritor(es) na frente
Thread 8: Quero ler mas tem 1 escritor(es) na frente
Thread 3: Quero ler mas tem 1 escritor(es) na frente
Thread 6: Quero ler mas tem 1 escritor(es) na frente
Thread 7: Quero escrever mas já estão escrevendo          // dois não podem escrever ao mesmo tempo
Thread 10: Quero ler mas tem 2 escritor(es) na frente
Thread 4: Terminei de escrever
Thread 4: Quero escrever mas já estão escrevendo
Thread 9: Quero ler mas tem 2 escritor(es) na frente
Thread 8: Quero ler mas tem 2 escritor(es) na frente
Thread 6: Quero ler mas tem 2 escritor(es) na frente
Thread 12: Quero ler mas tem 2 escritor(es) na frente
Thread 3: Quero ler mas tem 2 escritor(es) na frente
Thread 2: Quero ler mas tem 2 escritor(es) na frente
Thread 0: Quero ler mas tem 2 escritor(es) na frente
Thread 11: Quero ler mas tem 2 escritor(es) na frente
Thread 10: Quero ler mas tem 2 escritor(es) na frente
Thread 7: Terminei de escrever
Thread 13: Quero ler mas tem 1 escritor(es) na frente
Thread 4: Terminei de escrever
Thread 4: Terminei de escrever                            // todos terminaram de escrever
Thread 3: Terminei de ler                                 // leitores já podem voltar a ler
//várias threads leem sem mais nenhuma escrita...
Thread 2: Terminei de ler
write=6 read=54
Tempo: 0.001108
