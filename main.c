

#include "local.h"

// functions prototypes
void steps(struct TechnicalEmp *);
void shuffle(int *, int);
void check_laptop_count();
void load();
void load_in_trucks();
void Ceo();
void HR();
// mutexes and conditions
pthread_mutex_t count_mutex[10];
pthread_cond_t Laptop_Cond = PTHREAD_COND_INITIALIZER; // To move the box (full of laptops) to the storage room when ready
pthread_mutex_t Laptop_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Storage_Cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t Ceo_Cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t Storage_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Load_Cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t SUS_Cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t Truck_Cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t Truck_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Truck_Av_Cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t Ceo_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SUS_lock = PTHREAD_MUTEX_INITIALIZER;
// global variables used in the code
int unlock_flag = 0;
int SFull_Flag = 0;
int indx[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ordIndex[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int order[5] = {6, 7, 8, 9, 10};
int numOfBox = 0;
int numOfLaptops = 0;
int storageCapacity = 0;
int min_S_cap = 0;
int max_S_cap = 0;
int abs_S_max = 0;
int abs_S_min = 0;
int loaded_Boxes = 0;
int num_loading_emp = 0;
int truck_cap = 0;
int truck_id = 0;
int abs_T_max = 0;
int abs_T_min = 0;
int taken_boxes = 0;
int av_truck = 0;
int num_truck_drivers = 0;
int total_expense = 0;
int gain = 0;
int sus_flag = 0;
int laptop_price = 0;
int laptop_cost = 0;
int gainThreashhold = 0;
int tech_salary = 0;
int sus_line_cnt = 0;
int target_gain = 0;
int terminate_flag = 0;

//main
int main(void)
{
    pthread_t technical_emp_threads[10][10];
    pthread_t storage_emp_thread;
    pthread_t Ceo_thread;
    pthread_t HR_thread;

    struct TechnicalEmp attrb_Tech[10];
    FILE *fptr_input;

    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_init(&count_mutex[i], NULL); //define each mutex to "PTHREAD_MUTEX_INITIALIZER"
    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<read the input file>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    char inp_file_values[44][40];

    // open the file if exists
    if ((fptr_input = fopen(INPUT_FILE, "r")) == NULL)
    {
        perror("INPUT_FILE");
        exit(-1);
    }

    // read the file
    for (int i = 0; i < 44; i++)
    {
        fscanf(fptr_input, "%s\n", inp_file_values[i]);
        // printf("inp_file_values[%d] =  %s\n", i, inp_file_values[i]);
    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Creating Technical Employee Threads>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    printf("\n                        %s%sWELCOME TO OUR LAPTOP-MANUFACTURING FACTORY%s\n\n", WHTHB, BHBLK, ORIGINAL);
    printf("\n*********************************** Technical Employees are Manufacturing the Laptops ***********************************\n");
    shuffle(order, 5);
    max_S_cap = atoi(inp_file_values[9]);
    attrb_Tech->min_cap = atoi(inp_file_values[11]);
    min_S_cap = atoi(inp_file_values[11]);
    tech_salary = atoi(inp_file_values[27]);
    target_gain = atoi(inp_file_values[43]);

    for (int i = 0; i < 10; i++) // lines
    {
        attrb_Tech[i].min_step = atoi(inp_file_values[1]);
        attrb_Tech[i].max_step = atoi(inp_file_values[3]);
        attrb_Tech[i].num_steps = 0;
        attrb_Tech[i].line = i;

        for (int j = 0; j < 10; j++) // each line has 10 technical_emp
        {
            attrb_Tech[i].thread_ID[j] = j;
            if (pthread_create(&technical_emp_threads[i][j], NULL, (void *)steps, &attrb_Tech[i]) != 0)
            {
                perror("Failed to create thread");
                return 1;
            }
        }
    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Creating Storage Employee Thread >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    abs_S_max = atoi(inp_file_values[5]);
    abs_S_min = atoi(inp_file_values[7]);

    if (pthread_create(&storage_emp_thread, NULL, (void *)check_laptop_count, NULL) != 0)
    {
        perror("Failed to create Storage Emp thread");
        return 2;
    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Creating Loading Employee Threads >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    num_loading_emp = atoi(inp_file_values[15]);
    pthread_t loading_emp_threads[num_loading_emp];

    for (int j = 0; j < num_loading_emp; j++)
    {
        if (pthread_create(&loading_emp_threads[j], NULL, (void *)load, NULL) != 0)
        {
            perror("Failed to create thread");
            return 3;
        }
    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Creating Truck Threads >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    truck_cap = atoi(inp_file_values[17]);
    abs_T_min = atoi(inp_file_values[19]);
    abs_T_max = atoi(inp_file_values[21]);
    num_truck_drivers = atoi(inp_file_values[13]);
    av_truck = num_truck_drivers;
    pthread_t truck_driver_threads[num_truck_drivers];

    for (int t = 0; t < num_truck_drivers; t++)
    {
        if (pthread_create(&truck_driver_threads[t], NULL, (void *)load_in_trucks, NULL) != 0)
        {
            perror("Failed to create thread");
            return 4;
        }
    }
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Creating CEO Thread >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // techincal + storage + loading truck drivers salaries
    total_expense = 100 * atoi(inp_file_values[27]) + atoi(inp_file_values[29]) + num_loading_emp * atoi(inp_file_values[31]) + num_truck_drivers * atoi(inp_file_values[33]) + atoi(inp_file_values[23]) + atoi(inp_file_values[25]);
    laptop_cost = atoi(inp_file_values[37]);
    laptop_price = atoi(inp_file_values[39]);
    gainThreashhold = atoi(inp_file_values[41]);
    printf("\n%s%s**************************************** Total Expense The factory has is %d **********************************************%s\n\n", REDHB, BHWHT, total_expense, ORIGINAL);
    if (pthread_create(&Ceo_thread, NULL, (void *)Ceo, NULL) != 0)
    {
        perror("Failed to create Storage Emp thread");
        return 2;
    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Creating HR Thread >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if (pthread_create(&HR_thread, NULL, (void *)HR, NULL) != 0)
    {
        perror("Failed to create Storage Emp thread");
        return 2;
    }
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Termination >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    while (1)
    {
        if (sus_line_cnt > 5 || terminate_flag == 1)
        {
            for (int i = 0; i < 10; i++)
            {
                pthread_mutex_destroy(&count_mutex[i]);
                for (int j = 0; j < 10; j++)
                {

                    if (pthread_cancel(technical_emp_threads[i][j]) != 0)
                    {
                        perror("Failed to cancel technical employee thread\n");
                    }
                }
            }

            for (int i = 0; i < num_loading_emp; i++)
            {
                if (pthread_cancel(loading_emp_threads[i]) != 0)
                {
                    perror("Failed to cancel loading employee thread\n");
                }
            }
            for (int i = 0; i < num_truck_drivers; i++)
            {
                if (pthread_cancel(truck_driver_threads[i]) != 0)
                {
                    perror("Failed to cancel truck driver thread\n");
                }
            }

            if (pthread_cancel(HR_thread) != 0)
            {
                perror("Failed to cancel HR thread\n");
            }

            if (pthread_cancel(Ceo_thread) != 0)
            {
                perror("Failed to cancel CEO thread\n");
            }

            if (pthread_cancel(storage_emp_thread) != 0)
            {
                perror("Failed to cancel Storage Employee thread\n");
            }
            //destroy the mutexes
            pthread_mutex_destroy(&SUS_lock);
            pthread_mutex_destroy(&Storage_Mutex);
            pthread_mutex_destroy(&Truck_Mutex);
            pthread_mutex_destroy(&Ceo_Mutex);

            printf("\n%sXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX THE PROGRAM HAS BEEN TERMINATED XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n", BHRED);
            if (sus_line_cnt > 5)
            {
                printf("                 Since More Than 50%c Of The employees have been Suspended!!\n\n", 37);
            }
            else
            {
                printf("                      Since the Target Profit has Met which equals %d\n\n", target_gain);
            }
            printf("%s***********************************************************************************\n", BHCYN);
            printf("***********************************************************************************\n");
            printf("Done By: 1180414 Yazan Daibes \n         1180424 Omar Qattosh\n         1181557 Kareen Karam\n");
            printf("***********************************************************************************\n");
            printf("***********************************************************************************\n");

            return 8;
        }
    }

    return 0;
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< FUNCTIONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// The TECHNICAL Employee Function
void steps(struct TechnicalEmp *attrib_tech)
{
    while (1)
    {
        // sleep(1);
        if (pthread_mutex_lock(&count_mutex[attrib_tech->line]) == 0)
        {
            // steps 1-5 are in order
            if (attrib_tech->num_steps < 5)
            {
                attrib_tech->num_steps = (attrib_tech->num_steps) + 1;
                printf("%sTechnical employee%s with %sID = %d%s has %sstarted%s working on %sstep #%d%s (Line = %d) \n", YELLOW_COLOR, ORIGINAL, BLUE_COLOR, attrib_tech->thread_ID[indx[attrib_tech->line]] + 1, ORIGINAL, GREEN_COLOR, ORIGINAL, BLUE_COLOR, attrib_tech->num_steps, ORIGINAL, attrib_tech->line);
                // rand%(max - min) + min
                sleep((int)(rand() % (attrib_tech->max_step - attrib_tech->min_step + 1) + attrib_tech->min_step));
                printf("%sTechnical employee%s with %sID = %d%s has %sfinished%s working on %sstep #%d%s (Line = %d) \n", YELLOW_COLOR, ORIGINAL, BLUE_COLOR, attrib_tech->thread_ID[indx[attrib_tech->line]] + 1, ORIGINAL, RED_COLOR, ORIGINAL, BLUE_COLOR, attrib_tech->num_steps, ORIGINAL, attrib_tech->line);
                indx[attrib_tech->line]++;
            }
            // steps 6-10 are random
            else if (attrib_tech->num_steps >= 5)
            {
                attrib_tech->num_steps = (attrib_tech->num_steps) + 1;
                printf("%sTechnical employee%s with %sID = %d%s has %sstarted%s working on %sstep #%d%s (Line = %d) \n", YELLOW_COLOR, ORIGINAL, CYAN_COLOR, attrib_tech->thread_ID[indx[attrib_tech->line]] + 1, ORIGINAL, GREEN_COLOR, ORIGINAL, CYAN_COLOR, order[ordIndex[attrib_tech->line]], ORIGINAL, attrib_tech->line);

                // rand%(max - min) + min
                sleep((int)(rand() % (attrib_tech->max_step - attrib_tech->min_step + 1) + attrib_tech->min_step));
                printf("%sTechnical employee%s with %sID = %d%s has %sfinished%s working on %sstep #%d%s (Line = %d) \n", YELLOW_COLOR, ORIGINAL, CYAN_COLOR, attrib_tech->thread_ID[indx[attrib_tech->line]] + 1, ORIGINAL, RED_COLOR, ORIGINAL, CYAN_COLOR, order[ordIndex[attrib_tech->line]], ORIGINAL, attrib_tech->line);
                // the order index is an index used to get the shuffled steps
                ordIndex[attrib_tech->line]++;
                indx[attrib_tech->line]++;
            }

            // number of steps = 10 then 1 laptop is produced
            if (attrib_tech->num_steps == 10)
            {
                numOfLaptops++;

                printf("\n   %s%sNumber of laptops in carton box %s%d%s is %s%d%s\n\n", BLKB, BHWHT, BHRED, numOfBox + 1, BHWHT, BHRED, numOfLaptops, ORIGINAL);
                attrib_tech->num_steps = 0;
                indx[attrib_tech->line] = 0;
                ordIndex[attrib_tech->line] = 0;
                shuffle(order, 5); // shuffle the steps 6-10 in order not to do them in order
            }
            // number of lpatops = 10 then put them in a carton box
            if (numOfLaptops == 10) //the box has filled with 10 laptops
            {
                pthread_cond_signal(&Laptop_Cond);
            }

            if (storageCapacity >= max_S_cap)
            {
                // variables used in load employee function
                SFull_Flag = 1;
                unlock_flag = 1;
                // let the technical employees wait when storage area is max (full)
                pthread_cond_wait(&Storage_Cond, &count_mutex[attrib_tech->line]);
            }
            // if suspend flag = 1 then suspend a line
            if (sus_flag == 1)
            {
                pthread_mutex_lock(&SUS_lock);
                if (sus_flag == 1)
                {
                    printf("\n                         %s%sLINE %d has been suspended.....%s\n", WHTHB, BHRED, attrib_tech->line, ORIGINAL);
                    sus_flag = 0;
                    sus_line_cnt++;
                    printf("                          %s%sSuspended Lines for NOW =  %d%s \n\n", WHTHB, BHRED, sus_line_cnt, ORIGINAL);
                    pthread_cond_wait(&SUS_Cond, &SUS_lock);
                    printf("\n              %s%s Employees in Line %s%d%s is up and running again %s\n\n", WHTHB, BHBLK, BHRED, attrib_tech->line, BHBLK, ORIGINAL);
                    sus_line_cnt--;
                }
                pthread_mutex_unlock(&SUS_lock);
            }

            // if lock is not released by the cond_wait
            if (unlock_flag == 0)
            {
                pthread_mutex_unlock(&count_mutex[attrib_tech->line]);
            }
        }
        // the technical employee couldn't take the mutex
        else
        {
            printf("Thread ID = %d didn't lock\n", attrib_tech->thread_ID[indx[attrib_tech->line]] + 1);
            indx[attrib_tech->line]++;
        }
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    }
}

// The STORAGE Employee Function
void check_laptop_count()
{
    while (1)
    {
        if (pthread_mutex_lock(&Laptop_Mutex) == 0)
        {
            // wait until the box has 10 laptops
            if (numOfLaptops < 10)
                pthread_cond_wait(&Laptop_Cond, &Laptop_Mutex);

            if (numOfLaptops == 10)
            {
                gain = gain + (numOfLaptops * laptop_price - numOfLaptops * laptop_cost);
                numOfLaptops = 0;
            }

            // add boxes to storage area when it's not full yet
            if (storageCapacity < max_S_cap)
            {
                storageCapacity = storageCapacity + 1;
                numOfBox++;
                printf("\n\t%s%s Storage Employee is going to place the filled box %s%d%s in the storage room %s\n", WHTHB, BHBLK, BHRED, numOfBox, BHBLK, ORIGINAL);
                printf("\t\t\t\t%s%s Current Storage Capcity = %d %s\n\n", WHTHB, BHRED, storageCapacity, ORIGINAL);
                // storage employee becomes absent for user defined time
                sleep((int)(abs_S_min + rand() % (abs_S_max - abs_S_min + 1)));

                printf("\n%s------------------------------------%sThe Storage Employee Has Returned%s-------------------------------%s\n\n", BHWHT, BHRED, BHWHT, ORIGINAL);
            }

            // storage area full
            if (storageCapacity == max_S_cap)
            {
                printf("\n******* The storage is %sFULL%s and the storage capacity is %s%d%s \n", BHRED, ORIGINAL, BHRED, storageCapacity, ORIGINAL);
                printf("********* %sTechnical Employees%s will %sSTOP%s manufacturing Laptop \n", YELLOW_COLOR, ORIGINAL, BHRED, ORIGINAL);
                printf("********* %sLoading Employees%s will %sSTART%s loading the Boxes into Trucks with Capacity %s%d%s\n", CYAN_COLOR, ORIGINAL, BHRED, ORIGINAL, BHRED, truck_cap, ORIGINAL);
                pthread_cond_broadcast(&Load_Cond);

                // back to sleep
                pthread_cond_wait(&Laptop_Cond, &Laptop_Mutex);
            }

            pthread_mutex_unlock(&Laptop_Mutex);
        }
        else
        {
            printf("STORAGE EMP: CAN'T LOCK \n");
        }
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    }
}

// The LOAD Employee Function
void load()
{
    while (1)
    {
        if (pthread_mutex_lock(&Storage_Mutex) == 0)
        {
            if (storageCapacity <= min_S_cap && SFull_Flag == 0)
                pthread_cond_wait(&Load_Cond, &Storage_Mutex);

            if (storageCapacity > 0)
            {
                if (av_truck == 0) //check if truck is available (unavailable then wait)
                    pthread_cond_wait(&Truck_Av_Cond, &Storage_Mutex);
                    
                loaded_Boxes++;
                storageCapacity--;
                taken_boxes++;
                printf("\n\t\t\t%s%sBox %s%d%s is being loaded to the Truck with current ID =%d %s \n", WHTHB, BHMAG, BHRED, taken_boxes, BHMAG, truck_id + 1, ORIGINAL);
                printf("\t\t\t\t%s%sThe Storage Capcity is = %d%s\n\n", WHTHB, BHRED, storageCapacity, ORIGINAL);

                if (loaded_Boxes == truck_cap) // load on truck
                {
                    loaded_Boxes = 0;
                    av_truck = av_truck - 1;
                    truck_id++;
                    if (truck_id <= num_truck_drivers)
                    {
                        truck_id = truck_id % num_truck_drivers;
                    }
                    pthread_cond_signal(&Truck_Cond);
                }

                if (storageCapacity < min_S_cap && SFull_Flag == 1) //check if min threshhold is met
                {
                    printf("\n %sStorage Capacity %d < the minimum Capacity %d \n\t\t\t %s==> Technical Employees are going back to work%s\n\n", RED_COLOR, storageCapacity, min_S_cap, BHRED, ORIGINAL);
                    unlock_flag = 0;
                    SFull_Flag = 0;
                    pthread_cond_broadcast(&Storage_Cond);
                    pthread_cond_wait(&Load_Cond, &Storage_Mutex);
                }
            }
            pthread_mutex_unlock(&Storage_Mutex);
        }
        else
        {
            printf("LOAD EMPLOYEE: CAN'T LOCK \n");
        }
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    }
}

// The TRUCK Function
void load_in_trucks()
{
    while (1)
    {
        if (pthread_mutex_lock(&Truck_Mutex) == 0)
        {
            pthread_cond_wait(&Truck_Cond, &Truck_Mutex);
            // printf("--------------------------- available trucks =  %d----------------\n", av_truck);
            printf("\t %sTruck with license plate %ld has left (current ID = %d) %s\n\n", BHBLU, pthread_self(),truck_id, ORIGINAL);
            pthread_mutex_unlock(&Truck_Mutex);
            sleep((int)(abs_T_min + rand() % (abs_T_max - abs_T_min + 1)));
            av_truck++;
            printf("\n\t %s|||||| Truck with license plate %ld has returned (current ID = %d) |||||| %s\n\n", BHBLU, pthread_self(),truck_id, ORIGINAL);
            pthread_cond_broadcast(&Truck_Av_Cond);
        }
        else
        {
            printf("TRUCK: CAN'T LOCK \n");
        }
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    }
}

/////// HR FUNCIOTN //////////
void HR()
{
    while (1)
    {
        sleep(60);
        int profit = gain - total_expense;
        printf("\n%sAs 10 Laptops have produced, %s\n", BHWHT, ORIGINAL);
        printf("%s%s**************************************** The Gain is %d **********************************************%s\n", REDHB, BHWHT, gain, ORIGINAL);
        printf("%s%s**************************************** The profit will be %d ***************************************%s\n\n", REDHB, BHWHT, profit, ORIGINAL);
        if (profit < gainThreashhold)
        {
            sus_flag = 1;
        }
        if (profit > target_gain)
        {
            terminate_flag = 1;
        }
        gain = 0;
        pthread_cond_signal(&Ceo_Cond);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    }
}

//////////CEO FUNCTION//////////////
void Ceo()
{
    while (1)
    {
        pthread_mutex_lock(&Ceo_Mutex);
        pthread_cond_wait(&Ceo_Cond, &Ceo_Mutex);
        // suspend a line (decrementing the total expense)
        if (sus_flag == 1)
        {
            total_expense = total_expense - 10 * tech_salary;
        }
        // unsuspend a line (incrementing the total expense)
        else if (sus_flag == 0 && terminate_flag == 0)
        {

            pthread_cond_signal(&SUS_Cond);
            total_expense = total_expense + 10 * tech_salary;
        }

        pthread_mutex_unlock(&Ceo_Mutex);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    }
}

// The SHUFFLE Function
void shuffle(int *array, int n)
{
    if (n > 1)
    {
        int i;
        for (i = 0; i < n - 1; i++)
        {
            srand(time(0));
            int j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}
