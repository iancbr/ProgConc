// Ian de Andrade Camargo
// DRE_118089205
// Laboratório 9 - Atividade 5

package main

import (
	"fmt"
	"math"
)

// Função para verificar se um número é primo.
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
	limite := int(math.Sqrt(float64(n))) + 1
	for i := 3; i < limite; i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}

// Função que recebe números de um canal, verifica a primalidade e envia resultados para outro canal.
func tarefa(numeros <-chan int, resultados chan<- int, done chan<- bool) {
	for num := range numeros {
		if ehPrimo(num) {
			resultados <- num
		}
	}
	done <- true // Indica que a goroutine terminou
}

func main() {
	N := 10 // Defina o valor de N aqui
	M := 5  // Número de goroutines

	// Canais para comunicação
	numeros := make(chan int, N)
	resultados := make(chan int, N)
	done := make(chan bool, M)

	// Iniciar goroutines
	for i := 0; i < M; i++ {
		go tarefa(numeros, resultados, done)
	}

	// Enviar números para o canal 'numeros'
	go func() {
		for i := 1; i <= N; i++ {
			numeros <- i
		}
		close(numeros)
	}()

	// Fechar o canal 'resultados' quando todas as goroutines terminarem
	go func() {
		for i := 0; i < M; i++ {
			<-done // Espera cada goroutine sinalizar que terminou
		}
		close(resultados)
	}()

	// Receber e contar os números primos
	var primos []int
	for primo := range resultados {
		primos = append(primos, primo)
	}

	// Exibir os resultados
	fmt.Printf("Total de números primos entre 1 e %d: %d\n", N, len(primos))
  
	//Caso queira verificar os números primos, descomente a linha abaixo:
	//fmt.Printf("Números primos entre 1 e %d: %v\n", N, primos)

}
