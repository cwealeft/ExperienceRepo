// Registers for port 0 for switches and RedLEDs
#define FIO0DIR (*(volatile unsigned int *)0x2009c000)
#define FIO0PIN (*(volatile unsigned int *)0x2009c014)
#define FIO0SET (*(volatile unsigned int *)0x2009c018)
#define FIO0CLR (*(volatile unsigned int *)0x2009c01C)

// Registers for port 1 for internal LEDs
#define FIO1DIR (*(volatile unsigned int *)0x2009c020)
#define FIO1PIN (*(volatile unsigned int *)0x2009c034)
#define FIO1SET (*(volatile unsigned int *)0x2009c038)
#define FIO1CLR (*(volatile unsigned int *)0x2009c03C)

// Registers for port 2 for GrnLEDs
#define FIO2DIR (*(volatile unsigned int *)0x2009c040)
#define FIO2PIN (*(volatile unsigned int *)0x2009c054)
#define FIO2SET (*(volatile unsigned int *)0x2009c058)
#define FIO2CLR (*(volatile unsigned int *)0x2009c05C)

#define PINMODE0 (*(volatile unsigned int *)0x4002c040)
#define PINMODE1 (*(volatile unsigned int *)0x4002c044)
#define PINMODE2 (*(volatile unsigned int *)0x4002c048)
#define PINMODE3 (*(volatile unsigned int *)0x4002c04c)

void writeGrn1(int state){
    
    if (state)
        FIO2PIN |= (1<<5);
    else
        FIO2PIN &= ~(1<<5);
}

void writeGrn2(int state){
    
    if (state)
        FIO2PIN |= (1<<2);
    else
        FIO2PIN &= ~(1<<2);
}

void writeRed1(int state){
    
    if (state)
        FIO0PIN |= (1<<25);
    else
        FIO0PIN &= ~(1<<25);
}

void writeRed2(int state){
    
    if (state)
        FIO0PIN |= (1<<16);
    else
        FIO0PIN &= ~(1<<16);
}

void writeLED1(int state){
    if (state)
        FIO1PIN |= (1<<18);
    else
        FIO1PIN &= ~(1<<18);
}

void writeLED2(int state){
    if (state)
        FIO1PIN |= (1<<20);
    else
        FIO1PIN &= ~(1<<20);
}

void writeLED3(int state){
    if (state)
        FIO1PIN |= (1<<21);
    else
        FIO1PIN &= ~(1<<21);
}

void writeLED4(int state){
    if (state)
        FIO1PIN |= (1<<23);
    else
        FIO1PIN &= ~(1<<23);
}

static inline int readRedSw(void){
    return ((FIO0PIN>>18) & 1);
}

static inline int readGrnSw(void){
    return ((FIO0PIN>>10) & 1);
}

static inline int readGrn1(){
    return ((FIO2PIN>>5) & 1);
}

static inline int readGrn2(){
    return ((FIO2PIN>>2) & 1);
}

static inline int readRed1(){
    return ((FIO0PIN>>25) & 1);
}

static inline int readRed2(){
    return ((FIO0PIN>>16) & 1);
}
void wait(float sec) {
    volatile int count = sec*21.33e6;   // start with a count based on MHz
    while (count>0) count--;          // decrement count to zero
}

void initPorts(void){
    FIO0DIR &= ~(1<<18);   //p11 P0.18 input switchRed
    FIO0DIR |= (1<<16);    //p14 P0.16 output redLED2
    FIO0DIR |= (1<<25);    //p17 P0.25 output redLED1
    FIO0DIR &= ~(1<<10);   //p28 P0.10 input switchGrn
    FIO2DIR |= (1<<2);     //p24 P2.2 output grnLED2
    FIO2DIR |= (1<<5);     //p21 P2.5 output grnLED1

    FIO1DIR |= (1<<18);    //LED1 P1.18
    FIO1DIR |= (1<<20);    //LED2 P1.20
    FIO1DIR |= (1<<21);    //LED3 P1.21
    FIO1DIR |= (1<<23);    //LED4 P1.23


    PINMODE1 |= (1<<4) | (1<<5);    // Pull-down red on p11 5:4 P0.18
    PINMODE0 |= (1<<20) | (1<<21);  // Pull-down grn on p28 21:20 P0.10
    PINMODE3 &= ~(1<<4) | (1<<5);   // Pull-up LED1 on 5:4 P1.18 
}

void startLEDs(){
    writeLED1(0);
    wait(.005);
    writeLED2(1);
    wait(.005);
    writeLED3(1);
    wait(.005);
    writeLED4(0);
    wait(.005);
    writeGrn1(0), writeGrn2(0), writeRed1(0), writeRed2(0);
}
void updateLEDs(int grn, int red){
    
}
int main() {
    int grn = 0, red = 0;
    initPorts();
    startLEDs();
    while(1) {
        
        if (readGrnSw()){
            writeLED3(1);
            wait(.025);
            writeLED4(1);
            wait(.025);
            writeLED2(0);
            wait(.025);
            writeLED1(0);
            grn++;
        }
        wait(.1);           // while press/release switch
        if (readRedSw()){
            writeLED1(1);
            wait(.025);
            writeLED2(1);
            wait(.025);
            writeLED3(0);
            wait(.025);
            writeLED4(0);
            red++;
        }
        wait(.1);
        if (readGrnSw() && grn == 1){
            writeLED4(1);
            wait(.025);
            writeGrn1(1);
            wait(.025);
            writeLED3(0);
            wait(.025);
            writeLED2(0);
            wait(.025);
            grn++;
        }
        wait(.1);
        if (readRedSw() && red == 1){
            writeLED1(1);
            wait(.025);
            writeRed1(1);
            wait(.025);
            writeLED2(0), writeLED3(0), writeLED4(0);
            wait(.025);
            red++;
        }
        wait(.1);
        if (readGrnSw() && grn == 2) {
            writeGrn1(1), writeGrn2(1);
            wait(.025);
            break;
        }
        wait(.1);
        break;

  }
}
