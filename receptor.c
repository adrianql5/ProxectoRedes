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

    char bufferMensaje[50];                        // Buffer para almacenar el mensaje recibido

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
Tu código para el emisor y el receptor de mensajes usando sockets UDP en C parece estar casi correcto y sigue la lógica requerida para enviar y recibir mensajes. Sin embargo, te haré algunas recomendaciones y observaciones para mejorar y asegurar que funcione como esperas:

    Receptor (receptor.c):
        En el receptor, al recibir el mensaje, podrías considerar también verificar el valor de retorno de recvfrom para asegurarte de que no ocurrieron errores. Esto ya lo haces, pero por claridad, es bueno comentar que recvfrom devuelve -1 en caso de error.
        También puedes agregar un chequeo adicional para verificar si el mensaje recibido es una cadena vacía, en cuyo caso podrías finalizar el do-while.

    Emisor (emisor.c):
        En el emisor, conviertes el primer argumento a portEm, que representa el puerto del emisor (es decir, el puerto desde el cual estás enviando el mensaje). Luego, lo configuras usando bind para que el socket esté asociado a ese puerto.
        Al final, en el mensaje que envías (sendto), usas el puerto y la IP proporcionados como argumentos.

    Sugerencias generales:
        Los mensajes de error podrían ser más específicos, ya que podrían ayudarte a depurar si algo no funciona. Por ejemplo, en vez de solo Error en el puerto 1, podrías poner Error: el puerto emisor es menor a IPPORT_RESERVED.
        Como ya configuraste que el receptor escucha en INADDR_ANY, este recibirá mensajes desde cualquier IP en todas las interfaces.

    Verificación de Cierre de la Conexión:
        Para indicar el cierre del emisor, en el receptor podrías verificar si bufferMensaje es "\n", tal como lo haces, para luego salir del bucle.

A continuación, algunas correcciones menores y mejoras estilísticas que podrías hacer:

Código para emisor.c:

c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
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

    ipportEm.sin_family = AF_INET;
    ipportEm.sin_port = htons(portEm);
    ipportEm.sin_addr.s_addr = htonl(INADDR_ANY);

    int sockserv = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockserv < 0)
    {
        perror("No se pudo crear el socket");
        exit(1);
    }

    if (bind(sockserv, (struct sockaddr *)&ipportEm, sizeof(struct sockaddr_in)) < 0)
    {
        perror("No se pudo asignar dirección");
        exit(1);
    }

    ipportRec.sin_family = AF_INET;
    ipportRec.sin_port = htons(portRec);
    inet_pton(AF_INET, argv[2], &ipportRec.sin_addr.s_addr);

    bytes = sendto(sockserv, mensaje, sizeof(mensaje), 0, (struct sockaddr *)&ipportRec, sizeof(ipportRec));
    if (bytes < 0)
    {
        perror("Error al enviar el mensaje");
    }
    else
    {
        printf("Bytes enviados: %zd\n", bytes);
    }

    close(sockserv);
    return 0;
}

Código para receptor.c:

c

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Indique un puerto de escucha como argumento\n");
        exit(EXIT_FAILURE);
    }

    int sockReceptor, bytesRecibidos, i = 0;
    uint16_t puertoHost = (uint16_t)atoi(argv[1]);
    uint16_t puertoRed = htons(puertoHost);

    struct sockaddr_in direccionReceptor, direccionEmisor;
    socklen_t tamanoEmisor = sizeof(direccionEmisor);
    char bufferMensaje[50];

    sockReceptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockReceptor < 0)
    {
        perror("No se pudo crear el socket receptor");
        exit(EXIT_FAILURE);
    }

    direccionReceptor.sin_family = AF_INET;
    direccionReceptor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionReceptor.sin_port = puertoRed;

    if (bind(sockReceptor, (struct sockaddr *)&direccionReceptor, sizeof(direccionReceptor)) < 0)
    {
        perror("No se pudo asignar dirección al socket");
        close(sockReceptor);
        exit(EXIT_FAILURE);
    }

    do
    {
        bytesRecibidos = recvfrom(sockReceptor, bufferMensaje, sizeof(bufferMensaje) - 1, 0, (struct sockaddr *)&direccionEmisor, &tamanoEmisor);
        if (bytesRecibidos < 0)
        {
            perror("Error al recibir el mensaje");
            close(sockReceptor);
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
