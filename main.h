//====================//
//
// Purpose:     Header file for main.c
//
// $NoKeywords: $ios_project2
// $Author:     JAKUB JERABEK <xjerab28@stud.fit.vutbr.cz>
// $Date:       $2023-04-13
//============================================================================//
/**
 * @file main.h
 * @author JAKUB JERABEK
 * 
 * @brief Hlavickovy soubor pro main.c
 */
// Standard libs
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
// Process sync libs
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
#define SEM_COUNT 6
enum semaphores
{
        mutex,
        mutex2,
        finish,
        oxyQueue,
        hydroQueue,
        obarrier,
};
const char *semaphore_names[] = {
    "/xjerab28_sem_mutexxx",
    "/xjerab28_sem_mutex2",
    "finish",
    "oxyQueue",
    "hydroQueue",
    "obarrier",
};
*/
#define MEM_COUNT 6


typedef struct {
    int n_customers;
    int n_officials;
    int customer_wait;
    int official_wait;
    int post_close;
}arg_t;

typedef struct Memory {
    int *A_counter; //pořadové číslo
    int *cust_id; //číslo skřítka
    int *off_id; //číslo soba
    int *customers; // pocet zakazniku v poste
    int *is_opened;
    int *service_1;
    int *service_2;
    int *service_3;
}mem_t;

int semaphore_init();

void clean(mem_t *memory);

void my_print();

void customer_process(arg_t args, mem_t *memory);

void official_process(arg_t args, mem_t *memory);

int check_args(char **argv, int argc);