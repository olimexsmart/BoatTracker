/*


*/

// libraries
#include <GSM.h>
//#include <SD.h>
#include <TinyGPS++.h>

// PIN Number
#define PINNUMBER 		""
#define GPRS_APN       	"wap.tim.it"
#define GPRS_LOGIN     	"WAPTIM"
#define GPRS_PASSWORD  	"WAPTIM"
#define RXPin 			11
#define TXPin 			12
#define GPSBaud 		57600
#define port			80
#define server			"olimexsmart.it"

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;
TinyGPSPlus gps;

char data[256];

/*
    SETUP
*/
void setup() {
    // initialize serial communications and wait for port to open:
    Serial.begin(115200);
    Serial1.begin(GPSBaud);

    Serial.println(F("Starting Boat Tracker."));
    Serial.println(FreeRam());

    // Wait for GPS signal to become valid
    waitGPSFix();
    Serial.println("Got GPS fix");

    // Wait for GSM to be connecter to network
    waitGSMFix();
    Serial.println("Got GSM connection");

    Serial.println(F("Starting main loop"));
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
    // Get GPS data into URL encoded string
    if (getFormattedData()) {
        // Send to server
        Serial.println(data);
        if (sendData())
            Serial.println(F("Connected to olimexsmart"));
        else
            Serial.println(F("Connection failed"));
    } else {
        waitGPSFix();
    }

    // Repeat keeping the GPS active
    smartDelay(5000);
}


/*
    UTILITIES
*/
int FreeRam(void) {
    extern int  __bss_end;
    extern int* __brkval;
    int free_memory;
    if (reinterpret_cast<int>(__brkval) == 0) {
        // if no heap use from end of bss section
        free_memory = reinterpret_cast<int>(&free_memory)
                      - reinterpret_cast<int>(&__bss_end);
    } else {
        // use from top of stack to heap
        free_memory = reinterpret_cast<int>(&free_memory)
                      - reinterpret_cast<int>(__brkval);
    }
    return free_memory;
}
