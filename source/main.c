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
        feilsideavretn(heis);
        oppdatertarget(heis);
       

        
        getRetning(heis);
        startHeis(heis);
        while (heis->targetFloor != heis->currentFloor)
        {
            getFloor(heis);
         //   sjekkomStopp(heis); // Må inneholde funksjon for oppdatering av target ++ logikk for når mellom etasjer
            stoppknapp(heis);
            sjekketasjeknapp(heis);
            oppdatertarget(heis);
        }
    }

    return 0;
}

