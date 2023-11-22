unsigned long startTime; // Variable to store the start time

void setup() {
  Serial.begin(9600); // Initialize serial communication
  startTime = millis(); // Store the current time as the start time
}

void loop() {
  unsigned long elapsedTime = millis() - startTime; // Calculate elapsed time

  // Convert milliseconds to HH:MM:SS format
  int hours = elapsedTime / 3600000;
  int minutes = (elapsedTime % 3600000) / 60000;
  int seconds = (elapsedTime % 60000) / 1000;

  // Print the elapsed time in HH:MM:SS format
  Serial.print(hours < 10 ? "0" : "");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes < 10 ? "0" : "");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds < 10 ? "0" : "");
  Serial.println(seconds);

  delay(1000); // Wait for 1 second
}
