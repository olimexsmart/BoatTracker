/*
    Web client

    This sketch connects to a website through a GSM shield. Specifically,
    this example downloads the URL "http://www.arduino.cc/asciilogo.txt" and
    prints it to the Serial monitor.

    Circuit:
    GSM shield attached to an Arduino
    SIM card with a data plan

    created 8 Mar 2012
    by Tom Igoe

    http://www.arduino.cc/en/Tutorial/GSMExamplesWebClient

*/

// libraries
#include <GSM.h>
//#include <SD.h>

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "wap.tim.it" // replace your GPRS APN
#define GPRS_LOGIN     "WAPTIM"    // replace with your GPRS login
#define GPRS_PASSWORD  "WAPTIM" // replace with your GPRS password

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// URL, path & port (for example: arduino.cc)
char server[] = "arduino.cc";
char path[] = "/asciilogo.txt";
int port = 80; // port 80 is the default for HTTP

void setup() {
    // initialize serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.println(F("Starting Arduino web client."));
    Serial.println(FreeRam());
    // connection state
    boolean notConnected = true;

    // After starting the modem with GSM.begin()
    // attach the shield to the GPRS network with the APN, login and password
    while (notConnected) {
        if ((gsmAccess.begin() == GSM_READY) &
                (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
            notConnected = false;
        } else {
            Serial.println(F("Not connected"));
            delay(1000);
        }
    }

    Serial.println(F("connecting..."));

    // if you get a connection, report back via serial:
    if (client.connect(server, port)) {
        Serial.println("connected");
        // Make a HTTP request:
        client.print("GET ");
        client.print(path);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println();
    } else {
        // if you didn't get a connection to the server:
        Serial.println("connection failed");
    }
}

void loop() {
    // if there are incoming bytes available
    // from the server, read them and print them:
    if (client.available()) {
        char c = client.read();
        Serial.print(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.available() && !client.connected()) {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop();        
        // do nothing forevermore:
        for (;;)
            ;
    }
}

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
