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
#define DATARATE		10000	// Send point every N millisecons
#define TIMEOUT			5000	// Connection timeout for response from server
#define SD_CS			11		// SPI SD CS on pin 11

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;
TinyGPSPlus gps;

char data[256];
unsigned long loopTimer;

/*
    SETUP
*/
void setup() {
    // initialize serial communications and wait for port to open:
    Serial.begin(115200);
    Serial1.begin(GPSBaud);

    Serial.println(F("Starting Boat Tracker."));
    Serial.print(F("Free RAM bytes: "));
    Serial.println(FreeRam());
    Serial.println(F("Waiting for GPS fix..."));

    // Wait for GPS signal to become valid
    waitGPSFix();
    Serial.println(F("Got GPS fix.\nWaiting for GSM connectivity..."));

    // Wait for GSM to be connecter to network
    waitGSMFix();
    Serial.println(F("Got GSM connection.\nInitializing SD card..."));

    if (!SD.begin(SD_CS)) {
        Serial.println("SD card initialization failed!");
        return;
    }

    Serial.println(F("SD card OK\nStarting main loop."));
}


/*
    LOOP
*/
void loop() {
    /*
        Some stuff will change when the SD card will be added.
        If the connection to the server fails, the data will be saved into the memory card.
        Whenever the connection is re-estabilished, all the data stored will be sent.
    */
    loopTimer = millis();
    // Get GPS data into URL encoded string
    if (getFormattedData()) {
        // Send to server
        Serial.println(data);
        if (sendData())
            Serial.println(F("Data correctly sent"));
        else
            Serial.println(F("Connection failed"));
    } else {
        waitGPSFix();
    }

    // Repeat keeping the GPS active, maintaining a constant frequency
    smartDelay(DATARATE - (millis() - loopTimer));
}


