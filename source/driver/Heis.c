#include "Heis.h"
#include "elevio.h"


// -------------------------------


void nullstillko(struct Heis* h){
    for (int i = 0; i < Ko_str; i++)
    {
        h->ko[i]=0;
        h->prioriteringsko[i] = 0;
    }
}

void getFloorinit(struct Heis* h){


    if (elevio_floorSensor() == -1) 
    {
        elevio_motorDirection(1);
    }
    while (elevio_floorSensor() == -1)
    {}
    
    elevio_motorDirection(0);
    h->currentFloor = elevio_floorSensor();
}

void init(struct Heis* h){
    
    nullstillko(h);
    elevio_init();
    getFloorinit(h);
    h->targetFloor = h->currentFloor;  
    elevio_stopLamp(0);


    for (int i = 0; i<4; i++){
        for(int j = 0; j<3; j++){
            elevio_buttonLamp(i, j, 0);
        }
    }
}





//------------------------------


void prioritering(int bestilling, struct Heis* h) {
    int in = 0;
    for(int i = 1; i < Ko_str; i++){
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

void sjekketasjeknapp(struct Heis* h){
    int koindex;
    int retn;
    int JaNei;
    for(int i = 0; i < 4; i++){
        retn = 1;
        for(int k = 0; k < 2; k++){
            JaNei = elevio_callButton(i,k);
            koindex = i + retn*4;

            if(JaNei == 1){
                prioritering(koindex,h);
                elevio_buttonLamp(i, k, 1);
                h->ko[koindex] = 1;
            }
            retn--;
        }
    }
}



// ------------------------------------

void getFloor(struct Heis* h){
    if (elevio_floorSensor() != -1 && elevio_floorSensor() != h->currentFloor)  
    {
        h->currentFloor = elevio_floorSensor();
        if(h->currentFloor >= 0){
            elevio_floorIndicator(h->currentFloor);
        }
    }

    if (elevio_floorSensor() != -1) {
        elevio_buttonLamp(h->currentFloor, 2, 0);  
    }
}


void kjortilbestilling(struct Heis* h){

    
    int bestillingfra;
    if (h->prioriteringsko[0] > 3)
    {
        bestillingfra = h->prioriteringsko[0] - 4;
        h->retning = 1;
    }
    else
    {
        bestillingfra = h->prioriteringsko[0];
        h->retning = 0;

    }

    if (bestillingfra < h->currentFloor)
    {
        elevio_motorDirection(-1);
        while (h->currentFloor != bestillingfra)
        {
            getFloor(h);
            sjekketasjeknapp(h);
            stoppknapp(h);
        }
        elevio_motorDirection(0);
    }  

    if (bestillingfra > h->currentFloor){
        elevio_motorDirection(1);

        while (h->currentFloor != bestillingfra)
        { 
            getFloor(h);
            sjekketasjeknapp(h);
            stoppknapp(h);

        }
        elevio_motorDirection(0);
    } 
    h->targetFloor = h->currentFloor;
}





//-------------------------------------------------------------



void oppdatertarget(struct Heis* h){

// Hvis opp
    if(h->retning == 1)
    {
        for (int i = 0; i < 4; i++)
        {
            if (elevio_callButton(i,2) == 1)
            {
                elevio_buttonLamp(i, 2, 1);
                if ((i > h->currentFloor)  && (i < h->targetFloor))
                {
                    h->ko[i+4] = 1;
                }
                if ((i > h->currentFloor)  && (i > h->targetFloor))
                {
                   
                    h->ko[i+4] = 1;
                    h->targetFloor = i;
                }
            }
        }
    }

//Hvis ned
    else if(h->retning == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (elevio_callButton(i,2) == 1)
            {
                elevio_buttonLamp(i, 2, 1);
                if ((i < h->currentFloor)  && (i > h->targetFloor) )
                {
                    h->ko[i] = 1;
                    
                }
                if ((i < h->currentFloor)  && (i < h->targetFloor))
                {
                    h->ko[i] = 1;
                    h->targetFloor = i;
                }
            }
        }
    }
}


void ventpabestilling(struct Heis* h){
    time_t start_time, current_time;
    
    start_time = time(NULL);
    elevio_doorOpenLamp(1);
    
    while(1){

        oppdatertarget(h);
        sjekketasjeknapp(h);       
        stoppknapp(h);
        current_time = time(NULL);
        if (elevio_obstruction()){
            start_time = current_time;
        }
        if(current_time - start_time >= 3){
                elevio_doorOpenLamp(0);
            break;
        }
    }
}




void startHeis(struct Heis* h){
    if (h->retning == 1)
    {
        h->ko[h->currentFloor+4] = 0;
        elevio_motorDirection(1);
    }
    if(h->retning == 0)
    {
        h->ko[h->currentFloor] = 0;
        elevio_motorDirection(-1);
    }
}

void stoppHeis(){
    elevio_motorDirection(0);
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
    } 
}


void fjernfrako(struct Heis* h){

    int bestillingfra;
    h->ko[h->prioriteringsko[0]] = 0;
    if (h->prioriteringsko[0] > 3)
    {
        bestillingfra = h->prioriteringsko[0] - 4;
        h->retning = 1;
    }
    else
    {
        bestillingfra = h->prioriteringsko[0];
        h->retning = 0;
    }
    
    h->prioriteringsko[0] = 0;

    int knapptype = 0;
    if(h->retning == 0){
        knapptype++;
    }
    elevio_buttonLamp(bestillingfra, knapptype, 0);
    h->targetFloor = h->currentFloor;
    reorderque(h);
}



void sjekkomStopp(struct Heis* h){
 
    if ((h->retning == 1 ) && (h->currentFloor != -1) && (h->ko[h->currentFloor+4] == 1))
    {
    stoppHeis();
    time_t start_time, current_time;
    start_time = time(NULL);
        elevio_doorOpenLamp(1);
    while(1){
        oppdatertarget(h);
        sjekketasjeknapp(h);
        current_time = time(NULL);
        if (elevio_obstruction()){
            start_time = current_time;
        }
        if(current_time - start_time >= 3){
                elevio_doorOpenLamp(0);
            break;
        }
    }
    h->ko[h->currentFloor+4] = 0;
    for (int i = 0; i < Ko_str; i++)
    {
        if(h->prioriteringsko[i] == h->currentFloor + 4){
            h->prioriteringsko[i] = 0;
            elevio_buttonLamp(h->currentFloor, h->retning - 1 , 0);
        }
    }

    startHeis(h);
    }

    else if ((h->retning == 0 ) && (h->currentFloor != -1 )&& (h->ko[h->currentFloor] == 1))
    {
    stoppHeis();
    time_t start_time, current_time;
    start_time = time(NULL);
    elevio_doorOpenLamp(1);
    while(1){
        oppdatertarget(h);
        sjekketasjeknapp(h);
        current_time = time(NULL);
        if (elevio_obstruction()){
            start_time = current_time;
        }
        if(current_time - start_time >= 3){
            elevio_doorOpenLamp(0);
            break;
        }
    }
    h->ko[h->currentFloor] = 0;
    for (int i = 0; i < Ko_str; i++)
    {
        if(h->prioriteringsko[i] == h->currentFloor){
            h->prioriteringsko[i] = 0;
            elevio_buttonLamp(h->currentFloor, h->retning + 1 , 0);
        }
    }

    startHeis(h); 
    }
 
}




void stoppknapp(struct Heis* h){

    if (elevio_stopButton() == 1)
    {
        stoppHeis();

        

        int old = h->currentFloor;
        elevio_stopLamp(1);
        while (elevio_stopButton() == 1)
        {
            nullstillko(h);
        }
//        getFloorinit(h);
        h->currentFloor = old;
        h->targetFloor = old;
        elevio_stopLamp(0);


        for (int i = 0; i<4; i++){
            for(int j = 0; j<3; j++){
                elevio_buttonLamp(i, j, 0);
            }
        }
    }
}







/*
Kjørtilbestilling
    if (elevio_floorSensor == -1 && h->retning == 1)
    {
        elevio_motorDirection(1);
        while(elevio_floorSensor != h->currentFloor)
        {
            getFloor(h);
            sjekketasjeknapp(h);
            stoppknapp(h);
        }
        elevio_motorDirection(0);
    }

       
    if (elevio_floorSensor == -1 && h->retning == 0)
    {
        elevio_motorDirection(-1);
        while(elevio_floorSensor != h->currentFloor)
        {
            getFloor(h);
            sjekketasjeknapp(h);
            stoppknapp(h);
        }
        elevio_motorDirection(0);
    }
*/
