# 💈 The Sleeping Barber Problem
This project is a C implementation of the classic Sleeping Barber synchronization problem, developed as part of the CMPE 312 Operating Systems Lab term project.

## 📚 Description
In this simulation:
- There is **one barber**, **one barber chair**, and **three waiting chairs**.
- A total of **10 customers** arrive at random intervals.
- If the barber has no customers, he sleeps.
- Customers either wait, get a haircut, or leave if all waiting chairs are occupied.

The purpose of this simulation is to demonstrate **process synchronization** using **POSIX threads**, **semaphores**, and **mutexes**.

## 🚀 Features
- Mutual exclusion for shared variables (`waiting`, `cut_count`, `skipped_count`)
- Proper use of semaphores:
  - `customer_sem` to wake the barber
  - `barber_sem` to signal a finished haircut
- Random sleep intervals for realistic customer arrival and haircut durations
- Graceful termination once all customers are handled

```bash
gcc sleeping_barber.c -o sb -lpthread
./sb
```

## 📄 Report 
See doc.pdf for a full explanation of:
- Problem statement
- Algorithm methodology
- Semaphore & mutex usage
- Execution flow
- Design considerations

## 📁 Files

- `sleeping_barber.c`: Source code
- `doc.pdf`: Detailed documentation

## 👤 Author

Burak ER — CMPE 312 OS Lab Project, May 2025
