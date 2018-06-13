bool storeInBuffer() {
    File storage = SD.open(FILEBUFF, FILE_WRITE);
    if (!storage)
        return false;

    // Can't use prinln directly because it appends also a \r which rouins things the on the HTTP side
    strcat(data, "\n");
    if (storage.print(data) != strlen(data)) {
        printDebug("ERROR Mismatch in written data.\n");
        storage.close();
        return false;
    }

    printDebug(F("Data correctly stored.\n"));
    storage.close();
    return true;
}

// Send one entry to the server
bool sendBufferEntries(unsigned long window) {
    unsigned long timer = millis(); // Keep track of the time we spent in the function
    window -= 5000; // Remove 5000 as it's a worst case scenario for GSM data sending time

    // Check if buffer is present
    if (!SD.exists(FILEBUFF)) {
        printDebug(F("File does not exists, no data stored.\n"));
        return true;	// Nominal, nothing to send
    }

    File storage = SD.open(FILEBUFF, FILE_READ);
    if (!storage) {
        printDebug(F("ERROR in opening buffer file\n"));
        return false; // Error in opening
    }


    while (storage.available() && millis() - timer < window) { // Loop on lines until we have some time left, with some margin
        char c;
        int index = 0;
        while ((c = storage.read()) != '\n') { // Once line at a time
            data[index] = c;
            index++;
            if (index == BUFFLEN || !storage.available()) { // Don't overflow
                printDebug(F("ERROR buffer overflow when reading file buffer entry. Deleting file.\n"));
                storage.close();
                SD.remove(FILEBUFF); // Deleting file here for a fresh start
                return false;	// Error buffer overflow of EOF
            }
            data[index] = '\0'; // \n at the end would be a problem
        }
        printDebug(data);
        // Once we got the line check if was not already sent
        if (strstr(data, "###"))
            continue; // Line was already sent, got to the next one

        // Add 'buffered' flag
        strcat(data, "&buffered=yo");

        // Send data via GSM
        if (sendData()) {
            // Get current position to reopen file in write mode
            unsigned long pos = storage.position();
            storage.close(); // Close in read mode
            storage = SD.open(FILEBUFF, FILE_WRITE);
            if (!storage) {
                SD.remove(FILEBUFF); // Also here delete file trying to reset the problem
                return false;
            }
            storage.seek(pos - 5); // 5 chars before \n
            storage.println("###"); // Invalidating line, the \n is overwritten
            storage.flush();	// Ensure writing
            printDebug(F("Data sent from stored buffer and correctly invalidated.\n"));
        } else {
            printDebug(F("ERROR in sendind data from stored buffer.\n"));
            storage.close();
            return false; // Could not send the entry, end stream
        }
    }

    if (!storage.available()) { // Nothing else to read: delete the file
        printDebug(F("Reached end of file, file deleted.\n"));
        storage.close();
        SD.remove(FILEBUFF);
        return true;
    }

    // Close file and return
    printDebug(F("Not all entries sent in time, file not deleted.\n"));
    storage.close();
    return true;
}

