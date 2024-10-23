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
    // Verificación de argumentos
    if (argc < 4)
    {
        printf("Error en los argumentos. Uso: <IP servidor> <puerto servidor> <puerto cliente> <archivo>\n");
        exit(1);
    }

    // Inicialización de variables
    char linea[1000];
    ssize_t bytes;
    struct sockaddr_in ipportserv, ipportcli;
    socklen_t tam = sizeof(ipportserv);

    // Crear socket UDP
    int sockserv = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockserv < 0)
    {
        printf("No se pudo crear el socket UDP.\n");
        exit(1);
    }
    printf("Socket UDP creado.\n");

    // Argumento 1: IP del servidor
    ipportserv.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &ipportserv.sin_addr.s_addr);

    // Argumento 2: puerto del servidor
    uint16_t portserv = atoi(argv[2]);
    if (portserv < IPPORT_RESERVED)
    {
        printf("Error en el puerto del servidor.\n");
        exit(1);
    }
    ipportserv.sin_port = htons(portserv);  // Establece el puerto del servidor

    // Argumento 3: puerto del cliente
    uint16_t portcli = atoi(argv[3]);
    if (portcli < IPPORT_RESERVED)
    {
        printf("Error en el puerto del cliente.\n");
        exit(1);
    }

    // Configurar la dirección y puerto del cliente (emisor)
    ipportcli.sin_family = AF_INET;
    ipportcli.sin_addr.s_addr = htonl(INADDR_ANY); // Escucha en cualquier interfaz
    ipportcli.sin_port = htons(portcli);  // Asigna el puerto del cliente

    // Asignar el puerto del cliente al socket con bind()
    if (bind(sockserv, (struct sockaddr *)&ipportcli, sizeof(ipportcli)) < 0)
    {
        printf("Error al asignar el puerto del cliente.\n");
        exit(1);
    }
    printf("Cliente asociado al puerto %d.\n", portcli);

    // Argumento 4: nombre del archivo de lectura
    char *filename = argv[4];
    FILE *arquivo = fopen(filename, "r");
    if (arquivo == NULL)
    {
        printf("Error al abrir el archivo.\n");
        exit(1);
    }

    // Crear el nombre del archivo de salida en mayúsculas
    char FILENAME[256];
    int i;
    for (i = 0; filename[i] != '\0'; i++)
    {
        FILENAME[i] = toupper(filename[i]);
    }
    FILENAME[i] = '\0'; // Termina la cadena

    FILE *arquivo2 = fopen(FILENAME, "w");
    if (arquivo2 == NULL)
    {
        printf("Error creando el archivo de salida en mayúsculas.\n");
        exit(1);
    }

    // Leer el archivo y enviar cada línea por UDP
    while (fgets(linea, sizeof(linea), arquivo) != NULL)
    {
        // Enviar la línea al servidor usando UDP
        sendto(sockserv, linea, sizeof(linea), 0, (struct sockaddr *)&ipportserv, tam);
        
        // Recibir la respuesta del servidor
        bytes = recvfrom(sockserv, linea, sizeof(linea), 0, (struct sockaddr *)&ipportserv, &tam);
        if (bytes > 0)
        {
            linea[bytes] = '\0';  // Asegurarse de que la cadena termine
            printf("\n\nMensaje recibido: %s  Bytes: %zd", linea, bytes);
            fprintf(arquivo2, "%s", linea);  // Escribir la respuesta en el archivo de salida
        }
        else
        {
            printf("No se recibieron más datos.\n");
            break;
        }
    }

    printf("\n\nFin de la transmisión.\n");

    // Cerrar el socket y archivos
    close(sockserv);
    fclose(arquivo);
    fclose(arquivo2);

    return 0;
}
