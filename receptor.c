#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Indique un puerto de escucha como argumento\n");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    int sockReceptor, bytesRecibidos;
    uint16_t puertoHost = (uint16_t)atoi(argv[1]); // Número de puerto en formato de host
    uint16_t puertoRed = htons(puertoHost);        // Convertir a formato de red

    struct sockaddr_in direccionReceptor; // Estructura de la dirección del receptor
    struct sockaddr_in direccionEmisor;   // Estructura de la dirección del emisor
    socklen_t tamanoEmisor = sizeof(direccionEmisor);

    char bufferMensaje[10];                        // Buffer para almacenar el mensaje recibido

    // Crear el socket UDP
    sockReceptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockReceptor < 0)
    {
        perror("No se pudo crear el socket receptor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del receptor
    direccionReceptor.sin_family = AF_INET;                // IPv4
    direccionReceptor.sin_addr.s_addr = htonl(INADDR_ANY); // Escuchar en todas las interfaces
    direccionReceptor.sin_port = puertoRed;                // Número de puerto

    // Vincular el socket a la dirección del receptor
    if (bind(sockReceptor, (struct sockaddr *)&direccionReceptor, sizeof(direccionReceptor)) < 0)
    {
        perror("No se pudo asignar dirección al socket");
        exit(EXIT_FAILURE);
    }

    do
    {
        // Recibir mensaje
        bytesRecibidos = recvfrom(sockReceptor, bufferMensaje, sizeof(bufferMensaje) - 1, 0, (struct sockaddr *)&direccionEmisor, &tamanoEmisor);
        if (bytesRecibidos < 0)
        {
            perror("Error al recibir el mensaje");
            exit(EXIT_FAILURE);
        }

        bufferMensaje[bytesRecibidos] = '\0'; // Asegurar que el mensaje sea un string válido

        // Imprimir la IP, puerto y el mensaje recibido
        if (strcmp(bufferMensaje, "\n"))
        {
            printf("Mensaje recibido de %s:%d\n", inet_ntoa(direccionEmisor.sin_addr), ntohs(direccionEmisor.sin_port));
            printf("Contenido: %s (%d bytes)\n", bufferMensaje, bytesRecibidos);
            i++;
        }
    } while (strcmp(bufferMensaje, "\n"));
    printf("Números recibidos: %d.\n", i);
    close(sockReceptor);
    return 0;
}
