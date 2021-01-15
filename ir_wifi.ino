/*
 * WiFi IR Blaster by Buddy Crotty
 *  Use an ESP8266 module or dev board to receive HTTP GET request
 *  and then send IR codes to an attached IR LED based on those requests.
 *  This works best with another web server acting as a front end that 
 *  sends cURL requests based on which buttons are pressed. 
 *  cURL format: http://ESP8266/IRcode
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266mDNS.h>

const char* ssid = "FHome";
const char* password = "1234qwer";
MDNSResponder mdns;

int ir_length = 35;
int khz = 38;

IRsend irsend(D2);

uint16_t power[36] = {8750, 4600, 450, 700, 450, 600, 450, 650, 450, 1800, 450, 650, 450, 1800, 500, 600, 450, 650, 450, 1800, 450, 1800, 500, 1800, 500, 600, 450, 1800, 450, 700, 400, 1800, 450, 1850, 450};
uint16_t power_rgb[36] = {8750, 4650, 450, 600, 500, 1800, 450, 1800, 450, 650, 400, 700, 400, 1850, 500, 600, 450, 600, 450, 1850, 450, 600, 500, 600, 450, 1850, 400, 1850, 400, 700, 450, 1800, 450, 1800};
uint16_t light_increment[36] = {8750, 4600, 450, 1850, 500, 1800, 400, 1850, 400, 650, 450, 700, 400, 1850, 400, 650, 450, 650, 450, 650, 450, 650, 450, 600, 450, 1800, 450, 1850, 450, 650, 450, 1800, 450, 1850, 450};
uint16_t light_decrement[36] = {8750, 4600, 450, 1850, 450, 600, 450, 1850, 450, 600, 500, 1800, 500, 600, 450, 650, 400, 650, 450, 650, 450, 1800, 500, 600, 500, 1750, 450, 650, 450, 1850, 450, 1800, 450, 1850, 450};
uint16_t temp_increment[36] = {8800, 4600, 450, 650, 450, 650, 450, 600, 450, 1800, 500, 1800, 450, 650, 450, 650, 400, 650, 400, 1850, 500, 1800, 450, 1800, 500, 650, 400, 650, 450, 1800, 500, 1800, 450, 1800, 500};
uint16_t temp_decrement[36] = {8750, 4600, 500, 1800, 450, 600, 500, 1800, 500, 550, 500, 1800, 450, 600, 500, 600, 500, 600, 450, 650, 450, 1800, 500, 600, 450, 1800, 500, 600, 450, 1800, 500, 1800, 450, 1800, 450};
uint16_t reset_light_temp[36] = {8750, 4600, 500, 1800, 500, 1750, 500, 1750, 500, 600, 500, 1800, 450, 650, 450, 600, 450, 650, 450, 600, 500, 650, 450, 600, 500, 1750, 500, 600, 450, 1850, 450, 1800, 450, 1850, 450};
uint16_t off_timer[36] = {8800, 4600, 450, 650, 450, 600, 500, 600, 450, 1800, 500, 600, 450, 650, 450, 600, 500, 600, 450, 1850, 450, 1800, 500, 1800, 450, 600, 500, 1800, 450, 1850, 450, 1800, 450, 1800, 500};
uint16_t night_mode[36] = {8750, 4600, 500, 1800, 500, 600, 450, 600, 500, 600, 450, 1850, 450, 650, 450, 600, 450, 650, 450, 600, 450, 1850, 500, 1750, 500, 1800, 450, 650, 450, 1800, 450, 1850, 450, 1800, 450};
uint16_t change_temp[36] = {8750, 4600, 500, 1800, 500, 600, 450, 600, 500, 600, 450, 1850, 450, 650, 450, 600, 450, 650, 450, 600, 450, 1850, 500, 1750, 500, 1800, 450, 650, 450, 1800, 450, 1850, 450, 1800, 450};
uint16_t change_rgb_color[36] = {8750, 4600, 500, 600, 500, 600, 500, 1750, 500, 600, 450, 650, 450, 600, 500, 600, 500, 600, 500, 1750, 450, 1850, 500, 600, 450, 1800, 450, 1800, 500, 1800, 450, 1800, 500, 1800, 450};
uint16_t rgb_auto_mode[36] = {8800, 4600, 450, 600, 500, 1800, 450, 650, 450, 600, 450, 650, 450, 650, 450, 600, 500, 600, 450, 1800, 450, 650, 450, 1850, 450, 1800, 450, 1850, 450, 1800, 450, 1800, 500, 1800, 450};

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(80);


void setup() {
  Serial.begin(115200);
  delay(10);

  irsend.begin();
  
  // Connect to WiFi network
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
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("IRBlasterLR", WiFi.localIP())) {
    Serial.println("MDNS Responder Started");
  }

  Serial.println();
  Serial.println();

  server.on("/power", HTTP_GET, []() {
    irsend.sendRaw(power, ir_length, khz);   
    Serial.println("IR power sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/power_rgb", HTTP_GET, []() {
    irsend.sendRaw(power_rgb, ir_length, khz);   
    Serial.println("IR power_rgb sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/light_increment", HTTP_GET, []() {
    irsend.sendRaw(light_increment, ir_length, khz);   
    Serial.println("IR light_increment sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/temp_decrement", HTTP_GET, []() {
    irsend.sendRaw(temp_decrement, ir_length, khz);   
    Serial.println("IR temp_decrement sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/temp_increment", HTTP_GET, []() {
    irsend.sendRaw(temp_increment, ir_length, khz);   
    Serial.println("IR temp_increment sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/temp_decrement", HTTP_GET, []() {
    irsend.sendRaw(temp_decrement, ir_length, khz);
    Serial.println("IR temp_decrement sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/reset_light_temp", HTTP_GET, []() {
    irsend.sendRaw(reset_light_temp, ir_length, khz);
    Serial.println("IR reset_light_temp sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/off_timer", HTTP_GET, []() {
    irsend.sendRaw(off_timer, ir_length, khz);
    Serial.println("IR off_timer sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/night_mode", HTTP_GET, []() {
    irsend.sendRaw(night_mode, ir_length, khz);
    Serial.println("IR night_mode sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/change_temp", HTTP_GET, []() {
    irsend.sendRaw(change_temp, ir_length, khz);
    Serial.println("IR change_temp sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/change_rgb_color", HTTP_GET, []() {
    irsend.sendRaw(change_rgb_color, ir_length, khz);
    Serial.println("IR change_rgb_color sent");
    server.send(200, "application/json", "ok");
  });

  server.on("/rgb_auto_mode", HTTP_GET, []() {
    irsend.sendRaw(rgb_auto_mode, ir_length, khz);
    Serial.println("IR rgb_auto_mode sent");
    server.send(200, "application/json", "ok");
  });

  // Start the server
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {   
  server.handleClient();
}
