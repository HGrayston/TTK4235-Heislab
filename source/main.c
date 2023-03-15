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
        kjortilbestilling(heis);
        ventpabestilling(heis);


        if(heis->targetFloor != heis->currentFloor){
            startHeis(heis);
        }

        while (heis->targetFloor != heis->currentFloor)
        {     
     //       stoppknapp(heis);
            sjekketasjeknapp(heis);
            oppdatertarget(heis);
            getFloor(heis);
            sjekkomStopp(heis);
        }
        stoppHeis();
        fjernfrako(heis);

    }

    return 0;
}




////        feilsideavretn(heis);



