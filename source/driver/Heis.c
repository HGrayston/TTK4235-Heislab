#include "Heis.h"
#include "elevio.h"
#include "timer.h"

void init(struct Heis* h){
    nullstillko(h);
    elevio_init();
    getFloor(h);
    h->retning = 2;
    h->aktiv = 0;
    h->targetFloor= h->currentFloor;  
}

void nullstillko(struct Heis* h){
    for (int i = 0; i < Ko_str; i++)
    {
        h->ko[i]=0;
        h->prioriteringsko[i] = 0;
    }
}

void getFloor(struct Heis* h){
    if (elevio_floorSensor() != -1) 
    {
        h->currentFloor = elevio_floorSensor(); // Flagge slik at vi kun endrer når vi kommer i ny etasje
    }
}


void SetTargetFloor(struct Heis* h){
    if (h->prioriteringsko[0] != 0)
    {
        if(h->prioriteringsko[0] < 5)
        {
            h->targetFloor = h->prioriteringsko[0];
        }
        else
        {
            h->targetFloor = h->prioriteringsko[0] - 4;
        }
            
        h->targetFloor = h->prioriteringsko[0]; 
    } 
}


void getRetning(struct Heis* h){
    if (h->targetFloor > h->currentFloor)
    {
        h->retning = 1;
    }
    else
    {
        h->retning = 0; // BYTTET OM fortegn
    }
}

void startHeis(struct Heis* h){
    if (h->retning == 1)
    {
        elevio_motorDirection(1);
    }
    else
    {
        elevio_motorDirection(-1);
    }e = 0;
    }
}


void oppdatertarget(struct Heis* h){
    if(h->prioriteringsko[0] > 4) // Da skal vi opp
    {
        for (int i = 0; i < 4; i++)
        {
            if (elevio_callButton(i,2) == 1)
            {
                if ((i > h->currentFloor)  && (i < h->targetFloor))
                {
                    h->ko[i+4] = 1;
                }
                if ((i > h->currentFloor)  && (i > h->targetFloor))
                {
                    h->targetFloor = i;
                    h->ko[i+4] = 1;
                }
            }
        }
    }
    if(h->prioriteringsko[0] < 4) // Da skal vi ned
    {
        for (int i = 0; i < 4; i++)
        {
            if (elevio_callButton(i,2) == 1)
            {
            
                if ((i < h->currentFloor)  && (i > h->targetFloor) )
                {
                    h->ko[i] = 1;
                }
                if ((i < h->currentFloor)  && (i < h->targetFloor))
                {
                    h->targetFloor = i;
                    h->ko[i] = 1;
                }
            }
        }
    }
    getRetning(h);
}

void sjekketasjeknapp(struct Heis* h){
    int koindex;
    int retn;
    for(int i = 0; i < 4; i++){
        retn = 1;
        for(int k = 0; k < 2; k++){
            int JaNei = elevio_callButton(i,k);
            k++;
            koindex = i + retn*4;
            h->ko[koindex] = JaNei;
            if(JaNei == 1){
                priotitering(koindex,h);
            }
            retn = -1;
        }
    }
}


void stoppEtasje(struct Heis* h){
    elevio_motorDirection(0);
    h->aktiv = 0;
// FUnkjson som starter timer     
//    StartTimer();
    time_t start_time, current_time;
    
    start_time = time(NULL);
    while(1){
        stoppknapp(h);
        oppdatertarget(h);
        sjekketasjeknapp(h);
        current_time = time(NULL);
        if(current_time - start_time >= 3){
            break;
        }


    }

// Mens dør er åpen må vi oppdatere knapper, oppdatere target og sjekke stoppknapp
// Funksjon som oppdaterer target
    fjernfrako(h);
    startHeis(h);
    h->aktiv = 1;
}


void reorderque(struct Heis* h){
    for(int i = 0; i < Ko_str; i++)
    {
        if(h->prioriteringsko[i] == 0)
        {
            for(int k = i+1; k < Ko_str; k++)
            {
                if(h->prioriteringsko[k] != 0)
                {
                    h->prioriteringsko[i] = h->prioriteringsko[k];
                    h->prioriteringsko[k] = 0;
                    break;
                }
            }
        }
    } if (h->targetFloor > h->currentFloor)
    {
        h->retning = 1;
    }
    else
    {
        h->retning = 0; // BYTTET OM fortegn
    }
}
}

void fjernfrako(struct Heis* h){
    int index = h->currentFloor + 4*h->retning;
    h->ko[index] = 0;
    for(int i = 0; i < Ko_str ; i++){
        if(h->prioriteringsko[i] == index){
            h->prioriteringsko[i] = 0;
        }
    }
    reorderque(h);
}


void sjekkomStopp(struct Heis* h){
    if (h->ko[(h->retning)*4+(h->currentFloor)] == 1)
    {
        stoppEtasje(h);

    }

}











void priotitering(int bestilling, struct Heis* h) {
    int in;
    for(int i = 1; i < Ko_str; i++){
        in = 0;
        if (h->prioriteringsko[i] == bestilling){
            in = 1;
            break;
        }
    }
    if(in != 1){
        for(int k = 0; k < Ko_str;k++){
            if(h->prioriteringsko[k] == 0){
                h->prioriteringsko[k] = bestilling;
                break;
            }
        }
    }
}





void feilsideavretn(struct Heis* h){

    int endre = 0;
    if ((h->prioriteringsko[0] > 4) && ((h->prioriteringsko[0] - 4) < h->currentFloor)){
        int endre = 1;
        elevio_motorDirection(-1);
        h->aktiv = 1;

        while (h->currentFloor != (h->prioriteringsko[0] - 4))
        {
            getFloor(h);
            sjekketasjeknapp(h);
            stoppknapp(h);
        }
        elevio_motorDirection(0);
        h->aktiv = 0;
        stoppEtasje(h);
    }  

    if ((h->prioriteringsko[0] < 5) && (h->prioriteringsko[0] > h->currentFloor)){
        int endre = 1;
        elevio_motorDirection(1);
        h->aktiv = 1;
        while (h->currentFloor != (h->prioriteringsko[0] - 4))
        {
            getFloor(h);
            sjekketasjeknapp(h);
            stoppknapp(h);
        }
        elevio_motorDirection(0);
        h->aktiv = 0;
        stoppEtasje(h);
    } 
    if (endre == 0)
    {
        h->targetFloor = h->prioriteringsko[0];
    }  
}



