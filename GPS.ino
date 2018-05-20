void waitGPSFix() {
    while (!gps.location.isValid() || !gps.course.isValid() || !gps.speed.isValid() || !gps.date.isValid() || !gps.time.isValid()) {
        smartDelay(1000);
    }
}

bool getFormattedData() {
    // Checking if there is valid data to get
    if (!gps.location.isValid() || !gps.course.isValid() || !gps.speed.isValid() || !gps.date.isValid() || !gps.time.isValid())
        return false;

	data [0] = '\0'; // Empty buffer
	char buffer[15];
	
	// Latitude
    dtostrf(gps.location.lat(), 6, 4, buffer);
    strcat(data, "?lat=");
    strcat(data, buffer);
	// Longitude
    dtostrf(gps.location.lng(), 6, 4, buffer);
    strcat(data, "&lng=");
    strcat(data, buffer);
	// Course
    dtostrf(gps.course.deg(), 3, 1, buffer);
    strcat(data, "&course=");
    strcat(data, buffer);
	// Speed
    dtostrf(gps.speed.kmph(), 4, 2, buffer);
    strcat(data, "&speed=");
    strcat(data, buffer);

    // Date
    strcat(data, "&date=");
    // Year
    itoa(gps.date.year(), buffer, 10);
    strcat(data, buffer);
    strcat(data, "-");
	// Month
    itoa(gps.date.month(), buffer, 10);
    strcat(data, buffer);
    strcat(data, "-");
	// Day
    itoa(gps.date.day(), buffer, 10);
    strcat(data, buffer);
    strcat(data, "+"); // Space URL encoded
	// Hour
    itoa(gps.time.hour(), buffer, 10);
    strcat(data, buffer);
    strcat(data, ":");
	// Minute
    itoa(gps.time.minute(), buffer, 10);
    strcat(data, buffer);
    strcat(data, ":");
	// Second
    itoa(gps.time.second(), buffer, 10);
    strcat(data, buffer);

    return true;
}

// This custom version of delay() ensures that the gps object is being "fed"
static void smartDelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (Serial1.available())
            gps.encode(Serial1.read());
    } while (millis() - start < ms);
}


