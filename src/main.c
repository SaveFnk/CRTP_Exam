#include "../headers/BufferData.h"
#include "../headers/Utils.h"

/* Helper function using during developement to know semaphores status */
void getValues(struct BufferData buf){
    int m, d, r, a;
    sem_getvalue(&buf.mutexSem, &m);            //only released by actor
    sem_getvalue(&buf.dataAvailableSem, &d);    //released by consumer after using items
    sem_getvalue(&buf.roomAvailableSem, &r);    //released after first items are created
    sem_getvalue(&buf.actorSem, &a);            //relased by producer or consumer

    printf("mutex: %d\ndata: %d\nroom: %d\nactor: %d\n\n", m, d, r, a);
}


int main(int argc, char *args[]) {

    // Allocazione di un array di caratteri di dimensione BUFFER_SIZE
    char* act;
    act = malloc(BUFFER_SIZE * sizeof(char));

    // Inizializzazione del buffer condiviso
    struct BufferData *sharedBuf;
    void *shmemblk;

    // Mappatura di una regione di memoria condivisa
    shmemblk = mmap(NULL, sizeof(sharedBuf), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sharedBuf = (struct BufferData*)shmemblk;

    // Verifica minima
    int i, nConsumers;
    pid_t pids[MAX_PROCESSES];

    if (argc != 2) {
        printf("Non può avere meno di 2 consumatori\n");
        exit(0);
    }

    // Estrai il numero di consumatori da input
    sscanf(args[1], "%d", &nConsumers);

    // Inizializzazione della coda dei messaggi
    int bufId;
    bufId = msgget(IPC_PRIVATE, 0666);

    // Gestione dell'errore nella creazione della coda di messaggi
    if (bufId == -1) {
        perror("msgget");
        exit(0);
    }

    // Inizializzazione dei semafori
    sem_init(&sharedBuf->mutexSem, 1, 1);
    sem_init(&sharedBuf->dataAvailableSem, 1, 0);
    sem_init(&sharedBuf->roomAvailableSem, 1, 1);
    sem_init(&sharedBuf->actorSem, 1, 0);

    /* Avvia il processo produttore */
    pids[0] = fork();
    if (pids[0] == 0) {
        /* Processo figlio */
        producer(sharedBuf, bufId);
        exit(0);
        printf("Eliminato produttore\n");
    }

    /* Avvia il processo attore */
    pids[1] = fork();
    if (pids[1] == 0) {
        /* Processo figlio */
        client(sharedBuf, bufId, nConsumers,act);
        exit(0);
        printf("Eliminato attore\n");
    }

    /* Avvia i processi consumatori */
    for (i = 0; i < nConsumers && i < MAX_PROCESSES - 1; i++) {
        pids[i + 2] = fork();
        if (pids[i + 2] == 0) {
            // Consumatore
            consumer(sharedBuf, i, bufId);
            exit(0);
            printf("Distrutto figlio %d\n", i);
        }
    }

    /* Aspetta la terminazione dei processi, generalmente non verrà raggiunto */
    for (i = 0; i <= nConsumers; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}