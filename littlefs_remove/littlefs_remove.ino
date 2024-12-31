#include <LittleFS.h>

void setup() {
    Serial.begin(115200);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return;
    }
    Serial.println("LittleFS mounted successfully");

    // Check if the file exists
    if (LittleFS.exists("/index.html")) {
        Serial.println("File exists. Deleting...");

        // Remove the file
        if (LittleFS.remove("/index.html")) {
            Serial.println("File deleted successfully");
        } else {
            Serial.println("Failed to delete the file");
        }
    } else {
        Serial.println("File does not exist.");
    }

    // End LittleFS
    LittleFS.end();
}

void loop() {
    // Nothing here
}
