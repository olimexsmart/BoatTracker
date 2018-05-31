void waitGSMFix() {
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
}

bool sendData() {    
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

        Serial.println(F("POST request sent to server."));
    } else {
    	Serial.println(F("ERROR could not connect to server."));
    	client.stop();
    	return false;
    }
        

	unsigned long responseTime = millis();
    while (!client.available()) {
        smartDelay(10);
        if (millis() - responseTime > TIMEOUT) { // No response timeout
        	Serial.println(F("ERROR response from server timeout."));
        	client.stop();
        	return false;        	
        }
    }
	Serial.print(F("Waited for response: "));
	Serial.println(millis() - responseTime);
    
    char response[1000];
    int i = 0;
    while (client.available()) {
        response[i++] = client.read();
        if (i == 999) {
            Serial.println(F("ERROR response too large for buffer: probably error returned."));
            client.stop();
            return false;            
        }
    }
    response[i] = '\0';

    client.stop();
    Serial.println(response);
    if (strstr(response, "OK")) {
    	Serial.println(F("Got positive response from server."));
    	return true;
    } else {
		Serial.println(F("ERROR got negative response from server."));
    	return false;
    }
        

    
}

