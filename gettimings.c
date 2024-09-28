#define _XOPEN_SOURCE 700
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

long long nsecs();
void *thread_function(void *args);

volatile sig_atomic_t received_sig = 0;

// Signal Handler function
void handler(int sig){
    (void) sig;
    received_sig = 1;
}

// returns the number of nanoseconds that have elapsed since an arbitrary time - got from HW 2 write-up
long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}

__attribute__((noinline)) void my_empty_function() {
    __asm__("");
}

// Case 1 function handles the 1. task, calling an empty function such that it is not optimized in any way and having it return
void case_1_funct(){

    long long start_time;
    long long end_time;
    long long total_time = 0;

    long long overhead_total = 0;
    for (int i = 0; i < 5; i++) {
        start_time = nsecs();
        end_time = nsecs();
        overhead_total += (end_time - start_time);
    }
    long long time_diff = overhead_total / 5;

    for (int i = 0; i < 1000000; i++){
        start_time = nsecs();
        my_empty_function();
        end_time = nsecs();
        
        total_time = (end_time - time_diff - start_time) + total_time;
    }
    double avg_time = (double) total_time / 1000000.0;
    printf("Time elasped for empty function is: %f ns\n", avg_time);
}

// Case 2 function handles the 2. task, running the getppid function from <unistd.h>
void case_2_funct(){

    long long start_time;
    long long end_time;
    long long total_time = 0;

    long long overhead_total = 0;
    for (int i = 0; i < 5; i++) {
        start_time = nsecs();
        end_time = nsecs();
        overhead_total += (end_time - start_time);
    }
    long long time_diff = overhead_total / 5;

    for (int i = 0; i < 1000000; i++){
        start_time = nsecs();
        getppid();
        end_time = nsecs();
        total_time = (end_time - time_diff - start_time) + total_time;
    }

    double avg_time = (double) total_time / 1000000.0;
    printf("Time elasped for getppid() function is: %f ns\n", avg_time);
}

// Case 3 function handles the 3. task, running a system("/bin/true")
void case_3_funct(){

    long long start_time;
    long long end_time;
    long long total_time = 0;

    long long overhead_total = 0;
    for (int i = 0; i < 5; i++) {
        start_time = nsecs();
        end_time = nsecs();
        overhead_total += (end_time - start_time);
    }
    long long time_diff = overhead_total / 5;

    for (int i = 0; i < 500; i++){
        start_time = nsecs();
        system("/bin/true");
        end_time = nsecs();
        total_time = (end_time - time_diff - start_time) + total_time;
    }

    double avg_time = (double) total_time / 500.0;
    printf("Time elasped for system(\"/bin/true\"): %f ns\n", avg_time);
}

// Case 4 function handles the 4. task, sending a signal to the current process and having its signal handler start executing but not returning
void case_4_funct(){

    long long start_time;
    long long end_time;
    long long total_time = 0;

    long long overhead_total = 0;
    for (int i = 0; i < 5; i++) {
        start_time = nsecs();
        end_time = nsecs();
        overhead_total += (end_time - start_time);
    }
    long long time_diff = overhead_total / 5;

    received_sig = 0;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigaction(SIGUSR1, &sa, NULL);

    for (int i = 0; i < 1000000; i++){
        received_sig = 0;
        start_time = nsecs();
        kill(getpid(), SIGUSR1);

        while (received_sig == 0){}

        end_time = nsecs();
        total_time = (end_time - time_diff - start_time) + total_time;
    }

    double avg_time = (double) total_time / 1000000.0;
    printf("Time elasped for sending signal and have handler start executing: %f ns\n", avg_time);
}

// Case 5 function handles the 5. task, sending a signal to another process, then having that process's signal handler send a signal back.
void case_5_funct(){

    long long start_time;
    long long end_time;
    long long total_time = 0;
    pid_t other_pid;

    long long overhead_total = 0;
    for (int i = 0; i < 5; i++) {
        start_time = nsecs();
        end_time = nsecs();
        overhead_total += (end_time - start_time);
    }
    long long time_diff = overhead_total / 5;

    printf("This PID: %d\n", getpid());
    printf("Please input Helper PID. ");
    scanf("%d", &other_pid);

    received_sig = 0;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigaction(SIGUSR2, &sa, NULL);

    for (int i = 0; i < 1000; i++) {
        received_sig = 0;
        kill(other_pid, SIGUSR1);
        start_time = nsecs();

        while (received_sig == 0){}

        end_time = nsecs();
        total_time += (end_time - time_diff - start_time);
    }

    double avg_time = (double) total_time / 1000.0;
    printf("Time elasped for sending a signal to another process, then having that process's signal handler send a signal back: %f ns\n", avg_time);

    // kill(other_pid, SIGUSR1);
    // start_time = nsecs();

    // while (received_sig == 0){}

    // end_time = nsecs();
    // total_time = (end_time - time_diff - start_time);
    // printf("Time elasped for sending a signal to another process, then having that process's signal handler send a signal back: %lld\n", total_time);
}

// Case -1 function handles the -1 helper task, pritn its PID and read a pid from STDIN
void case_neg1_funct(){

    pid_t other_pid;
    printf("Helper PID (not outputting timings): %d\n", getpid());
    printf("Please input main PID (will output timings). ");
    scanf("%d", &other_pid);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigaction(SIGUSR1, &sa, NULL);
    for (int i = 0; i < 1000; i++) {
        received_sig = 0;
        pause();

        kill(other_pid, SIGUSR2);
    }
    // pause();
    // kill(other_pid, SIGUSR2);
    // pause();

}

int main(int argc, char *argv[]){

    if (argc != 2) {
        fprintf(stderr, "Usage: %s [1-5 | -1]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int read_arg = atoi(argv[1]);

    if (read_arg == -1){
        case_neg1_funct();
        return 0;
    }

    if (read_arg >= 6 || read_arg <= 0){
        fprintf(stderr, "Invalid argument, please enter an integer between 1 and 5.\n");
        exit(EXIT_FAILURE);
    }

    switch (read_arg) {
    case 1:
        case_1_funct();
        break;
    case 2:
        case_2_funct();
        break;
    case 3:
        case_3_funct();
        break;
    case 4:
        case_4_funct();
        break;
    case 5:
        case_5_funct();
        break;
    default:
        printf("Invalid argument or other issue.\n");
        exit(EXIT_FAILURE);
    }

}