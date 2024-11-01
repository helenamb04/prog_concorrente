package main

import (
	"fmt"
	"math"
)

// função para verificar se um número é primo
func ehPrimo(n int) bool {
	if n <= 1 {
		return false
	}
	if n == 2 {
		return true
	}
	if n%2 == 0 {
		return false
	}
	for i := 3; i <= int(math.Sqrt(float64(n))); i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}

func main() {
	N := 50  // intervalo de números a verificar (1 a N)
	M := 4   // número de goroutines

	numChan := make(chan int)    // canal para enviar números
	countChan := make(chan int)  // canal para receber contagem parcial de primos

	// cria M goroutines para contar primos
	for i := 0; i < M; i++ {
		go func() {
			count := 0
			for num := range numChan {
				if ehPrimo(num) {
					count++
				}
			}
			countChan <- count // envia a contagem parcial para countChan
		}()
	}
	
	for i := 1; i <= N; i++ {
		numChan <- i
	}
	close(numChan) // fecha o canal após enviar todos os números


	// aguarda todas as contagens parciais e calcula o total
	totalPrimos := 0
	for i := 0; i < M; i++ {
		totalPrimos += <-countChan
	}

	fmt.Printf("Total de números primos entre 1 e %d: %d\n", N, totalPrimos)
}
