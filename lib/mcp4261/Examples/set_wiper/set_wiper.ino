#include <SPI.h>
#include "mcp4261.h"

const int LOOP_DELAY = 50;
const int DIGIPOT_CS = 5;

MCP4261 digiPot = MCP4261(DIGIPOT_CS);

void setup() {

    // Setup SPI communications
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.begin();

    // Initialize pot
    digiPot.initialize();
}

void loop() {
    static int cnt=0;
    digiPot.setWiper0(cnt);
    cnt += 1;
    if (cnt > 246) {
        cnt = 0;
    }
    delay(LOOP_DELAY);
}
