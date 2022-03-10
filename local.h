#ifndef _LOCAL_H_
#define _LOCAL_H_

// 1180414 Yazan Daibes
// 1181557 Kareen Karam
// 1180424 Omar Qattosh

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h> // for EBSY
#include <unistd.h>

#define ORIGINAL "\x1B[0m"
#define RED_COLOR "\x1B[31m"
#define GREEN_COLOR "\x1B[32m"
#define YELLOW_COLOR "\x1B[33m"
#define BLUE_COLOR "\x1B[34m"
#define PURPLE_COLOR "\x1B[35m"
#define CYAN_COLOR "\x1B[36m"
#define WHITE_COLOR "\x1B[37m"

//High intensty background
#define REDHB "\e[0;101m" 
#define WHTHB "\e[0;107m"
#define WHTB "\e[47m"
#define BLKB "\e[40m"

//Bold High intensty 
#define BHBLK "\e[1;90m"
#define BHRED "\e[1;91m"
#define BHGRN "\e[1;92m"
#define BHYEL "\e[1;93m"
#define BHBLU "\e[1;94m"
#define BHMAG "\e[1;95m"
#define BHCYN "\e[1;96m"
#define BHWHT "\e[1;97m"

#define INPUT_FILE "./input.txt"

struct TechnicalEmp
{
    int min_step;
    int max_step;
    int num_steps;
    int thread_ID[10];
    int line;
    int max_cap;
    int min_cap; //minimum capacity for storage room
};

#endif