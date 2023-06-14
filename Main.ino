#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = "b";
const char* password = "intergouvernementalisme";
float[] temperature = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float[] humidity = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

WiFiServer server(80);

void setup() {
  
    // Start DHT11
    dht.begin();
    Serial.begin(115200);
    delay(10);
    Serial.println();

    // Connect to WiFi network
    WiFi.mode(WIFI_STA);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());
}

void loop() {

    float h = dht.readHumidity();
    float t = dht.readTemperature(); // Read temperature as Celsius (the default)

    //   Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
     Serial.println("Failed to read from DHT sensor!");
     return;
    }

    Serial.print("Humidity % : ");
    Serial.println(h);
    Serial.print("Temperature *C: ");
    Serial.println(t);
    for (int i = 0; i < 9; i++) {
        temperature[i] = temperature[i+1];
        humidity[i] = humidity[i+1];
    }

    WiFiClient client = server.available();
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");  // the connection will be closed after completion of the response
    client.println("Refresh: 5");  // refresh the page automatically every 5 sec
    client.println();
    client.println("<!DOCTYPE html>");
    client.println("<html lang="en"><head>");
    client.println("<meta charset="UTF-8">");
    client.println("<title>Température et humidité locale</title><style>@keyframes background_anim {0% {background-color: mediumpurple;}100% {background-color: rebeccapurple;}}body {font-family: sans-serif;font-weight: bold;text-align: center;animation: background_anim 10s infinite alternate;color: aliceblue;}h1 {text-shadow: 2px 2px 2px #000000;}</style></head><body><h1>Température et humidité locale</h1>");
    client.println("<p>Température : °C</p>");
    client.println("<canvas id="chartTemperature" width="160" height="110"></canvas><br/>");
    client.println("<p>Humidité : %</p>");
    client.println("<canvas id="chartHumidity" width="160" height="110"></canvas></body>");
    client.print("<script>const temp = [");

    for (int i = 0; i < 10; i++) {
        client.print(temperature[i]);
        client.print(",");
    }
    client.println("];");

    client.print("<script>const hum = [");
    for (int i = 0; i < 10; i++) {
        client.print(humidity[i]);
        client.print(",");
    }
    client.println("];");

    client.println("let cTemp = document.getElementById('chartTemperature').getContext('2d');let cHum = document.getElementById('chartHumidity').getContext('2d');class Chart {constructor(ctx, param2, maxY) {this.ctx = ctx;this.param2 = param2;this.maxY = maxY;this.drawAxis();this.drawPoints();}drawAxis() {this.ctx.moveTo(0, 0);this.ctx.lineTo(0, 110);this.ctx.lineTo(160, 110);this.ctx.strokeStyle = 'white';this.ctx.lineWidth = 5;this.ctx.stroke();}drawPoints() {this.ctx.fillStyle = 'white';for (let i = 0; i < this.param2.length; i++) {this.ctx.beginPath();this.ctx.arc((i+1) * 15, (this.maxY - this.param2[i]) + 5, 3, 0, 2 * Math.PI);this.ctx.fill();if (i < 9) {this.ctx.moveTo((i+1) * 15, (this.maxY - this.param2[i]) + 5);this.ctx.lineTo((i+2) * 15, (this.maxY - this.param2[i+1]) + 5);this.ctx.lineWidth = 2;this.ctx.stroke();}}}}new Chart(cTemp, temp, 50);new Chart(cHum, hum, 100);</script>");
    client.println("</html>");
    delay(2000);
}
