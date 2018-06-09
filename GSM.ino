void waitGSMFix() {
	unsigned long timeout = millis();
    // After starting the modem with GSM.begin()
    // attach the shield to the GPRS network with the APN, login and password
    // Give it a 3 minute timeout
    while (GSMnotInit && millis() - timeout < 180000) {
        if ((gsmAccess.begin() == GSM_READY) &
                (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
            GSMnotInit = false;
        } else {
            Serial.println(F("Not connected"));
            delay(1000);
        }
    }
}

bool sendData() {   
	if (GSMnotInit) { // Don't even try
		Serial.println(F("ERROR module not correctly initialized, could not send"));		
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

        Serial.println(F("POST request sent to server."));
    } else {
    	Serial.println(F("ERROR could not connect to server."));    	
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
    if (strstr(response, "ROGER")) {
    	Serial.println(F("Got positive response from server."));
    	return true;
    } else {
		Serial.println(F("ERROR got negative response from server."));
    	return false;
    }
        

    
}

