#include <iostream>
#include <JetsonGPIO.h>

using namespace GPIO;

int main()
{
    // Pin Definitions
    int output_pin = 18; // BOARD pin 12, BCM pin 18

    // Pin Setup.
    setmode(BCM);
    // set pin as an output pin with optional initial state of HIGH
    setup(output_pin, OUT, HIGH);

    while(1){
        setup(output_pin, OUT, HIGH);
    }

    cleanup();

    return 0;
}
