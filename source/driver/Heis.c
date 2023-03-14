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
    h->aktiv = 0;
    h->targetFloor = h->currentFloor;  
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
            h->ko[koindex] = JaNei;
            if(JaNei == 1){
                prioritering(koindex,h);
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
    // --------------------------- Logikk for å oppdatere knapper = sjekketasjeknapp(h) + stoppknapp(h)
        }
        elevio_motorDirection(0);
    }  

    if (bestillingfra > h->currentFloor){
        elevio_motorDirection(1);

        while (h->currentFloor != bestillingfra)
        { 
            getFloor(h);
    // --------------------------- Logikk for å oppdatere knapper = sjekketasjeknapp(h) + stoppknapp(h)
        }
        elevio_motorDirection(0);
    } 
}


/*
void feilsideavretn(struct Heis* h){
    // Her skal vi rette opp dersom heisen er på feil side av den veien den skal.
    // Sjekker scenario om vi skal opp men er nå plassert over den som har bestillt
    if ((h->prioriteringsko[0] > 3) && ((h->prioriteringsko[0] - 4) < h->currentFloor))
    {
        elevio_motorDirection(-1);
        while (h->currentFloor != (h->prioriteringsko[0] - 4))
        {
            getFloor(h);
    // --------------------------- Logikk for å oppdatere ting
    //        sjekketasjeknapp(h);
    //        stoppknapp(h);
        }
        elevio_motorDirection(0);
    }  
    if ((h->prioriteringsko[0] < 4) && (h->prioriteringsko[0] > h->currentFloor)){
        elevio_motorDirection(1);
        while (h->currentFloor != h->prioriteringsko[0])
        { 
            getFloor(h);
    // --------------------------- Logikk for å oppdatere ting
    //        sjekketasjeknapp(h);
    //        stoppknapp(h);
        }
        elevio_motorDirection(0);
    } 
}
*/




//-------------------------------------------------------------


void oppdatertarget(struct Heis* h){

// Oppdaterer target på to måter. Enten om vi skal opp eller ned


// Hvis opp
    if(h->retning == 1)
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

//Hvis ned
    if(h->retning == 0)
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
}



void ventpabestilling(struct Heis* h){
    time_t start_time, current_time;
    
    start_time = time(NULL);
    
    while(1){

        oppdatertarget(h);
   // Logikk for å oppdatere ting i loop     sjekketasjeknapp(h);        stoppknapp(h);
        current_time = time(NULL);
        if(current_time - start_time >= 3){
            break;
        }
    }
}




// --------------------------------------------------------
// Har nå fått bestillinger i heisen og starter å kjøre mot etasje




void startHeis(struct Heis* h){
    if (h->retning == 1)
    {
        elevio_motorDirection(1);
    }
    else
    {
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
    } if (h->targetFloor > h->currentFloor)
    {
        h->retning = 1;
    }
    else
    {
        h->retning = 0; // BYTTET OM fortegn
    }
}


void fjernfrako(struct Heis* h){
    h->prioriteringsko[0] = 0;
/*
    int index = h->currentFloor + 4*h->retning;
    h->ko[index] = 0;
    for(int i = 0; i < Ko_str ; i++){
        if(h->prioriteringsko[i] == index){
            h->prioriteringsko[i] = 0;
        }
    }
*/
    reorderque(h);
}


/*
void sjekkomStopp(struct Heis* h){
    if (h->ko[(h->retning)*4+(h->currentFloor)] == 1)
    {
        stoppEtasje(h);
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
*/
