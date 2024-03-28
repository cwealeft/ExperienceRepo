#include "mbed.h"

SPI microP(p11,p12,p13);      //initialize pin - device comm
/* how to discern between mbed pins and other pins
   when select is hi on p15, talking to 165
   when select is lo on p14, talking to 595
   seg msg comes from mbed mosi p11, goes to 595 p14
   switch states come from 165 p9 to mbed p12 */
DigitalOut ss595(p14);
DigitalOut ss165(p15);

Serial cpu(USBTX, USBRX);

volatile int swState;       // 1=A, 2=B, 3=C
//volatile bool play = true;	 

Timer gameT;

int randNum = rand() % 3 + 1, rnd1 = 1000, rndCnt = 0;   //init w/ random , 1 sec


int main() {
    gameT.start();
    cpu.baud(9600);
    cpu.format(8,SerialBase::None,1);
    microP.frequency(1000000);	// freq set by chips, 1MHz
    wait(.055);
    srand(gameT.read_us());
    randNum = rand() % 3 + 1;
    gameT.stop();
    gameT.reset();
    cpu.printf("%d \r\n", randNum);
	while(1) {
        gameT.start(); 
	    microP.format(8,0);
			if (randNum == 1) {
                ss595 = 0;                  // speaking to 595
				microP.write(0b11111010);
                ss595 = 1;                  // de-select
                wait_us(100);               // 595 proc time
            }                           // need brackets if >1 line
			else if (randNum == 2) {
                ss595 = 0;
				microP.write(0b00111110);
                ss595 = 1;
                wait_us(100);
            }
			else if (randNum == 3) {
                randNum = 4;        // to match reg bit for 3rd sw
                ss595 = 0;
				microP.write(0b00011010);
                ss595 = 1;
                wait_us(100);
            }
        gameT.reset();	
        microP.format(8,2);
        ss165 = 1;
        swState = microP.write(1);
        ss165 = 0;
        wait_us(50);
            while(gameT.read() <= (rnd1 - rndCnt*200)){  
                microP.format(8,2);
                ss165 = 1;
                swState = microP.write(1);
                ss165 = 0;
                wait_us(50);
                    if ((swState != randNum && swState != 0) /*|| (gameT > (rnd1 - rndCnt*200))*/) {
                    ss165 = 0;
                    ss595 = 0;
                    microP.format(8,0);
                    microP.write(0b00011100);
                    ss595 = 1;
                    wait_us(100);
                    gameT.stop();
                    gameT.reset();
                    //ticker.detach();
                    rndCnt = 0;
                        }
                }
        if (gameT > (rnd1 - rndCnt*200)) {
        ss165 = 0;
        ss595 = 0;
        microP.format(8,0);
        microP.write(0b00011100);
        ss595 = 1;
        wait_us(100);
        gameT.stop();
        gameT.reset();
            }
        if (swState == randNum) {
            gameT.stop();
            ++rndCnt;
            gameT.reset();
            wait_us(100);
	        }
            if (rndCnt < 5)
                rndCnt = 0;
        
    }

}

