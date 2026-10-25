#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include <sched.h>

#define TEST_A     8
#define RESPONSE_A 9
#define TEST_B     7
#define RESPONSE_B 15
#define TEST_C     16
#define RESPONSE_C 1

#define NUM_DIST_THREADS 10

// Argument structure shared across all thread tasks
typedef struct {
    int test_pin;
    int response_pin;
} responseTaskArgs;

// Initialization function provided for the BRTT connection
void io_init(void) {
    wiringPiSetup();
    
    pinMode(TEST_A, INPUT);  pinMode(RESPONSE_A, OUTPUT); 
    pinMode(TEST_B, INPUT);  pinMode(RESPONSE_B, OUTPUT); 
    pinMode(TEST_C, INPUT);  pinMode(RESPONSE_C, OUTPUT); 
    
    digitalWrite(RESPONSE_A, HIGH);
    digitalWrite(RESPONSE_B, HIGH);
    digitalWrite(RESPONSE_C, HIGH);
}
void sleep_realtime(long nanoseconds)
{
    struct timespec ts;
    ts.tv_sec = nanoseconds / 1e9;
    ts.tv_nsec = nanoseconds % (long)1e9;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
}

int set_cpu(int cpu_id)
{
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_id < 0 || cpu_id >= num_cores)
        return EINVAL;

    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_id, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

// Reusable continuously running thread function for Tasks A, B, and C
void* TaskA(void* arg) {
    set_cpu(1);
    responseTaskArgs* args = (responseTaskArgs*)arg;
    
    while (1) {
        // Busy-wait until the test pin goes LOW (signal from BRTT)
        while (digitalRead(args->test_pin) == HIGH) {
            // Busy waiting for test signal
        }
        
        // Respond by pulling the response pin LOW
        digitalWrite(args->response_pin, LOW);
        
        // Wait for the test pin to return HIGH (end of the test signal pulse)
        while (digitalRead(args->test_pin) == LOW) {
            // Busy waiting for test signal release
        }
        
        // Return response pin to HIGH
        digitalWrite(args->response_pin, HIGH);
    }
    
    return NULL;
}
// Reusable continuously running thread function for Tasks A, B, and C
void* TaskB(void* arg) {
    set_cpu(1);
    responseTaskArgs* args = (responseTaskArgs*)arg;
    
    while (1) {
        // Busy-wait until the test pin goes LOW (signal from BRTT)
        while (digitalRead(args->test_pin) == HIGH) {
            // Busy waiting for test signal
        }
        
        // Respond by pulling the response pin LOW
        digitalWrite(args->response_pin, LOW);
        
        // Wait for the test pin to return HIGH (end of the test signal pulse)
        while (digitalRead(args->test_pin) == LOW) {
            // Busy waiting for test signal release
        }
        
        // Return response pin to HIGH
        digitalWrite(args->response_pin, HIGH);
    }
    
    return NULL;
}
// Reusable continuously running thread function for Tasks A, B, and C
void* TaskC(void* arg) {
    set_cpu(1);
    responseTaskArgs* args = (responseTaskArgs*)arg;
    
    while (1) {
        // Busy-wait until the test pin goes LOW (signal from BRTT)
        while (digitalRead(args->test_pin) == HIGH) {
            // Busy waiting for test signal
        }
        
        // Respond by pulling the response pin LOW
        digitalWrite(args->response_pin, LOW);
        
        // Wait for the test pin to return HIGH (end of the test signal pulse)
        while (digitalRead(args->test_pin) == LOW) {
            // Busy waiting for test signal release
        }
        
        // Return response pin to HIGH
        digitalWrite(args->response_pin, HIGH);
    }
    
    return NULL;
}
void* disturbance_task() {
    set_cpu(1);

    while(1){
        asm volatile("" ::: "memory");
    }
}
void* PeriodicPollTask(void* arg) {
    set_cpu(1);
    responseTaskArgs* args = (responseTaskArgs*)arg;
    
    while (1) {
        // Busy-wait until the test pin goes LOW (signal from BRTT)
        if(digitalRead(args->test_pin) == LOW) {
            // Respond by pulling the response pin LOW
            digitalWrite(args->response_pin, LOW);
            
            // Wait for the test pin to return HIGH (end of the test signal pulse)
            while (digitalRead(args->test_pin) == LOW) {
                // Busy waiting for test signal release
            }
            
            // Return response pin to HIGH
            digitalWrite(args->response_pin, HIGH);
        }

        //2 000 000 ns = 2 ms
        sleep_realtime(2000000);
    }
    
    return NULL;
}

int main(void) {
    // Initialize hardware and pins
    io_init();
    
    pthread_t threadA, threadB, threadC;

    // periodic poll threads handle
    //pthread_t threadA_periodic, threadB_periodic, threadC_periodic;

    pthread_t threads[NUM_DIST_THREADS];
    
    // Setup argument structs for each thread using the renamed pins
    responseTaskArgs argsA = { .test_pin = TEST_A, .response_pin = RESPONSE_A };
    responseTaskArgs argsB = { .test_pin = TEST_B, .response_pin = RESPONSE_B };
    responseTaskArgs argsC = { .test_pin = TEST_C, .response_pin = RESPONSE_C };
    
    printf("Starting continuous response tasks for A, B, and C...\n");
    
    // creating the disturbance threads
    for (int i = 0; i<NUM_DIST_THREADS; i++){
        pthread_create(&threads[i], NULL, disturbance_task, NULL);
    }
    // Create POSIX threads for Tasks A, B, and C
    pthread_create(&threadA, NULL, TaskA, &argsA);
    pthread_create(&threadB, NULL, TaskB, &argsB);
    pthread_create(&threadC, NULL, TaskC, &argsC);

    // creating the periodic poll threads
    //pthread_create(&threadA_periodic, NULL, PeriodicPollTask, &argsA);
    //pthread_create(&threadB_periodic, NULL, PeriodicPollTask, &argsB);
    //pthread_create(&threadC_periodic, NULL, PeriodicPollTask, &argsC);
    
    for(int i = 0;i<NUM_DIST_THREADS;i++){
        pthread_join(threads[i],NULL);
    }
    // Join threads (since they run indefinitely, execution stays here)
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);

    //joining the periodic poll threads
    //pthread_join(threadA_periodic, NULL);
    //pthread_join(threadB_periodic, NULL);
    //pthread_join(threadC_periodic, NULL);

    
    return 0;
}