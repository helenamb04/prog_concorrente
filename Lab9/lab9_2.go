package main

import (
	"fmt"
	"time"
)

func dialogo(str chan string) {
	fmt.Println(<-str)                    // recebe mensagem da main
	str <- "Oi Main, bom dia! Tudo bem?"  // responde
	fmt.Println(<-str)                    // recebe outra mensagem
	str <- "Certo, entendido."            // confirma
	fmt.Println("Goroutine: finalizando") // goroutine finaliza
}

func main() {
	str := make(chan string)

	go dialogo(str)                       // inicia a goroutine

	str <- "Olá, Goroutine, bom dia!"     // envia mensagem inicial à goroutine
	fmt.Println(<-str)                    // recebe resposta
	str <- "Tudo bem! Vou terminar tá?"   // envia outra mensagem
	fmt.Println(<-str)                    // recebe confirmação final

	time.Sleep(time.Second/10)            //bloqueio na main
	fmt.Println("Main: finalizando")      // main finaliza
}
