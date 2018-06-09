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

    Serial.println(F("Starting Boat Tracker."));
    Serial.print(F("Free RAM bytes: "));
    Serial.println(FreeRam());
    Serial.println(F("Waiting for GPS fix..."));

    // Wait for GPS signal to become valid
    waitGPSFix();
    digitalWrite(GPSLED, LOW);
    Serial.println(F("Got GPS fix.\nWaiting for GSM connectivity..."));

    // Wait for GSM to be connecter to network
    waitGSMFix();
    if (!GSMnotInit) {
        digitalWrite(GSMLED, LOW);
        Serial.println(F("Got GSM connection.\nInitializing SD card..."));
    } else {
        Serial.println(F("ERROR GSM module not initialized correctly, will retry later."));
    }

    // Init SD card
    pinMode(SD_CS, OUTPUT);
    if (!SD.begin(SD_CS)) {
        Serial.println(F("SD card initialization failed!"));
        //return; The system can partially work even without SD card
    }
    digitalWrite(SDLED, LOW);
    Serial.println(F("SD card OK\nStarting main loop.\n#############\n"));

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
        Serial.println(data);
        if (GSMnotInit) // If not initialized, initialize
            waitGSMFix();

        if (sendData()) { // If not connected this will fail anyway
            digitalWrite(GSMLED, LOW);
            Serial.println(F("Positive sendData call."));
            // Probably we have connection, try to empty the buffer
            // At the same time the function must return for another loop
            if (sendBufferEntries(DATARATE - constrain(millis() - loopTimer, 0, DATARATE)))
                Serial.println(F("Positive sendBufferEntries call."));
            else
                Serial.println(F("Negative sendBufferEntries call."));
        }
        else {
            digitalWrite(GSMLED, HIGH); // Signal disconnection
            Serial.println(F("Negative sendData call."));
            if (storeInBuffer()) {
            	digitalWrite(SDLED, LOW);
                Serial.println(F("Positive storeInBuffer call."));
            }
            else {
            	digitalWrite(SDLED, HIGH);
                Serial.println(F("Negative storeInBuffer call."));
            }
        }
    } else {
        digitalWrite(GPSLED, HIGH);
        waitGPSFix();
        digitalWrite(GPSLED, LOW);
    }

    Serial.print(F("End loop with millis left: "));
    Serial.println(DATARATE - constrain(millis() - loopTimer, 0, DATARATE));
    Serial.println(F("++++++++++++++++\n"));
    digitalWrite(LOOPLED, LOW);

    // Repeat keeping the GPS active, maintaining a constant frequency
    smartDelay(DATARATE - constrain(millis() - loopTimer, 0, DATARATE));
}


