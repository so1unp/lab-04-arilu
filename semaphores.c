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
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    sem_t *semaforo;

    switch(option) {
        case 'c':
            if((semaforo = sem_open(argv[2], O_CREAT, 0755, argv[3])) == (sem_t *)-1){
                printf("Hubo un error");
                exit(0);
            }
            printf("Se creó el semáforo %s", argv[2]);

            break;
        case 'u':
            if(sem_post(semaforo) < 0){
                printf("No se pudo hacer up del semaforo %s", argv[2]);
            }else{

            }

            break;
        case 'd':
            break;
        case 'b':
            if( (semaforo = sem_open(argv[2], O_WRONLY)) == SEM_FAILED){
                printf("No se pudo encontrar el semáforo.");
                exit(0);   
            }
            if(sem_unlink(argv[2]) < 0){
                printf("No se pudo borrar el semáforo.");
                exit(0);
            }
            printf("Se eliminó el semáforo %s", argv[2]);
            
            break;
        case 'i':
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
