#include <Tetris.h>

extern "C" {

int write(const char *buffer, int size) {
    Serial.write(buffer,size); 
}

int available() {
    return Serial.available();
}

int getchr() {
    return Serial.read();
}

}

void setup() {
    Serial.begin(9600);

    sei( );

    extern void _main();
    _main();
}

void loop() {
} 
