#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "netinet/in.h"
#include "arpa/inet.h"
#include "sys/socket.h"
#include "sys/types.h"

int main(int argc, char **argv)
{
    // inicialización variables
    struct sockaddr_in ipportEm, ipportRec;
    socklen_t tam = sizeof(struct sockaddr_in);
    char mensaje[50] = "Saludo! Hola bo día que tal.";
    ssize_t bytes;

    if (argc < 4)
    {
        printf("Error en los argumentos. Se debe indicar la IP, puerto emisor y puerto receptor\n");
        exit(1);
    }
    uint16_t portEm = atoi(argv[1]);

    if (portEm < IPPORT_RESERVED)
    {
        printf("Error en el puerto 1.\n");
        exit(1);
    }

    uint16_t portRec = atoi(argv[3]);

    if (portRec < IPPORT_RESERVED)
    {
        printf("Error en el puerto 2.\n");
        exit(1);
    }
    // Creación socket (ip: argumento 1)
    ipportEm.sin_family = AF_INET;
    ipportEm.sin_port = htons(portEm);
    ipportEm.sin_addr.s_addr = htonl(INADDR_ANY);

    // Inicialización socket
    int sockserv = socket(AF_INET, SOCK_DGRAM, 0);
    ipportRec.sin_family = AF_INET;
    ipportRec.sin_port = htons(portRec);
    inet_pton(AF_INET, argv[2], &ipportRec.sin_addr.s_addr);

    if (sockserv < 0)
    {
        printf("No se pudo crear el socket.\n");
        exit(1);
    }

    // Asignación de dirección con bind
    if (bind(sockserv, (struct sockaddr *)&ipportEm, sizeof(struct sockaddr_in)) < 0)
    {
        printf("No se pudo asignar dirección\n");
        exit(1);
    }

    bytes = sendto(sockserv, mensaje, sizeof(mensaje), 0, (struct sockaddr *)&ipportRec, sizeof(ipportRec));
    if (bytes < 0)
    {
        perror("Error al enviar el mensaje");
    }

    printf("Bytes enviados: %zd\n", bytes);

    close(sockserv);
}
