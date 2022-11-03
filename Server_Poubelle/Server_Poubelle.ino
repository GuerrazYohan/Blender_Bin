#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HCSR04.h>

const char* ssid     = "WaterLevel"; //Access point ssid
const char* password = "12341234";   //Access point password

float l = 0.0;
float per = 0.0;
float fcm = 0;
//Rectangular Tank dimentions in cm
float lcm = 200;  //Length
float bcm = 200;    // width
float hcm = 130;  // Height, from Sensor to tank bottom

HCSR04 ultrasonicSensor(D6, D5, 20, 300);
AsyncWebServer server(80);
unsigned long previousMillis = 0;
const long interval = 1000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; Color:DodgerBlue; }
    h3 { font-size: 1.5rem; Color:Tomato; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .temp-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 10px;
    }
  </style>
</head>
<body>
  <h2>Water Tank Level</h2>
  <p>
    <span id="permap">%PERMAP%</span>
    <sup class="units">%</sup> 
  </p>
   <p>
   <span id="liter">%LITER%</span>
    <sup class="units">Liters</sup>
  </p>
      <p>
    <span id="distance">%DISTANCE%</span>
    <sup class="units">Cm</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("permap").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/permap", true);
  xhttp.send();
}, 1000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("liter").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/liter", true);
  xhttp.send();
}, 1000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("distance").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/distance", true);
  xhttp.send();
}, 1000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholders with sensor values
String processor(const String& var){
 
  if(var == "DISTANCE"){
    return String(fcm);
  }
  else if(var == "LITER"){
    return String(l);
  }
  else if(var == "PERMAP"){
    return String(per);
  }
  return String();
}
void setup(){
  Serial.begin(115200);  
  ultrasonicSensor.begin(); 
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println(WiFi.localIP());

  // Route for root
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/permap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(per).c_str());
  });
  server.on("/liter", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(l).c_str());
  });
  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(fcm).c_str());
  });
  server.begin();
} 
void loop()
{  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
     float newdistance = ultrasonicSensor.getMedianFilterDistance(); //pass 3 measurements through median filter, better result on moving obstacles
    if (newdistance != HCSR04_OUT_OF_RANGE)
    {
      Serial.print("sensor raw value:");
      Serial.println(newdistance);
      fcm = hcm - newdistance;
      float v = fcm * lcm * bcm ;
      l = v / 1000;
      per = map(fcm, 0, hcm, 0, 100);
      Serial.println(per);
      Serial.println(l);
      Serial.println(fcm);     
    }
    else 
    {
      Serial.println(F("out of range"));
    }     
  }
}
