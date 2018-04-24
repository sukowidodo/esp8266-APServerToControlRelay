/* Create a WiFi access point and provide a web server on it. */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
IPAddress    apIP(192, 168, 43, 114);
IPAddress    gw(192, 168, 4, 1); // Defining a static IP address: local & gateway
                                    // Default IP in AP mode is 192.168.4.1

/* This are the WiFi access point settings. Update them to your likin */
const char *ssid = "SUKOWI";
const char *password = "SUKOWIDODO";
const char *imei = "357700061240713";

uint8_t relay[2]= {D1,D2};

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

void handleGenericArgs() { //Handler
                           //Add a new line
String message="{";
int last = (server.args()-1);
//for (int i = 0; i < server.args(); i++) {
  
  if(server.arg(0)==(String)relay[0] && server.arg(1)=="0"){
    digitalWrite (relay[0], HIGH );
    Serial.print((String)relay[0]+" high");
  }else if(server.arg(0)==(String)relay[0] && server.arg(1)=="1"){
    digitalWrite (relay[0], LOW );
    Serial.print((String)relay[0]+" low");
  }
  else if(server.arg(0)==(String)relay[1] && server.arg(1)=="0"){
    digitalWrite (relay[1], HIGH );
    Serial.print((String)relay[0]+" high");
  }else if(server.arg(0)==(String)relay[1] && server.arg(1)=="1"){
    digitalWrite (relay[1], LOW );
    Serial.print((String)relay[1]+" low");
  }

  message += "\"name\" :" + server.arg(0) + " ,";   
  message += "\"status\":"+server.arg(1) + "";             
//  if(i!=last){
 //   message +=",";
 // }
//} 
message+="}";

server.send(200, "application/json", message);       //Response to the HTTP request

}

void handleNotFound() {
  digitalWrite ( LED_BUILTIN, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( LED_BUILTIN, 1 ); //turn the built in LED on pin DO of NodeMCU off
}

void setupRelayIdiot(){
   for(int i=0;i<sizeof(relay);i++){
    pinMode(relay[i],OUTPUT);
    digitalWrite(relay[i],HIGH);
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  /**Setup GPIO**/
  setupRelayIdiot();

  /** Setup DNS **/
    // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(100);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  //dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNS_PORT, "*", apIP);
  
  server.on("/args", handleGenericArgs); //Associate the handler function to the path
  server.on ( "/inline", []() {
    server.send ( 200, "text/html", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
}



void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
