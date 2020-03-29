/* include libraries in project */
#include <WiFi.h>
#include <Servo.h>
#include <SPI.h>

const int SERIAL_SPEED = 9600;

const int PIN = 13;
const int MIN_ANGLE = 0;
const int MAX_ANGLE = 15;
const int ROT_SPEED = 30;

const int PORT = 80;
const char* SSID = "";
const char* PW = "";


/* create objects */
Servo motor;
WiFiServer server(PORT);

/* run once */
void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.println("Serial: Port connected");

  motor.attach(PIN);
  Serial.println("Servo: Attached to pin " + String(PIN));
  Serial.println("Servo: Setting initial position " + String(MIN_ANGLE));
  angle(MAX_ANGLE);
  angle(MIN_ANGLE);

  Serial.println("Wifi: Setting up server");
  WiFi.begin(SSID, PW);
  Serial.println("Wifi: Waiting for connection state");
  while (WiFi.status() != WL_CONNECTED);
  Serial.println("Wifi: Connected to network " + String(WiFi.SSID()));
  Serial.print("Wifi: Connect to server via IP ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println();
  Serial.println("Setup completed successfully");
  delay(2000);
  Serial.println("Wifi: Listening for client");
  Serial.println();
}

/* run repeatedly */
void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = readRequest(client);
    Serial.println("Wifi: Client requested " + request);
    if (request.equals("/")) {
      sendHTML(client);
      Serial.println("Wifi: Sent HTML page");
      Serial.println();
    } else if (request.equals("/control/left") || request.equals("/control/right")) {
      int control = (request.equals("/control/left")) ? MAX_ANGLE : MIN_ANGLE;
      angle(control);
      sendHeader(client, 204);
      Serial.println("Servo: Position set to " + String(control));
      Serial.println();
    } else {
      sendHeader(client, 400);
    }
    client.stop();
  }
}

/* helper functions */
void angle(int degree) {
  if (degree >= MIN_ANGLE && degree <= MAX_ANGLE) {
    for (int pos = motor.read(); pos != degree; ) {
      if (pos > degree) {
        pos--;
      } else if (pos < degree) {
        pos++;
      }
      motor.write(pos);
      delay(ROT_SPEED);
    }
  }
}

String readRequest(WiFiClient client) {
  String currentLine = "";
  String requestLine = "/";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n') {
        if (currentLine.length() == 1) {
          return requestLine;
        } else {
          if (currentLine.startsWith("GET")) {
            int startPos = String("GET ").length();
            int endPos = currentLine.lastIndexOf(" ");
            requestLine = currentLine.substring(startPos, endPos);
          }
          currentLine = "";
        }
      } else {
        currentLine += c;
      }
    }
  }
}

void sendHeader(WiFiClient client, int code) {
  switch (code) {
    case 200:
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println();
      break;
    case 204:
      client.println("HTTP/1.1 204 No Content");
      client.println();
      break;
    case 400:
      client.println("HTTP/1.1 400 Bad Request");
      client.println("Content-type:text/html");
      client.println();
      client.println("Requested ressource is not handled by server");
      client.println();
      break;
  }

}

void sendHTML(WiFiClient client) {
  sendHeader(client, 200);
  client.println("<!DOCTYPE html><html>");
  client.println("<head><title>Thomas</title></head>");
  client.println("<body><a href=\"control/left\">left</a><br><a href=\"control/right\">right</a></body>");
  client.println("</html>");
  client.println();
}
