/**
 * @file main.h
 * @author Jakub Jerabek (xjerab28)
 * 
 * @brief Hlavickovy soubor pro main.c
 */


#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>





typedef struct {
    int n_customers;    // pocet zakazniku
    int n_officials;    // pocet uredniku > 0
    int customer_wait;  // cekani zakaznika 0<=TZ<=10000 v milisekundach
    int official_wait;  // prestavka urednika 0<=TU<=100 v milisekundach
    int post_close;     // doba,po ktere se posta uzavre 0<=F<=1000 v milisekundach
}arg_t;

typedef struct Memory {
    int A_counter; //pořadové číslo
    int cust_id; //ID zakaznika
    int off_id; //ID urednika
    int customers; // pocet "zijicich" zakazniku
    int is_opened; // 0 - zavreno, 1 - otevreno
    int service_q[3]; // pocet zakazniku cekajicich na service
}mem_t;

/**
 * @brief Inicializuje vsechny potrebne semafory
 * 
 * @return Vrati 1, pokud se nepodarilo semafory inicializovat, jinak nic.
 * Funkce vypočítá faktoriál pro zadané celé číslo. Pokud je číslo menší nebo rovno 0, vrátí 1.
 */
int semaphore_init();

/**
 * @brief Alokuje sdilenou pamet pro procesy
 * 
 * Alokuje deklarovanou strukturu mem_t a tu alokuje pomoci mmap
 */
void shared_memory();

/**
 * @brief Vycisti a uvolni vytvorene semafory
 */
void clean_sem();

/**
 * @brief Uvolni alokovanou strukturu mem_t
 */
void clean();

/**
 * @brief Proces zakaznika
 */
void customer_process(arg_t args);

/**
 * @brief Proces urednika
*/
void official_process(arg_t args);

/**
 * @brief Kontrola argumentu
 * 
 * @param argv Pole argumentu
 * @param argc Pocet argumentu
 * @return 0 pokud argumenty odpovidaji specifikaci, jinak 1
 */
int check_args(char **argv, int argc);

/**
 * @brief Vygeneruje procesy urednika
 * @param args struktura obsahujici zadane argumenty
 */
void office_gen(arg_t args);

/**
 * @brief Vygeneruje procesy zakaznika
 * @param args struktura obsahujici zadane argumenty
 */
void cust_gen(arg_t args);