/**
 * @file main.c
 * @brief Hlavni implementace projektu IOS FIT VUT
 * @author Jakub Jerabek (xjerab28)
 * @date 20.04.2023
*/

#include "main.h"


FILE *file;

sem_t *customer; // Rada pro zakazniky
sem_t *customer_done; // Rada pro zakazniky
sem_t *output; // Kontroluje zapis do souboru
sem_t *official; // Semafor pro praci urednika
sem_t *official_done; // Semafor pro praci urednika

sem_t *service[3];
mem_t *memory = NULL;



int semaphore_init() {

    file = fopen("proj2.out", "w");
    setbuf(file,NULL);

    if(file == NULL) {
        fprintf(stderr, "Error opening proj2.out");
        return 1;
    }

    // inicializace output semaforu na hodnotu 1

    output = sem_open("/xjerab28_sem_output", O_CREAT , 0666, 1);
    if (output == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforu!");
        return 1;
    }
    customer = sem_open("/xjerab28_sem_customer", O_CREAT , 0666, 0);
    if (customer == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforu!");
        return 1;
    }
    official = sem_open("/xjerab28_sem_official", O_CREAT , 0666, 0);
    if (official == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforu!");
        return 1;
    }

    service[0] = sem_open("/xjerab28_sem_service_1", O_CREAT , 0666, 0);
    service[1] = sem_open("/xjerab28_sem_service_2", O_CREAT , 0666, 0);
    service[2] = sem_open("/xjerab28_sem_service_3", O_CREAT , 0666, 0);

    for(int i = 0; i < 3; i++) {
        if (service[i] == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforu!");
        return 1;
        }
    }

    if (output == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforu!");
        return 1;
    }
    official_done = sem_open("/xjerab28_sem_official_done", O_CREAT , 0666, 0);
    if (official_done == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforu!");
        return 1;
    }
    customer_done = sem_open("/xjerab28_sem_customer_done", O_CREAT , 0666, 0);
    if (customer_done == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforu!");
        return 1;
    }


    return 0;
}

void shared_memory() {

    memory = mmap(NULL, sizeof(mem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED){
        fprintf(stderr, "Chyba alokovani pameti");
        return ;
    }
    // Inicializace jednotlivych hodnot
    memory->A_counter = 0;
    memory->cust_id = 0;
    memory->off_id = 0;
    memory->customers = 0;
    memory->is_opened = 1; // 1 = posta otevrena
    memory->service_q[0] = 0;
    memory->service_q[1] = 0;
    memory->service_q[2] = 0;  
    
}
void clean_sem() {

    sem_close(customer);
    sem_unlink("/xjerab28_sem_customer");
    sem_close(official);
    sem_unlink("/xjerab28_sem_official");
    sem_close(output);
    sem_unlink("/xjerab28_sem_output");

    sem_close(service[0]);
    sem_unlink("/xjerab28_sem_service_1");
    sem_close(service[1]);
    sem_unlink("/xjerab28_sem_service_2");
    sem_close(service[2]);
    sem_unlink("/xjerab28_sem_service_3");

    sem_close(customer_done);
    sem_unlink("/xjerab28_sem_customer_done");
    sem_close(official_done);
    sem_unlink("/xjerab28_sem_official_done");
}
void clean() {

    munmap(memory, sizeof(mem_t));
    
}



void customer_process(arg_t args) {
    int customer_id;
    
    // Zakaznik vejde na postu a priradi se mu ID
    sem_wait(output);
    ++memory->cust_id;
    ++memory->A_counter;
    ++memory->customers;
    customer_id = memory->cust_id;
    fprintf(file,"%d: Z %d: started\n", memory->A_counter, customer_id);
    sem_post(output);

    // Zakaznik jde spinkat v i intervalu <0,args.customer_wait>

    if(args.customer_wait != 0) {
        usleep(((rand() + getpid()) % args.customer_wait) * 1000);
    }
    else {
        usleep(0);
    }

    // Pokud je posta zavrena, jde domu
    if (memory->is_opened == 0) {
        sem_wait(output);
        ++memory->A_counter;
        --memory->customers;
        fprintf(file,"%d: Z %d: going homeeee\n", memory->A_counter, customer_id);
        sem_post(output);
        exit(0);
    }

    // Zakaznik si nahodne vybere cinnost z intervalu <0,2>
    int service_id = ((rand()+getpid()) % 3);
    // Prida +1 do promenne
    ++memory->service_q[service_id];


    // Vypise, jakou cinnost si vybral
    sem_wait(output);
    ++memory->A_counter;
    fprintf(file,"%d: Z %d: entering office for a service %d\n", memory->A_counter, customer_id, service_id+1);
    sem_post(output);
    // Ceka, nez si ho vezme urednik - zaradi se do fronty
    sem_wait(service[service_id]);
    sem_post(customer);
    sem_wait(official);
    
    // Je zavolan urednikem
    sem_wait(output);
    ++memory->A_counter;
    fprintf(file,"%d: Z %d: called by office worker\n", memory->A_counter, customer_id);
    sem_post(output);

    // Ohlasi, ze skoncil
    sem_post(customer_done);
    sem_wait(official_done);

 
    // Potom spinka v intervalu <0,10>
    usleep((rand() % 11) * 1000);

    // Jde domu a ukoncuje se
    sem_wait(output);
    ++memory->A_counter;
    fprintf(file,"%d: Z %d: going home\n", memory->A_counter, customer_id);
    sem_post(output);
    exit(0);
}


void official_process(arg_t args) {
    // Priradim ID a vypisu startovaci vystup
    int official_id;
    sem_wait(output);
    ++memory->off_id;
    ++memory->A_counter;
    official_id = memory->off_id;
    fprintf(file,"%d: U %d: started\n", memory->A_counter, official_id);
    sem_post(output);

    // Zacatek cyklu urednika
    while(1) {
        // Kontroluje, zda ve fronte cekaji zakaznici
        if(memory->service_q[0] > 0 || memory->service_q[1] > 0 || memory->service_q[2] > 0) {
            sem_wait(output);
            // Jde obslouzit jednu ze tri front - neprazdnych
            // Interval <0,2>
            int service_id;
            // Cyklus, dokud nenajde neprazdnou frontu
            do {
                    service_id = (rand() + getpid()) % 3;
                } while (memory->service_q[service_id] == 0);

            --memory->service_q[service_id];
            sem_post(output);

            // Obsluha sluzeb
            // Uvolnim cekajiciho zakaznika na sluzbe X
            sem_post(service[service_id]);
            // Cekam na zakaznika 
            sem_wait(customer);
            sem_post(official);
            // Obslouzim zakaznika
            sem_wait(output);
            ++memory->A_counter;
            fprintf(file,"%d: U %d: serving a service of type %d\n", memory->A_counter, official_id, service_id+1);
            sem_post(output);
            // Oznamim konec cinnosti a poslu zakaznika domu
            sem_wait(customer_done);
            sem_post(official_done);

            // Jde spinkat v intervalu <0,10>
            usleep((rand() % 11) * 1000);

            // Skonci s vyrizovanim
            sem_wait(output);
            ++memory->A_counter;
            fprintf(file,"%d: U %d: service finished\n", memory->A_counter, official_id);
            sem_post(output);     
        }
        // Pokud nejsou zakaznici ve fronte, bere si prestavku
        else if (memory->service_q[0] == 0 && memory->service_q[1] == 0 && memory->service_q[2] == 0 && memory->is_opened == 1){
            sem_wait(output);
            ++memory->A_counter;
            fprintf(file,"%d: U %d: taking break\n", memory->A_counter, official_id);
            sem_post(output);

            // Bere prestavku v intervalu <0,TU>
            srand(time(NULL) + getpid());
            if(args.official_wait != 0) {
                usleep(((rand() + getpid()) % (args.official_wait)) * 1000);
            }
            else {
                usleep(0);
            }
            // Skonci prestavku a pokracuje v cyklu
            sem_wait(output);
            ++memory->A_counter;
            fprintf(file,"%d: U %d: break finished \n", memory->A_counter, official_id);
            sem_post(output);
            //continue;
        }
        // Nebo konci = posta je zavrena a nejsou zakaznici
        else if (memory->is_opened == 0 && memory->service_q[0] == 0 && memory->service_q[1] == 0 && memory->service_q[2] == 0) {
            // Jde domu a ukoncuje cinnost
            sem_wait(output);
            ++memory->A_counter;
            fprintf(file,"%d: U %d: going home\n", memory->A_counter, official_id);
            sem_post(output);
            exit(0);
        
        }   
    }
}



int check_args(char *argv[], int argc) {

    if(argc != 6) {
        fprintf(stderr, "Chybny pocet argumentu");
        return 1;
    }


    int NZ = atoi(argv[1]);
    int NU = atoi(argv[2]);
    int TZ = atoi(argv[3]);
    int TU = atoi(argv[4]);
    int F = atoi(argv[5]);

    for (int i = 1; i < argc; i++) {
        for (size_t j = 0; j < strlen(argv[i]); j++) {
            if (!isdigit(argv[i][j])) {
                fprintf(stderr, "Chyba: Zadavejte pouze cisla");
                return 1;
            }
        }
    }

    if(NZ <= 0) {
        fprintf(stderr, "Nespravna hodnota prvniho argumentu\n");
        return 1;
    }

    if(NU < 1) {
        fprintf(stderr, "Nespravna hodnota druheho argumentu\n");
        return 1;
    }

    if(TZ < 0 || TZ > 10000) {
        fprintf(stderr, "Nespravna hodnota tretiho argumentu\n");
        return 1;
    }

    if(TU < 0 || TU > 100) {
        fprintf(stderr, "Nespravna hodnota ctvrteho argumentu\n");
        return 1;
    }

    if(F < 0 || F > 10000) {
        fprintf(stderr, "Nespravna hodnota pateho argumentu\n");
        return 1;
    }

    return 0;
}


void office_gen(arg_t args) {
    for(int i = 0; i < args.n_officials; i++) {
        pid_t office_pid = fork();
        if(office_pid == -1) {
            fprintf(stderr, "Chyba pri vytvareni ditete!");
            exit(1);
        }
        else if (office_pid == 0){
            official_process(args);
        }
    }
}

void cust_gen(arg_t args) {
    for(int i = 0; i < args.n_customers; i++) {    
        pid_t cust_pid = fork();
        if(cust_pid == -1) {
            fprintf(stderr, "Chyba pri vytvareni ditete!");
            exit(1);
        }
        else if (cust_pid == 0){
            customer_process(args);
        }
    }
}

int main(int argc, char *argv[]) {

arg_t args;

if(check_args(argv, argc) == 1) {
    exit(0);
    return 1;
}

args.n_customers = atoi(argv[1]);
args.n_officials = atoi(argv[2]);
args.customer_wait = atoi(argv[3]);
args.official_wait = atoi(argv[4]);
args.post_close = atoi(argv[5]);

if(semaphore_init(args) == 1){
    exit(1);
}
shared_memory();    
    //pid_t wpid;         // for main process to wait till child processes are dead
    //int status = 0;

 
    cust_gen(args);

    office_gen(args);

    if(args.post_close > 0) {
        srand(time(NULL) + getpid());
        usleep(((rand() % (args.post_close/2)) + args.post_close/2) * 1000);
    }
    else {
        usleep(0);
    }

    sem_wait(output);
    memory->is_opened = 0;
    ++memory->A_counter;
    fprintf(file, "%d: closing\n", memory->A_counter);
    sem_post(output);
    
    //Cekani, dokud neskonci vsechny deti - procesy

    while(wait(NULL) > 0);
 
    clean();
    clean_sem();
    fclose(file);
    exit(0);

}