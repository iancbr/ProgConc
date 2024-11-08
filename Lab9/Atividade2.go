// Ian de Andrade Camargo
// DRE_118089205
// Laboratório 9 - Atividade 2

/*
Guia de mensagens no channel:
Main envia para Goroutine: ’Ol ́a, Goroutine, bom dia!’ (impresso pela Goroutine)
Goroutine envia para Main: ’Oi Main, bom dia, tudo bem?’ (impresso pela Main)
Main envia para Goroutine: ’Tudo bem! Vou terminar t ́a?’ (impresso pela Goroutine)
Goroutine envia para Main: ’Certo, entendido.’ (impresso pela Main)
Goroutine imprime: finalizando
Main imprime: finalizando
*/

// introduz canais na aplicação Go
package main

import (
	"fmt"
)

func tarefa(str chan string) {
	//escreve no canal
	var msg = ""
	msg = <-str
	fmt.Println("[Goroutine]", msg)
	str <- "Oi Main, bom dia, tudo bem?"
	msg = <-str
	fmt.Println("[Goroutine]", msg)
	str <- "Certo, entendido."
	msg = <-str
	// goroutine finaliza
	fmt.Println("[Goroutine]", "Finalizando")
	str <- "finalizei"
}

func main() {
	var msg = "" //string vazia

	//cria um canal de comunicacao nao-bufferizado
	str := make(chan string)
	//cria um goroutine que executará a função 'tarefa'
	go tarefa(str)
	str <- "Olá, Goroutine, bom dia!"
	msg = <-str
	fmt.Println("[Main]", msg)
	str <- "Tudo bem! Vou terminar tá?"
	msg = <-str
	fmt.Println("[Main]", msg)

	str <- "pode termina!"
	msg = <-str
	fmt.Println("[Main]", "Finalizando")

}


