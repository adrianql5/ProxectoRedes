# Informe Práctica3

Informe redactado por Manuel Pereiro Conde e Adrián Quiroga Linares

---

## Exercicio 1.c

Enviamos unha mensaxe dende emisor.c que ocupa máis da mensaxe que é capaz de recibir o emisor, recibimos a mexase cortada. Non é posible recuperar os datos perdidos porque cada mensaxe enviada con sentdto hay una única recepción con recvfrom.


## Exercicio 1.d

Enviamos un array de floats, enviando cada float por separado empregando a función sprintf para convertir de float a string, cando acaba de enviar a string enviamos un \n para detectar o final da string. No receptor simplemente recibimos as strings e imprimimos os números, os bytes enviados e o numero de números recibidos.

## Exercicio 3

Incluimos un sleep entre as mensaxes enviadas polo cliente para que dea tempo a conectar un segundo cliente. O servidor UDP pode atender a ambos simultáneamente, enviando as liñas a cada un dos clientes como se ve na captura. Isto é distinto ao que se producía cun servidor de conexión.