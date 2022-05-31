# Schedular

* ## Introduction

    * > Scheduling the processes by the **operating system** is one of the most  important jobs of the **OS**. We will apply some scheduling algorithm in this project, like **SRTN** algorithm, **RoundRobin** algorithm, **HPF** algorithm.
---
* ## Algorithms    

    * **SRTN** algorithm:  
        >  This algorithm depends on choosing the shortest remaining time process to be executed, it depends on the **priority queue** datastructure. and it is the **preemptive** version of **SJF** (shortest job first) algorithm.

    * Non preemptive **HPF** algorithm:
         >  This algorithm's criteria depends on choosing the highest priority process, each process has a number represents its priority, **the lowest** number represnts **the highest priority** process

    * **Round Robin** algorithm:
        > This algorithm's criteria is to give each process a **time slot** for running and then, return it back to the **ready queue** and run the next process, and so on.
---
* ## Project structure
```    
        ├── Code
        │   ├── DS
        │   │   ├── linkedlist.h
        │   │   ├── priorityQueue.h
        │   │   ├── queue.h
        |   ├── algorithms
        |   |   ├── HPF.h
        |   |   ├── RounRobin.h
        |   |   ├── SRTN.h
        |   |   ├── common.h
        |   ├── globals
        |   |   ├── PCB.h
        |   |   ├── global_variables.h
        |   |   ├── headers.h
        |   ├── helping
        |   |   ├── kill_running_processes.sh
        |   |   ├── run_scheduler.sh
        |   ├── process
        |   |   ├── clk.c
        |   |   ├── process.c
        |   |   ├── scheduler.c
        |   |   ├── process_generator.c
        |   |   ├── memory.h
        |   |   ├── memory.c
        |   |   ├── processes.txt
        |   |   ├── process_info.txt
        |                      
        ├── README.md
```
