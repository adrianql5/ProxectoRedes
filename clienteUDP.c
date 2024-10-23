#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "netinet/in.h"
#include "arpa/inet.h"
#include "sys/socket.h"
#include "sys/types.h"

int main(int argc, char **argv)
{
    // Inicialización variables
    char linea[1000];
    int sockserv = socket(AF_INET, SOCK_STREAM, 0);
    ssize_t bytes;

    if (argc < 3)
    {
        printf("Error en los argumentos.");
        exit(1);
    }
    // Argumento 3: nombre de archivo de lectura
    char *filename = argv[3];
    FILE *arquivo = fopen(filename, "r");
    if (arquivo == NULL)
    {
        printf("Error en el archivo.");
        exit(1);
    }

    // El nombre del archivo de salida es el de entrada pero en mayúsculas.
    char FILENAME[256];
    int i;
    for ( i = 0; filename[i] != '\0'; i++)
    {
        FILENAME[i] = toupper(filename[i]);

    }
    FILENAME[i+1]='\0';
    FILE *arquivo2 = fopen(FILENAME, "w");
    if (arquivo2 == NULL)
    {
        printf("Error creando el archivo de salida en mayúsculas.");
    }

    // Argumento 2: puerto
    uint16_t port = atoi(argv[2]);

    if (port < IPPORT_RESERVED)
    {
        printf("Error en el puerto.");
        exit(1);
    }

    // Argumento 1: IP
    struct sockaddr_in ipportserv;
    ipportserv.sin_family = AF_INET;
    ipportserv.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &ipportserv.sin_addr.s_addr);

    // Creaciíon de socket
    if (sockserv < 0)
    {
        printf("No se pudo crear el socket");
        exit(1);
    }
    printf("Socket creado.\n");

    // Conexión con servidor.
    if (connect(sockserv, (struct sockaddr *)&ipportserv, sizeof(struct sockaddr_in)) < 0)
    {
        printf("No se pudo conectar\n");
        exit(1);
    }
    printf("Conectado.");

    // Se lee todo el archivo y se envía línea por línea
    while (fgets(linea, sizeof(linea), arquivo) != NULL)
    {
        send(sockserv, linea, sizeof(linea), 0);
        // Se guarda el mensaje (linea) y el numero de bytes recibidos (bytes)
        bytes = recv(sockserv, linea, sizeof(linea), 0);
        if (bytes > 0)
        {
            printf("\n\nMensaje: %s  Bytes: %ld", linea, bytes);
            fprintf(arquivo2, "%s", linea);
        }
        else
            break;
    }
    printf("\n\nFin transmisión.\n");

    close(sockserv);
}
