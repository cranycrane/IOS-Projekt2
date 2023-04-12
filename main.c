#include <stdio.h>
#include <semaphore.h>
// stream https://www.youtube.com/watch?v=mXo8wSXUKvo&ab_channel=ToasterBro
// kniha https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf

sem_t *mutex;
FILE *file;


void semaphore_init() {
    //mmap
    //inicializace semaforu
    // udelam jen jednou a bude pro všechny procesy
    // sdilena paměť - musim dat pozor, co přepisuju

}
void clean() {
    //sem_destroy(mutex);
    //munmap;
}
/*
Fork - kdyz ho zavolam, vytvori dalsi proces.
ten co zavolal fork je rodič a dítě je forknuté. Rozdeli praci.


Bude na konci:
while(wait(NULL) > 0) - pasivní čekání, uspí proces
while(*oxygen < 2) - aktivní čekání

cleanup()
*/

void my_print() {


    // zkopirovat funkci printf a pridat si fflush(file)
    // budu volat vždycky když budu chtit printovat
}

main(int argc, char *argv[]) {
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
if(argc != 6) {
    fprintf(stderr, "Chybny pocet argumentu");
    return -1;
}
if(argv[3] < 0 || argv[3] > 10000) {
    fprintf(stderr, "Nespravna hodnota tretiho argumentu");
    return -1;
}
if(argv[4] < 0 || argv[4] > 100) {
    fprintf(stderr, "Nespravna hodnota ctvrteho argumentu");
    return -1;
}
if(argv[5] < 0 || argv[5] > 10000) {
    fprintf(stderr, "Nespravna hodnota pateho argumentu");
    return -1;
}

int n_zakaznik = argv[1];
int n_urednik = argv[2];
int zakaznik_wait = argv[3];
int urednik_wait = argv[4];
int posta_close = argv[5];

// sem_wait() 
// fory pro zakaznika a urednika, asi
// tohle chci volat v kazdem zakaznikovi a urednikovi
// srand(time(NULL) * getpid()) 
// usleep(1000*(rand() % (TI+1)))

/*
pri debugovacich printech nezapominat, ze musim flushnout i tyto printy
jinak se nemusi zobrazit
*/

//setbuf() - nastavim buffer na 0, aby se zabranilo bufferovani
// druhy zpusob je dat po kazdem fprintf fflush(file) - 
}