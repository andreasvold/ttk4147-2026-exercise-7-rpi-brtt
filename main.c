#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>

#define TEST_A     8
#define RESPONSE_A 9
#define TEST_B     7
#define RESPONSE_B 15
#define TEST_C     16
#define RESPONSE_C 1

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

// Reusable continuously running thread function for Tasks A, B, and C
void* TaskA(void* arg) {
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

int main(void) {
    // Initialize hardware and pins
    io_init();
    
    pthread_t threadA, threadB, threadC;
    
    // Setup argument structs for each thread using the renamed pins
    responseTaskArgs argsA = { .test_pin = TEST_A, .response_pin = RESPONSE_A };
    responseTaskArgs argsB = { .test_pin = TEST_B, .response_pin = RESPONSE_B };
    responseTaskArgs argsC = { .test_pin = TEST_C, .response_pin = RESPONSE_C };
    
    printf("Starting continuous response tasks for A, B, and C...\n");
    
    // Create POSIX threads for Tasks A, B, and C
    pthread_create(&threadA, NULL, TaskA, &argsA);
    pthread_create(&threadB, NULL, TaskB, &argsB);
    pthread_create(&threadC, NULL, TaskC, &argsC);
    
    // Join threads (since they run indefinitely, execution stays here)
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    
    return 0;
}