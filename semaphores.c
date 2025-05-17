#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s [OPCION] [PARAMETRO]\n", argv[0]);
    fprintf(stderr, "Opciones:\n");
    fprintf(stderr, "\t-c semaforo valor\tcrea semáforo con el nombre y valor inicial indicado.\n");
    fprintf(stderr, "\t-u semaforo\t\trealiza un UP en el semáforo indicado.\n");
    fprintf(stderr, "\t-d semaforo\t\trealiza un DOWN en el semaforo indicado.\n");
    fprintf(stderr, "\t-b semaforo\t\telimina el semáforo indicado.\n");
    fprintf(stderr, "\t-i semaforo\t\tinformación del semáforo indicado.\n");
    fprintf(stderr, "\t-h\t\t\timprime este mensaje.\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-')
    {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    sem_t *semaforo;

    switch (option)
    {
    case 'c':
        if ((semaforo = sem_open(argv[2], O_CREAT, 0755, atoi(argv[3]))) == SEM_FAILED)
        {
            perror("Error al crear el semáforo");
            exit(EXIT_FAILURE);
        }
        printf("Se creó el semáforo %s\n", argv[2]);
        sem_close(semaforo);
        break;

    case 'u':
        if ((semaforo = sem_open(argv[2], 0)) == SEM_FAILED)
        {
            perror("Error al abrir el semáforo");
            exit(EXIT_FAILURE);
        }
        if (sem_post(semaforo) < 0)
        {
            perror("No se pudo hacer UP del semáforo");
        }
        else
        {
            printf("Se realizó UP del semáforo %s\n", argv[2]);
        }
        sem_close(semaforo);
        break;

    case 'd':
        if ((semaforo = sem_open(argv[2], 0)) == SEM_FAILED)
        {
            perror("Error al abrir el semáforo");
            exit(EXIT_FAILURE);
        }
        if (sem_wait(semaforo) < 0)
        {
            perror("No se pudo hacer DOWN del semáforo");
        }
        else
        {
            printf("Se realizó DOWN del semáforo %s\n", argv[2]);
        }
        sem_close(semaforo);
        break;

    case 'b':
        if ((semaforo = sem_open(argv[2], 0)) == SEM_FAILED)
        {
            perror("Error al abrir el semáforo");
            exit(EXIT_FAILURE);
        }
        if (sem_unlink(argv[2]) < 0)
        {
            perror("No se pudo borrar el semáforo");
            sem_close(semaforo);
            exit(EXIT_FAILURE);
        }
        printf("Se eliminó el semáforo %s\n", argv[2]);
        sem_close(semaforo);
        break;

    case 'i':
        if ((semaforo = sem_open(argv[2], 0)) == SEM_FAILED)
        {
            perror("Error al abrir el semáforo");
            exit(EXIT_FAILURE);
        }
        int val;
        if (sem_getvalue(semaforo, &val) < 0)
        {
            perror("No se pudo obtener el valor del semáforo");
        }
        else
        {
            printf("El contenido del semáforo %s es %d\n", argv[2], val);
        }
        sem_close(semaforo);
        break;

    case 'h':
        usage(argv);
        break;

    default:
        fprintf(stderr, "Opción desconocida: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
