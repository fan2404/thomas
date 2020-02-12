/* include libraries in project */
#include <WiFi.h>
#include <Servo.h>
#include <SPI.h>

const int SERIAL_SPEED = 9600;

const int PIN = 13;
const int MIN_ANGLE = 0;
const int MAX_ANGLE = 90;

const int PORT = 80;
char ID[] = "";
char PW[] = "";
const int CONNECTING_STATE = WL_IDLE_STATUS;
const int CONNECTED_STATE = WL_CONNECTED;


/* create objects */
Servo motor;
WiFiServer server(PORT);
WiFiClient client;

/* run once */
void setup() {
  Serial.begin(SERIAL_SPEED);
  debug("Serial: Port connected");

  debug("Servo: Attaching to pin " + PIN);
  motor.attach(PIN, MIN_ANGLE, MAX_ANGLE);
  if (!motor.attached()) {
    debug("Servo: Failed to attach");
    while (true);
  }
  debug("Servo: Attached");
  if (motor.read() != MIN_ANGLE) {
    debug("Servo: Setting initial position");
    motor.write(MIN_ANGLE);
  }

  debug("Wifi: Setting up server");
  WiFi.begin(ID, PW);
  debug("Wifi: Waiting for connection state");
  while (WiFi.status() == CONNECTING_STATE);
  if (WiFi.status() != CONNECTED_STATE) {
    debug("Wifi: Resolved to state " + WiFi.status());
    while (true);
  }
  debug("Wifi: Connected to network " + String(WiFi.SSID()));
  debug("Wifi: Connect to server via IP " + WiFi.localIP());
  debug(ln() + "Setup completed successfully" + ln());
  delay(2000);
}

/* run repeatedly */
void loop() {
  debug("Wifi: Listening for client");
  client = server.available();
  if (client) {
    debug("Wifi: Client connected");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE html><html>");
    client.println("<head><title>Thomas</title></head>");
    client.println("<body>yay</body>");
    client.println("</html>");
    client.println();
    debug("Wifi: Sent HTML page");
    client.stop();
    debug("Wifi: Connection with client closed" + ln());
  }
}

/* helper functions */
void debug(String message) {
  Serial.println(message);
}

String ln() {
  return "\r\n";
}
