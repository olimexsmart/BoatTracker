/*


*/

// libraries
#include <GSM.h>
#include <SD.h>
#include <TinyGPS++.h>

// PIN Number
#define PINNUMBER 		""
#define GPRS_APN       	"wap.tim.it"
#define GPRS_LOGIN     	"WAPTIM"
#define GPRS_PASSWORD  	"WAPTIM"
#define GPSBaud 		57600
#define port			80
#define server			"olimexsmart.it"
#define DATARATE		45000	// Send point every N millisecons
#define TIMEOUT			5000	// Connection timeout for response from server
#define SD_CS			11		// SPI SD CS on pin 11
#define FILEBUFF		"buffer.txt"
#define FILELOG			"logger.txt"
#define BUFFLEN			256
#define LOOPLED			45
#define SETUPLED		46
#define GPSLED			44
#define GSMLED			36
#define SDLED			33

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;
TinyGPSPlus gps;

char data[BUFFLEN];
unsigned long loopTimer;

// connection state
boolean GSMnotInit = true;



/*
    SETUP
*/
void setup() {
    // Init LED pins as outputs
    pinMode(LOOPLED, OUTPUT);
    pinMode(SETUPLED, OUTPUT);
    pinMode(GPSLED, OUTPUT);
    pinMode(GSMLED, OUTPUT);
    pinMode(SDLED, OUTPUT);

    // Rise all leds while setup is executing
    digitalWrite(SETUPLED, HIGH); // Setup is executing
    digitalWrite(GPSLED, HIGH);
    digitalWrite(GSMLED, HIGH);
    digitalWrite(SDLED, HIGH);

    // initialize serial communications and wait for port to open:
    Serial.begin(115200);
    Serial1.begin(GPSBaud);

    // Init SD card
    pinMode(SD_CS, OUTPUT);
    if (!SD.begin(SD_CS)) {
        printDebug(F("ERROR SD card initialization failed.\n"));
        //return; The system can partially work even without SD card
    } else {
        printDebug(F("SD init OK.\n"));
        digitalWrite(SDLED, LOW);
    }


    printDebug(F("Starting Boat Tracker.\nFree RAM bytes: "));
    printDebug(itoa(FreeRam(), data, 10));
    printDebug(F("\nWaiting for GPS fix...\n"));

    // Wait for GPS signal to become valid
    waitGPSFix();
    digitalWrite(GPSLED, LOW);
    printDebug(F("Got GPS fix.\nWaiting for GSM connectivity...\n"));

    // Wait for GSM to be connecter to network
    waitGSMFix();
    if (!GSMnotInit) {
        digitalWrite(GSMLED, LOW);
        printDebug(F("Got GSM connection.\n"));
    } else {
        printDebug(F("ERROR GSM module not initialized correctly, will retry later.\n"));
    }


    printDebug(F("Starting main loop.\n################\n\n"));

    digitalWrite(SETUPLED, LOW);
}


/*
    LOOP
*/
void loop() {
    /*
        If the connection to the server fails, the data will be saved into the memory card.
        Whenever the connection is re-estabilished, all the data stored will be sent.
    */
    digitalWrite(LOOPLED, HIGH);
    loopTimer = millis();
    // Get GPS data into URL encoded string
    if (getFormattedData()) {
        // Send to server
        printDebug(data);
        if (GSMnotInit) // If not initialized, initialize
            waitGSMFix();

        if (sendData()) { // If not connected this will fail anyway
            digitalWrite(GSMLED, LOW);
            printDebug(F("Positive sendData call.\n"));
            // Probably we have connection, try to empty the buffer
            // At the same time the function must return for another loop
            if (sendBufferEntries(DATARATE - constrain(millis() - loopTimer, 0, DATARATE)))
                printDebug(F("Positive sendBufferEntries call.\n"));
            else
                printDebug(F("Negative sendBufferEntries call.\n"));
        }
        else {
            digitalWrite(GSMLED, HIGH); // Signal disconnection
            printDebug(F("Negative sendData call.\n"));
            if (storeInBuffer()) {
                digitalWrite(SDLED, LOW);
                printDebug(F("Positive storeInBuffer call.\n"));
            }
            else {
                digitalWrite(SDLED, HIGH);
                printDebug(F("Negative storeInBuffer call.\n"));
            }
        }
    } else {
        digitalWrite(GPSLED, HIGH);
        waitGPSFix();
        digitalWrite(GPSLED, LOW);
    }

    printDebug(F("End loop with millis left: "));
    printDebug(ltoa(DATARATE - constrain(millis() - loopTimer, 0, DATARATE), data, 10));
    printDebug(F("\n++++++++++++++++\n\n"));
    digitalWrite(LOOPLED, LOW);

    // Repeat keeping the GPS active, maintaining a constant frequency
    smartDelay(DATARATE - constrain(millis() - loopTimer, 0, DATARATE));
}


void printDebug(const __FlashStringHelper *ifsh) {
    //
    PGM_P p = reinterpret_cast<PGM_P>(ifsh);
    File history = SD.open(FILELOG, FILE_WRITE);
    unsigned char c = pgm_read_byte(p++);

    if (history) { // Write on file only if open was successful
        while (c != 0) {
            Serial.write(c);
            history.write(c);
            c = pgm_read_byte(p++);
        }
        history.close();
    } else {
        while (c != 0) {
            Serial.write(c);
            c = pgm_read_byte(p++);
        }
    }
}


void printDebug(char * message) {
    Serial.print(message);
    File history = SD.open(FILELOG, FILE_WRITE);
    if (history) {
        history.print(message);
        history.close();
    }
}



