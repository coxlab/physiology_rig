// Play around with the AD9835 DDS breakout board from SparkFun
// http://www.sparkfun.com/products/9169
//
// by Mark J. Blair, NF6X
//
// Some bits taken from tjfreebo's comment post on 3/22/2011
//
// License: Do whatever you want with this. No warranty. Thppppt.


#include <SPI.h>
#include <math.h>

// I/O pin assignments
const int SCLKpin   = 13;   // SCK
// 12 = MISO
const int SDATApin  = 11;   // MOSI
// 10 = SS
const int FSYNCpin  = 9;    // PORTB.1
const int FSELpin   = 8;    // PORTB.0
const int PSEL1pin  = 7;    // PORTD.7
const int PSEL0pin  = 6;    // PORTD.6


// DDS clock frequency (Hz)
#define FCLK 50000000

// Phase and frequency calculation debugging
#define DEBUG_CALC
#define BAUD 115200

// Write directly to AVR port in SPIwrite() instead of using digitalWrite()?
#define FAST_IO



// SPI write to AD9835 register
void SPIwrite(int byte1, int byte2) {
    // take the FSYNC pin low to select the chip:
#ifdef FAST_IO
    PORTB &= ~0x02;
#else
    digitalWrite(FSYNCpin, LOW);
#endif

    //  send in the address and value via SPI:
    SPI.transfer(byte1);
    SPI.transfer(byte2);

    // take the FSYNC pin high to de-select the chip:
#ifdef FAST_IO
    PORTB |= 0x02;
#else
    digitalWrite(FSYNCpin, LOW);
#endif
}


// Calculate the frequency tuning word for a DDS with a 32 bit
// phase register. Desired frequency and DDS clock frequency
// specified in Hz.
unsigned long calcFTW32(unsigned long freq, unsigned long fclk) {

    unsigned long FTW;
    double FTWf;

    // This calculation loses precision on the Arduino because doubles
    // are presently implemented as floats. So, for example,
    // calcFTW32(10000000,50000000) should return 0x33333333, but it
    // instead returns 0x33333340 on the Arduino. A good future improvement
    // to this code would be to implement an efficient calculation that maintains
    // the full 32 bit FTW precision for some desired resolution (say, 1 Hz).
    FTWf = pow(2,32) * (double)freq / (double)fclk;
    FTW = (unsigned long) FTWf;
    
#ifdef DEBUG_CALC
    Serial.print("calcFTW32(");
    Serial.print(freq);
    Serial.print(", ");
    Serial.print(fclk);
    Serial.print(") = 0x");
    Serial.println(FTW, HEX);
#endif

    return FTW;
}


// Calculate the phase tuning word for a DDS with a 12 bit
// phase offset register. Desired phase specified in Hz (0-359)
unsigned int calcPTW12d(unsigned int deg) {

    unsigned int PTW;
    double PTWf;
    
    PTWf = ((double)(deg % 360) / 360.0) * pow(2,12);
    PTW = (unsigned int) PTWf;
    
#ifdef DEBUG_CALC
    Serial.print("calcPTW12d(");
    Serial.print(deg);
    Serial.print(") = 0x");
    Serial.println(PTW, HEX);
#endif

    return PTW;
}


// Write to speficied frequency register (0-1) of AD9835
void writeFTW(unsigned long FTW, int reg) {
    int regaddr;
    regaddr = (reg & 0x01) << 2;

    SPIwrite(0x33 + regaddr, ((FTW & 0xFF000000) >> 24));
    SPIwrite(0x22 + regaddr, ((FTW & 0x00FF0000) >> 16));
    SPIwrite(0x31 + regaddr, ((FTW & 0x0000FF00) >> 8));
    SPIwrite(0x20 + regaddr, ((FTW & 0x000000FF)));
}


// Write to specified phase register (0-3) of AD9835
void writePTW(unsigned int PTW, int reg) {
    int regaddr;
    regaddr = (reg & 0x03) << 1;

    SPIwrite(0x19 + regaddr, ((PTW & 0x0F00) >> 8));
    SPIwrite(0x08 + regaddr, ((PTW & 0x00FF)));
}



// Initialization code
void setup() {

#ifdef DEBUG_CALC
    Serial.begin(BAUD);
    Serial.println("reset");
#endif

    digitalWrite(FSYNCpin, HIGH); 
    digitalWrite(SCLKpin,  LOW); 
    digitalWrite(FSELpin,  LOW);
    digitalWrite(PSEL0pin, LOW);
    digitalWrite(PSEL1pin, LOW);
    pinMode(FSYNCpin, OUTPUT);
    pinMode(SCLKpin,  OUTPUT);
    pinMode(FSELpin,  OUTPUT);
    pinMode(PSEL0pin, OUTPUT);
    pinMode(PSEL1pin, OUTPUT);

    SPI.setDataMode(SPI_MODE1);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.begin();

    // Reset and clear device
    SPIwrite(0xF8, 0x00);
    delay(1);
  
    // Power up device
    SPIwrite(0xC0, 0x00);

    // Set sync and FSEL source (external pins)
    SPIwrite(0x80, 0x00);
  
}

// Main loop
void loop() {

    // Set up frequency registers for toggle
    // between 5MHz and 10MHz with FSEL pin    
    writeFTW(calcFTW32(1000, 50000000), 0);
    writeFTW(calcFTW32( 100, 50000000), 1);

    // Set up phase registers for QPSK with PSEL1, PSEL0 pins
    writePTW(calcPTW12d(0),   0);
    writePTW(calcPTW12d(90),  1);
    writePTW(calcPTW12d(180), 2);
    writePTW(calcPTW12d(270), 3);

    while (1) {
        // Do nothing... we'll just output 10 MHz. If we fiddled with the
        // FSEL, PSEL1 and PSEL0 pins here, interesting things would happen.
        // We could also repeatedly write to the frequency registers to change
        // frequencies. Since the Arduino is pretty slow at doing floating-point
        // math, pre-computing the desired frequencies might be necessary to
        // achieve a suitable sampling rate (for example, to frequency modulate
        // the DDS output with a sine wave). For FSK, BPSK or QPSK, it'd be most
        // efficient to preload the DDS's frequency and/or phase registers and
        // then twiddle the FSEL/PSELn pins to modulate its output.
        
        delay(1000); 
    }  
}
