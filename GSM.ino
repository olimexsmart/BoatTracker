void waitGSMFix(bool restart) {
    unsigned long timeout = millis();
    // After starting the modem with GSM.begin()
    // attach the shield to the GPRS network with the APN, login and password
    // Give it a 3 minute timeout
    while (GSMnotInit && millis() - timeout < DATARATE) {
        if (gsmAccess.begin("", restart) == GSM_READY) {
            if (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY) {
                GSMnotInit = false;
                printDebug(F("GSM correctly connected.\n"));
            }
        } else {
            printDebug(F("ERROR GSM failed to connect.\n"));
            delay(1000);
        }
    }
}

bool sendData() {
    if (GSMnotInit) { // Don't even try
        printDebug(F("ERROR module not correctly initialized, could not send.\n"));
        return false;
    }
    // Send data to the database
    if (client.connect(server, port)) {
        // Make a HTTP request:
        client.print(F("GET /tracker/LogPoint.php"));
        client.print(data);
        client.println(F(" HTTP/1.1"));
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println();

        printDebug(F("\nPOST request sent to server.\n"));
    } else {
        printDebug(F("\nERROR could not connect to server.\n"));
        return false;
    }


    unsigned long responseTime = millis();
    while (!client.available()) {
        smartDelay(10);
        if (millis() - responseTime > TIMEOUT) { // No response timeout
            printDebug(F("ERROR response from server timeout.\n"));
            client.stop();
            return false;
        }
    }
    printDebug(F("Waited for response: "));
    printDebug(itoa(millis() - responseTime, data, 10));

    char response[1000];
    int i = 1;
    response[0] = '\n'; // This way in the debug print it gets a newline
    while (client.available()) {
        response[i++] = client.read();
        if (i == 999) {
            printDebug(F("\nERROR response too large for buffer: probably error returned.\n"));
            client.stop();
            return false;
        }
    }
    response[i] = '\0';

    client.stop();
    printDebug(response);
    printDebug(F("Response lenght: "));
    printDebug(itoa(i - 1, data, 10));
    if (strstr(response, "ROGER")) {
        printDebug(F("\nGot positive response from server.\n"));
        return true;
    }

    printDebug(F("\nERROR got negative response from server.\n"));
    return false;
}

