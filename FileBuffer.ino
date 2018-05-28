bool storeInBuffer() {
    File storage = SD.open(FILEBUFF, FILE_WRITE);
    if (!storage)
        return false;

    if (storage.println(data) != strlen(data)) {
        Serial.println("Mismatch in written data");
        storage.close();
        return false;
    }
    storage.close();
    return true;
}

// Send one entry to the server
bool sendBufferEntries(unsigned long window) {
    unsigned long timer = millis(); // Keep track of the time we spent in the function

    // Check if buffer is present
    if (!SD.exists(FILEBUFF))
        return true;	// Nominal, nothing to write

    File storage = SD.open(FILEBUFF, FILE_READ);
    if (!storage)
        return false; // Error in opening

    int index = 0;
    // Remove 3000 as it's a worst case scenario for GSM data sending time. Cast to prevent underflow
    while (storage.available() && ((long) (millis() - timer - 3000) > (long) window)) { // Loop on lines until we have some time left, with some margin
        char c;
        while ((c = storage.read()) != '\n') { // Once line at a time
            data[index] = c;
            index++;
            if (index == BUFFLEN || !storage.available()) { // Don't overflow
                // Consider deleting file here for a fresh start
                return false;	// Error buffer overflow of EOF
            }
            data[index] = '\0'; // \n at the end would be a problem
        }

        // Once we got the line check if was not already sent
        if (strstr(data, "###"))
            continue; // Line was already sent, got to the next one

        // Send data via GSM
        if (sendData()) {
            // Get current position to reopen file in write mode
            unsigned long pos = storage.position();
            storage.close(); // Close in read mode
            storage = SD.open(FILEBUFF, FILE_WRITE);
            if (!storage)
                return false; // Probably also here delete file a good idea
            storage.seek(pos - 5); // 3 chars before \n
            storage.println("###"); // Invalidating line, the \n is overwritten
            storage.flush();	// Ensure writing
   /*         Slould be unecessary
    *          // Now open again in read mode 
            pos = storage.position();
            storage.close();
            storage = SD.open(FILEBUFF, FILE_READ);
            if (!storage)
                return false; // Probably also here delete file a good idea
            storage.seek(pos); */
        } else {
            storage.close();
            return false; // Could not send the entry, end stream
        }
    } 
	
    if (!storage.available()) { // Nothing else to read: delete the file
		storage.close();	
		SD.remove(FILEBUFF);
		return true;
    }

   	// Close file and return
	storage.close();	
	return true;
}

