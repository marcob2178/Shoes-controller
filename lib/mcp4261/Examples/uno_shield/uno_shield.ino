#include <SPI.h>
#include "mcp4261.h"

// Simple example demonstrating how to use the MCP4261 library with 
// the MCP4261 Arduino Uno shield

const int POT_0_CS = 5;
const int POT_1_CS = 6;
const int LOOP_DELAY = 200;
MCP4261 pot0 = MCP4261(POT_0_CS);
MCP4261 pot1 = MCP4261(POT_1_CS);

void setup() {

    // Setup SPI communications
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.begin();

    // Initialize potentiometers
    pot0.initialize();
    pot1.initialize();
}

void loop() {
    static int cnt=0;

    pot0.setWiper0(cnt);
    pot0.setWiper1(256-cnt);

    pot1.setWiper0(cnt);
    pot1.setWiper1(256-cnt);

    cnt += 1;
    if (cnt > 256) {
        cnt = 0;
    }
    delay(LOOP_DELAY);
}
