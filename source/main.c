#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

#include "driver/Heis.h"

int main(){
    struct Heis h;
    struct Heis* heis = &h;

    init(heis);



  while(1)
    {
        while (heis->prioriteringsko[0] == 0)
        {

            sjekketasjeknapp(heis);
        }
        
//        feilsideavretn(heis);
        kjortilbestilling(heis);
        
// Nå skal heisen stå stille i etasjen den har fått bestilling fra
// Her vil vi kjøre delay + oppdatere target
        ventpabestilling(heis);

        if(heis->targetFloor != heis->currentFloor){
            startHeis(heis);
        }
        startHeis(heis);

        while (heis->targetFloor != heis->currentFloor)
        {
            
            sjekkomStopp(heis); // Må inneholde funksjon for oppdatering av target ++ logikk for når mellom etasjer
     //       stoppknapp(heis);
            sjekketasjeknapp(heis);
            oppdatertarget(heis);
            getFloor(heis);
        }
        stoppHeis();
        fjernfrako(heis);

    }

    return 0;
}












