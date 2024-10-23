#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // para perror
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h> // para toupper()

/**
 * Convertir una cadena de caracteres minúsculos a mayúsculos
 * @param cadena cadena a convertir
 */
void toMayusculas(char cadena[])
{
    int i = 0;
    while (cadena[i] != '\0')
    {
        cadena[i] = toupper(cadena[i]); // Convierte cada carácter a mayúsculas
        i++;
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Indique un puerto de escucha como argumento\n");
        exit(EXIT_FAILURE);
    }

    int sockReceptor, bytesRecibidos, bytesEnviados;
    uint16_t puerto = (uint16_t)atoi(argv[1]); // Número de puerto con formato de host
    uint16_t puertoRed = htons(puerto);        // Convertir a formato de red

    struct sockaddr_in direcReceptor, direcEmisor;
    socklen_t tamanoEmisor = sizeof(direcEmisor);

    char mensajeRecibido[1000];

    // Crear el socket UDP
    sockReceptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockReceptor < 0)
    {
        perror("No se pudo crear el servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del receptor
    direcReceptor.sin_family = AF_INET;                // IPv4
    direcReceptor.sin_addr.s_addr = htonl(INADDR_ANY); // Escuchar en todas las interfaces
    direcReceptor.sin_port = puertoRed;                // El número de puerto

    // Vincular el socket al puerto
    if (bind(sockReceptor, (struct sockaddr *)&direcReceptor, sizeof(direcReceptor)) < 0)
    {
        perror("No se pudo asignar dirección al socket");
        exit(EXIT_FAILURE);
    }

    printf("Servidor UDP escuchando por el puerto %d...\n", puerto);

    while (1)
    {
        // Recibir mensaje del cliente
        bytesRecibidos = recvfrom(sockReceptor, mensajeRecibido, sizeof(mensajeRecibido) - 1, 0, (struct sockaddr *)&direcEmisor, &tamanoEmisor);
        if (bytesRecibidos < 0)
        {
            perror("Error al recibir el mensaje");
            exit(EXIT_FAILURE);
        }

        mensajeRecibido[bytesRecibidos] = '\0'; // Asegurarse de que sea una cadena válida

        // Convertir el mensaje a mayúsculas
        toMayusculas(mensajeRecibido);

        // Imprimir la IP y el puerto del cliente
        char ipCliente[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(direcEmisor.sin_addr), ipCliente, INET_ADDRSTRLEN);
        printf("Mensaje recibido de %s:%d\n", ipCliente, ntohs(direcEmisor.sin_port));
        printf("Contenido convertido: %s\n", mensajeRecibido);

        // Enviar el mensaje convertido de vuelta al cliente
        bytesEnviados = sendto(sockReceptor, mensajeRecibido, bytesRecibidos, 0, (struct sockaddr *)&direcEmisor, tamanoEmisor);
        if (bytesEnviados < 0)
        {
            perror("Error al enviar el mensaje");
            exit(EXIT_FAILURE);
        }
    }

    close(sockReceptor);
    return 0;
}

