

#include "main.h"
#define ALOC_SHARED(pointer) {(pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define DEL_SHARED(pointer) {munmap((pointer), sizeof(*(pointer)));}
// stream https://www.youtube.com/watch?v=mXo8wSXUKvo&ab_channel=ToasterBro
// kniha https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf


//sem_t *semaphors[6];
FILE *file;
sem_t *mutex; // Hlavni proces
sem_t *customer; // Rada pro zakazniky
sem_t *customer_done; // Rada pro zakazniky
sem_t *output; // Kontroluje zapis do souboru
sem_t *official; // Semafor pro praci urednika
sem_t *official_done; // Semafor pro praci urednika
sem_t *service_1; // Rada pro sluzbu 4
sem_t *service_2; // Rada pro sluzbu 2
sem_t *service_3; // Rada pro sluzbu 3

sem_t *service[3];

 /*
 
 TODO: PREDELAT SERVICES DO POLE SEMAFORU!!!
 
 */   

int semaphore_init() {
/* Hint z knihy
1 n = 4
2 customers = 0
3 mutex = Semaphore (1)
4 customer = Semaphore (0)
5 barber = Semaphore (0)
6 customerDone = Semaphore (0)
7 barberDone = Semaphore (0)
n is the total number of customers that can be in the shop: three in the
waiting room and one in the chair.
customers counts the number of customers in the shop; it is protected by
mutex.
The barber waits on customer until a customer enters the shop, then the
customer waits on barber until the barber signals him to take a seat.
After the haircut, the customer signals customerDone and waits on
barberDone.
*/
//sem_t *xjerab28_semaphor_TODO;
    //mmap
    //inicializace semaforu
    // udelam jen jednou a bude pro všechny procesy
    // sdilena paměť - musim dat pozor, co přepisuju
    file = fopen("proj2.out", "w");
    setbuf(file,NULL);

    if(file == NULL) {
        fprintf(stderr, "Error opening proj2.out");
        return 1;
    }

    // inicializace mutexu na hodnotu 1

    customer = sem_open("/xjerab28_sem_customer", O_CREAT , 0666, 0);
    if (customer == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }
    official = sem_open("/xjerab28_sem_official", O_CREAT , 0666, 0);
    if (official == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }
    output = sem_open("/xjerab28_sem_output", O_CREAT , 0666, 1);
    if (output == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }

    /*
    service_1 = sem_open("/xjerab28_sem_service_1", O_CREAT , 0666, 0);
    if (output == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }
    service_2 = sem_open("/xjerab28_sem_service_2", O_CREAT , 0666, 0);
    if (output == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }
    service_3 = sem_open("/xjerab28_sem_service_3", O_CREAT , 0666, 0);
    if (output == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }
    */

    service[0] = sem_open("/xjerab28_sem_service_1", O_CREAT , 0666, 0);
    service[1] = sem_open("/xjerab28_sem_service_2", O_CREAT , 0666, 0);
    service[2] = sem_open("/xjerab28_sem_service_3", O_CREAT , 0666, 0);

    for(int i = 0; i < 3; i++) {
        if (service[i] == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
        }
    }

    if (output == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }
    official_done = sem_open("/xjerab28_sem_official_done", O_CREAT , 0666, 0);
    if (official_done == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }
    customer_done = sem_open("/xjerab28_sem_customer_done", O_CREAT , 0666, 0);
    if (customer_done == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvareni semaforuuu!");
        return 1;
    }
    // Inicializace dalsich semaforu
    /*
    for(int i = 1; i<SEM_COUNT; i++) {
        semaphors[i] = sem_open(semaphore_names[i], O_CREAT, 0666, 0);
        if (semaphors[i] == SEM_FAILED) {
            fprintf(stderr, "Chyba pri vytvareni semaforu!");
            return 1;
        }

    }
    */

    return 0;
}

void shared_memory(mem_t *memory) {
    ALOC_SHARED(memory->A_counter);
    ALOC_SHARED(memory->cust_id);
    ALOC_SHARED(memory->off_id);
    ALOC_SHARED(memory->customers);
    ALOC_SHARED(memory->is_opened);

    ALOC_SHARED(memory->service_1);
    ALOC_SHARED(memory->service_2);
    ALOC_SHARED(memory->service_3);

    *memory->A_counter = 0;
    *memory->cust_id = 0;
    *memory->off_id = 0;
    *memory->customers = 0;
    *memory->is_opened = 1;
    *memory->service_1 = 0;
    *memory->service_2 = 0;
    *memory->service_3 = 0;
    /* 
    *A_counter = 0;
    *c_id = 0;
    *cust_waitting = 0;
    *o_id = 0;
    */
    
}
void clean_sem() {

    sem_close(customer);
    sem_unlink("/xjerab28_sem_customer");
    sem_close(official);
    sem_unlink("/xjerab28_sem_official");
    sem_close(output);
    sem_unlink("/xjerab28_sem_output");
    /*
    sem_close(service_1);
    sem_unlink("/xjerab28_sem_service_1");
    sem_close(service_2);
    sem_unlink("/xjerab28_sem_service_2");
    sem_close(service_3);
    sem_unlink("/xjerab28_sem_service_3");
    */
    for(int i = 0; i < 3; i++) {
        sem_close(service[i]);
    }
    sem_unlink("/xjerab28_sem_service_1");
    sem_unlink("/xjerab28_sem_service_2");
    sem_unlink("/xjerab28_sem_service_3");

    sem_close(customer_done);
    sem_unlink("/xjerab28_sem_customer_done");
    sem_close(official_done);
    sem_unlink("/xjerab28_sem_official_done");
}
void clean(mem_t *memory) {

    fclose(file);

    DEL_SHARED(memory->A_counter);
    DEL_SHARED(memory->cust_id);
    DEL_SHARED(memory->off_id);
    DEL_SHARED(memory->customers);
    DEL_SHARED(memory->is_opened);
    DEL_SHARED(memory->service_1);
    DEL_SHARED(memory->service_2);
    DEL_SHARED(memory->service_3);

}

void my_print() {


    // zkopirovat funkci printf a pridat si fflush(file)
    // budu volat vždycky když budu chtit printovat
}

void customer_process(arg_t args, mem_t *memory) {
    // random sleep
    int customer_id;
    
    // Zakaznik vejde na postu a vypise vystup
    sem_wait(output);
    ++(*memory->cust_id);
    ++(*memory->A_counter);
    ++(*memory->customers);
    customer_id = *memory->cust_id;
    fprintf(file,"%d: Z %d: started\n", *memory->A_counter, customer_id);
    sem_post(output);
    // Zakaznik jde spinkat v i intervalu <0,TZ>
    //srand(time(NULL) + getpid());
    if(args.customer_wait != 0) {
        usleep(((rand() + getpid()) % args.customer_wait) * 1000);
    }
    // Pokud je posta zavrena, jde domu
    if (*memory->is_opened == 0) {
        sem_wait(output);
        sem_post(customer);
        sem_wait(official);
        ++(*memory->A_counter);
        --(*memory->customers);
        fprintf(file,"%d: Z %d: going home\n", *memory->A_counter, customer_id);
        sem_post(output);
        return;
    }


    // Zakaznik si nahodne vybere cinnost z intervalu <1,3>
    //int service = (rand() % 3) + 1;

    ///////////////////////
    int service = 1;
    //////////////////////////

    sem_wait(output);
    ++(*memory->A_counter);
    ++(*memory->service_1);
    
    // Vypise, jakou cinnost si vybral a zaradi se do fronty
    fprintf(file,"%d: Z %d: entering office for a service %d\n", *memory->A_counter, customer_id, service);
    sem_post(output);


    
        sem_post(customer);
        sem_wait(official);


    // Zaradi se do jedne ze tri front
    if(service == 1) {


        sem_wait(output);
        ++(*memory->A_counter);
        fprintf(file,"%d: Z %d: called by office worker\n", *memory->A_counter, customer_id);
        --(*memory->service_1);

        sem_post(output);
        
        sem_post(service_1);

        sem_post(customer_done);
        sem_wait(official_done);
        //fprintf(file, "Jsem zakaznik a chci domu\n");
    }
    else if(service == 2) {
        sem_post(service_2);
        sem_wait(output);
        ++(*memory->A_counter);
        ++(*memory->service_2);
        fprintf(file,"%d: Z %d: called by office worker\n", *memory->A_counter, customer_id);
        sem_post(output);
        sem_wait(service_2);
    }
    else if(service == 3) {
        sem_post(service_3);
        sem_wait(output);
        ++(*memory->A_counter);
        ++(*memory->service_3);
        fprintf(file,"%d: Z %d: called by office worker\n", *memory->A_counter, customer_id);
        sem_post(output);
        sem_wait(service_3);
    }
    // Potom spinka v intervalu <0,10>
    usleep((rand() % 11) * 1000);

    sem_wait(output);
    ++(*memory->A_counter);
    fprintf(file,"%d: Z %d: going home\n", *memory->A_counter, customer_id);
    sem_post(output);
    exit(0);
}


void official_process(arg_t args, mem_t *memory) {
    // Priradim ID a vypisu startovaci vystup
    int official_id;
    sem_wait(output);
    ++(*memory->off_id);
    ++(*memory->A_counter);
    official_id = *memory->off_id;
    fprintf(file,"%d: U %d: started\n", *memory->A_counter, official_id);
    sem_post(output);

    // Zacatek cyklu urednika
    while(1) {

        //srand(time(NULL) + getpid());
        //int service = (rand() % 3) + 1;
        sem_wait(output);
        if(*memory->customers > 0) {
            --(*memory->customers);
            sem_post(output);
            sem_wait(customer);
            sem_post(official);

        }
        
        else if (*memory->customers == 0 && *memory->is_opened == 1){
            //sem_post(output);
            
            //sem_wait(output);
            ++(*memory->A_counter);
            fprintf(file,"%d: U %d: taking break \n", *memory->A_counter, official_id);
            sem_post(output);

            // Bere prestavku v intervalu <0,TU>
            if(args.official_wait != 0) {
                usleep(((rand() + getpid()) % args.official_wait) * 1000);
            }

            sem_wait(output);
            ++(*memory->A_counter);
            fprintf(file,"%d: U %d: break finished \n", *memory->A_counter, official_id);
            sem_post(output);
            continue;
        }
        else {
        // V poste uz nejsou zakaznici a je zavreno
        //sem_wait(output);
        ++(*memory->A_counter);
        fprintf(file,"%d: U %d: going home\n", *memory->A_counter, official_id);
        sem_post(output);
        break;
        }
        
        int service = 1;
        // Jde obslouzit jednu ze tri front - neprazdnych
        // Sluzba 1
        if(service == 1 && *memory->service_1 > 0) {


            sem_wait(service_1);
            sem_wait(output);
            ++(*memory->A_counter);
            fprintf(file,"%d: U %d: serving a service of type %d\n", *memory->A_counter, official_id, service);

            sem_post(output);


            usleep((rand() % 11) * 1000);

            // Skonci s vyrizovanim
            sem_wait(output);
            ++(*memory->A_counter);
            fprintf(file,"%d: U %d: service finished\n", *memory->A_counter, official_id);
            sem_post(output);     

            sem_wait(customer_done);
            sem_post(official_done);
            //fprintf(file,"Tady je %d\n", official_id);
        }
        else if(service == 2 && *memory->service_2 > 0) {
            sem_wait(service_2);
            sem_wait(output);
            ++(*memory->A_counter);
            fprintf(file,"%d: U %d: serving a service of type %d\n", *memory->A_counter, official_id, service);
            --(*memory->service_2);
            sem_post(output);
            sem_post(service_2);

            usleep((rand() % 11) * 1000);

            // Skonci s vyrizovanim
            sem_wait(output);
            ++(*memory->A_counter);
            fprintf(file,"%d: U %d: service finished\n", *memory->A_counter, official_id);
            sem_post(output);     
        }
        else if(service == 3 && *memory->service_3 > 0) {
            sem_wait(service_3);
            sem_wait(output);
            ++(*memory->A_counter);
            fprintf(file,"%d: U %d: serving a service of type %d\n", *memory->A_counter, official_id, service);
            --(*memory->service_3);
            sem_post(output);
            sem_post(service_3);

            usleep((rand() % 11) * 1000);

            // Skonci s vyrizovanim
            sem_wait(output);
            ++(*memory->A_counter);
            fprintf(file,"%d: U %d: service finished\n", *memory->A_counter, official_id);
            sem_post(output);    

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

    if(NU < 0) {
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


void office_gen(arg_t args, mem_t *memory) {
    for(int i = 0; i < args.n_officials; i++) {
        pid_t office_pid = fork();
        if(office_pid == -1) {
            fprintf(stderr, "Chyba pri vytvareni ditete!");
            //return 1;
        }
        else if (office_pid == 0){
            official_process(args, memory);
            //fprintf(file, "Urednik\n");
            exit(0);
        }
    }
   //exit(0);
}

void cust_gen(arg_t args, mem_t *memory) {
    for(int i = 0; i < args.n_customers; i++) {    
        pid_t cust_pid = fork();
        if(cust_pid == -1) {
            fprintf(stderr, "Chyba pri vytvareni ditete!");
            //return 1;
        }
        else if (cust_pid == 0){
            //sem_wait(mutex);
            customer_process(args, memory);
            //fprintf(file, "Zakaznik\n");
            exit(0);

        }
    }
    //exit(0);
}

int main(int argc, char *argv[]) {
// Zpracuju argumenty
/*
 NZ: počet zákazníků
NU: počet úředníků
TZ: Maximální čas v milisekundách, po který zákazník po svém vytvoření čeká, než vejde na 
poštu (eventuálně odchází s nepořízenou). 0<=TZ<=10000
TU: Maximální délka přestávky úředníka v milisekundách. 0<=TU<=100
F: Maximální čas v milisekundách, po kterém je uzavřena pošta pro nově příchozí. 
0<=F<=10000
*/
arg_t args;
mem_t memory;
//pid_t main_pid;


memory.A_counter = NULL;
memory.cust_id = NULL;
memory.off_id = NULL;


if(check_args(argv, argc) == 1) {
    return 1;
}

args.n_customers = atoi(argv[1]);
args.n_officials = atoi(argv[2]);
args.customer_wait = atoi(argv[3]);
args.official_wait = atoi(argv[4]);
args.post_close = atoi(argv[5]);

    //clean(&memory);
    //clean_sem();



printf("n_customers = %d, n_officials = %d, customer_wait = %d, official_wait = %d, post_close = %d\n",
            args.n_customers, args.n_officials, args.customer_wait, args.official_wait, args.post_close);

semaphore_init(args);
shared_memory(&memory);

    //main_pid = fork();
        
    pid_t wpid;         // for main process to wait till child processes are dead
    int status = 0;
/*
    if(main_pid == -1) {
        fprintf(stderr, "Chyba pri vytvareni ditete!");
        return 1;
    }
    //else if(main_pid == 0) {
        pid_t CustOff = fork();
        if(CustOff == -1) {
            fprintf(stderr, "Chyba pri vytvareni ditete!");
            return 1;
        }
        
*/
        //else if(CustOff ==  0) {
            cust_gen(args, &memory);
        //}
        //else {
            office_gen(args, &memory);

        //}
    //}


    if(args.post_close > 1) {
        srand(time(NULL) + getpid());
        usleep(((rand() % (args.post_close/2)) + args.post_close/2) * 1000);
    }

    sem_wait(output);
    *memory.is_opened = 0;
    ++(*memory.A_counter);
    fprintf(file, "%d: closing\n", *memory.A_counter);
    sem_post(output);

    //while(wait(NULL));
    printf("Jsem tady\n");
    while ((wpid = wait(&status)) > 0);         // waiting for child processes to end
    clean(&memory);
    clean_sem();
    exit(0);
    return 0;

}