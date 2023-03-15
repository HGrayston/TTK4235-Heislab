#include <time.h>
#pragma once

#define Ko_str 8



struct Heis {
    int targetFloor;               // Initialiseres som currentfloor 
    int currentFloor;              // Initialiseres som currentfloor
    int retning;                   // 0 er ned, 1 er opp. Initialiseres med 2 aner ikke om nødvendig
    int ko[Ko_str];                // indeks 0-3 er ned. 4-7 er opp. 1 betyr ja. 0 betyr nei
    int prioriteringsko[Ko_str];   // 0 betyr ingenting. 1-4 er ned. 4-7 er opp
    time_t time;                   // Heisens timer som da resettes ved obstruksjoner og døråpne(). se timer.c
};

// Initiellfunksjoner
void init(struct Heis* h);
void nullstillko(struct Heis* h);
void getFloorinit(struct Heis* h);
void getFloor(struct Heis* h);


// Hente fra stack 
void sjekketasjeknapp(struct Heis* h);
void feilsideavretn(struct Heis* h);


// Køhåndtering
void reorderque(struct Heis* h);
void priotitering(int bestilling, struct Heis* h);
void fjernfrako(struct Heis* h);

void oppdatertarget(struct Heis* h);

// Logikk til heis
void SetTargetFloor(struct Heis* h);
void getRetning(struct Heis* h);
void startHeis();
void sjekkomStopp(struct Heis* h);
void stoppEtasje(struct Heis* h);
void stoppknapp(struct Heis* h);



