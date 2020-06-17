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
  client.println("<!DOCTYPE html><html><head><title>Thomas🚂💨</title><meta name="viewport" content="width=device-width,initial-scale=1"><style>body{height:100vh;margin:0;display:flex;flex-direction:column;justify-content:stretch;}.button{display:flex;width:7rem;height:5rem;border:.5rem outset gray;border-radius:1rem;background-color:white;}.button-icon{display:inline-block;background-image:url(https://f0.pngfuel.com/png/346/433/rail-transport-train-steam-locomotive-computer-icons-train-png-clip-art.png);background-size:contain;background-position:center;background-repeat:no-repeat;width:inherit;height:inherit;z-index:-1;}.button:nth-of-type(2){margin-left:1rem;}.button:active{border-style:inset;}.flip{transform:scaleX(-1);}.control{display:flex;justify-content:center;margin:3rem 1rem 0;}.pane{background-color:#e6e6e600;opacity:0.5;width:100%;height:100%;}.info{display:flex;justify-content:center;}.fact{font-family:"Comic Sans MS";text-align:center;opacity:1;transition:opacity 1.5s ease-in-out;margin:0 1rem;flex-shrink:1;}.fade{opacity:0;}main,.info,.control-wrapper{display:flex;flex-direction:column;}main{justify-content:stretch;flex:1;background-color:#e8f4f7;}h1,h2{text-align:center;display:block;margin:0;font-family:"Comic Sans MS";padding-top:1rem;}.control-wrapper{flex:1;justify-content:center;padding-bottom:3rem;}.info{height:20vh;justify-content:flex-start;border-top:.1rem solid black;display:flex;}header{background-color:lightblue;border-bottom:.1rem solid black;padding-bottom:1rem;}.thomas{height:30vh;background-image:url(https://img.gta5-mods.com/q95/images/thomas-the-tank-engine-nsh3t/e22045-1.1c.jpg);background-position:50% 50%;background-repeat:no-repeat;background-size:30rem;}</style><script type="text/javascript">const facts=["In Island gibt es keine Eisenbahn.","Die Transsibirische Eisenbahn ist mit 9288 km die längste Eisenbahnstrecke der Welt und verbindet mehr als 400 Bahnhöfe. Sie ist die Hauptverkehrsachse Russlands.","Am 07.12.1835 fuhr zwischen Nürnberg und Fürth die erste deutsche Eisenbahn.","Die Spurweite einer Normalspur beträgt 1435mm (Europa, Nordamerika, Mexiko, China) - Schmalspurbahnen sind zwischen 300 und 600 mm breit.","Die Deutsche Bahn hat über 270 ICE-Züge in ihrer Flotte.","Rund 216.000 Menschen sind pro Tag im ICE unterwegs.","„Verkehrsrot“ und „Lichtgrau“ - so heißen die Farben, in denen ICE-Züge lackiert sind, in ihrer offiziellen Bezeichnung.","2004 fuhr der längste Modellzug mit 103 Metern und mehr als 500 Wagons im Miniatur Wunderland Hamburg mehr als 231 Meter weit.","2007 wurde ein ICE-Wagon mit einem Gewicht von fast 50 Tonnen von 200 Modell-Lokomotiven 10 Meter weit gezogen.","Die Londoner U-Bahn war die erste U-Bahn der Welt, sie wurde 1863 eröffnet. 1900 und 1905 folgten Paris und New York."];function newRandom(max,prev){let num;do{num=Math.floor(Math.random()*Math.floor(max));}while(num===prev);return num;}window.onload=()=>{const element=document.getElementById('fact');let previous;function loadFact(){const next=newRandom(facts.length,previous);previous=next;element.classList.toggle('fade');setTimeout(()=>{element.innerText=facts[next];element.classList.toggle('fade');},2500);}loadFact();const load=setInterval(loadFact,10000);};</script></head><body><header><h1>Thomas 🚂💨</h1></header><main><div class="thomas"></div><div class="control-wrapper"><h2>Weichenposition:</h2><div class="control"><a href="control/left" class="button"><div class="pane"><span class="button-icon"></div></span></a><a href="control/right" class="button"><div class="pane"><span class="button-icon flip"></div></span></a></div></div><div class="info"><h2>Random train fact:</h2><p id="fact" class="fact"></p></div></main></body></html>");
  client.println();
}
