#define CLEAR 0
#define RESETGSM 3
#define RESETMCU 4
#define LED 2

#define MAXINTERVAL 180000

volatile unsigned long timer;
volatile byte state = LOW;

void setup() {
    //pinMode(CLEAR, INPUT);
    pinMode(RESETGSM, OUTPUT);
    pinMode(RESETMCU, OUTPUT);
    pinMode(LED, OUTPUT);

    digitalWrite(RESETMCU, HIGH);
    digitalWrite(RESETGSM, HIGH);

    GIMSK = 0b00100000;    // turns on pin change interrupts
    PCMSK = 0b00000001;    // turn on interrupts on pin PB0
    sei();                 // enables interrupts

    timer = millis();
}

void loop() {
    digitalWrite(LED, state);

    if (millis() - timer > MAXINTERVAL) { // Timer expired
        digitalWrite(RESETGSM, LOW);
        digitalWrite(RESETMCU, LOW);
        delay(2000);
        digitalWrite(RESETMCU, HIGH);
        digitalWrite(RESETGSM, HIGH);
        timer = millis();
    }
}

ISR(PCINT0_vect) {
    timer = millis();
    state = !state;
}

