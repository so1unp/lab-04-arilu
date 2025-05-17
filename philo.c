//
// El problema de la cena de filósofos.
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

// Mueve el cursor a la linea x, columna y (ver codigos ANSI).
#define xy(x, y) printf("\033[%d;%dH", x, y)

// Borra desde el cursor hasta el fin de la linea.
#define clear_eol(x) print(x, 17, "\033[K")

// Borra la pantalla.
#define clear() printf("\033[H\033[J")

#define N 5
const char *names[N] = {"Socrates", "Kant", "Spinoza", "Wittgenstein", "Popper"};

#define M 5
const char *topic[M] = {"Espagueti!", "Vida", "El Ser", "Netflix", "La verdad"};

// Tiempos
int segs_piensa = 1; // intervalo de pensamiento en [1, seg_piensa] segundos.
int segs_come = 1;

// Mutex
static pthread_mutex_t screen = PTHREAD_MUTEX_INITIALIZER;
/*La condición de carrera es que un filósofo quiera agarrar el mismo tenedor que otro ya tiene agarrado, para evitarla se debería usar un mutex_lock antes de agarrar el tenedor y un mutex_unlock para cuando termine de usarlo (deje de comer)*/

static pthread_mutex_t forks[N];
sem_t semaforos[N];

volatile int running = 1;

// Función para manejar señales
void handle_signal(int sig)
{
    if (sig == SIGTERM || sig == SIGINT)
    {
        running = 0; // Cambia el estado para detener el programa
    }
}

// Imprime en la posición (x,y) la cadena *fmt.
void print(int y, int x, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    pthread_mutex_lock(&screen);
    xy(y + 1, x), vprintf(fmt, ap);
    xy(N + 1, 1), fflush(stdout);
    pthread_mutex_unlock(&screen);
}

// El filosofo come.
void eat(int id)
{
    int f[2]; // tenedores
    int ration, i;

    // los tenedores a tomar
    f[0] = id;
    f[1] = (id + 1) % N;

    clear_eol(id);
    print(id, 18, "..oO (necesito tenedores)");
    sleep(2);

    // Toma los tenedores.
    for (i = 0; i < 2; i++)
    {
        if (!i)
        {
            clear_eol(id);
        }
        if (sem_wait(&semaforos[f[i]]) < 0)
        {
            printf("Error, no pudo agarrar el tenedor.");
        }

        pthread_mutex_lock(&forks[f[i]]);
        print(id, 18 + (f[i] != id) * 12, "tenedor%d", f[i]);

        // Espera para tomar el segundo tenedor.
        sleep(3);
    }

    /*
        // Toma los tenedores en orden diferente según el ID
    if (id % 2 == 0) {
        pthread_mutex_lock(&forks[f[1]]); // Tenedor derecho primero
        print(id, 30, "tenedor%d", f[1]);
        pthread_mutex_lock(&forks[f[0]]); // Tenedor izquierdo después
        print(id, 18, "tenedor%d", f[0]);
    } else {
        pthread_mutex_lock(&forks[f[0]]); // Tenedor izquierdo primero
        print(id, 18, "tenedor%d", f[0]);
        pthread_mutex_lock(&forks[f[1]]); // Tenedor derecho después
        print(id, 30, "tenedor%d", f[1]);
    }
         */

    // Come durante un tiempo.
    for (i = 0, ration = 3 + rand() % 8; i < ration; i++)
    {
        print(id, 40 + i * 4, "ñam");
        sleep(1 + (rand() % segs_come));
    }

    // Libera los tenedores
    for (i = 0; i < 2; i++)
    {
        pthread_mutex_unlock(&forks[f[i]]);
        if (sem_post(&semaforos[f[i]]) < 0)
        {
            printf("Error, no pudo soltar el tenedor.");
        }
    }
}

// El filosofo piensa.
void think(int id)
{
    int i, t;
    char buf[64] = {0};

    do
    {
        clear_eol(id);

        // Piensa en algo...
        sprintf(buf, "..oO (%s)", topic[t = rand() % M]);

        // Imprime lo que piensa.
        for (i = 0; buf[i]; i++)
        {
            print(id, i + 18, "%c", buf[i]);
            if (i < 5)
                sleep(1);
        }

        sleep(1 + rand() % segs_piensa);
    } while (t);
}

void *filosofo(void *p)
{
    int id = *(int *)p;
    print(id, 1, "%15s", names[id]);
    while (1)
    {
        think(id);
        eat(id);
    }
}

int main(int argc, char *argv[])
{
    int i;
    int id[N]; // id para cada filosofo.
    pthread_t tid[N];

    // Configura el manejador de señales usando signal
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s segs-piensa segs-come\n", argv[0]);
        fprintf(stderr, "\tsegs-piensa:\tmáxima cantidad de segundos que puede estar pensando.\n");
        fprintf(stderr, "\tsegs-come:\tmáxima cantidad de segundos que puede estar comiendo.\n");
        exit(EXIT_FAILURE);
    }

    if ((segs_piensa = atoi(argv[1])) <= 0)
    {
        fprintf(stderr, "segs-piensa debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    if ((segs_come = atoi(argv[2])) <= 0)
    {
        fprintf(stderr, "segs-come debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    srand(getpid());

    clear();

    for (i = 0; i < N; i++)
    {
        pthread_mutex_init(&forks[i], NULL);
        sem_init(&semaforos[i], 0, 1);
    }

    for (i = 0; i < N; i++)
    {
        id[i] = i;
        pthread_create(tid + i, 0, filosofo, id + i);
    }
    // Espera mientras el programa esté corriendo
    while (running)
    {
        sleep(1);
    }

    // Limpia los recursos antes de salir
    for (i = 0; i < N; i++)
    {
        pthread_mutex_destroy(&forks[i]);
        sem_destroy(&semaforos[i]);
    }

    printf("\nPrograma terminado correctamente.\n");
    pthread_exit(0);
    exit(0);
}
