/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

const char *ssid = "Pigpig"; // wifi_name
const char *password = "88888888";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  delay(10);

  // prepare LED
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  Serial.println("new client");
  while (!client.available())
  {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  if (req.indexOf(F("/gpio/0")) != -1)
  {
    val = 0;
  }
  else if (req.indexOf(F("/gpio/1")) != -1)
  {
    val = 1;
  }
  else
  {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  // web
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");

  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<style>");
  client.println("button{");
  client.println("background-color:#bfa;");
  client.println("width:250px;");
  client.println("height:250px;");
  client.println("border:2px solid black");
  client.println("margin-left:50%;");
  client.println("margin-right:50%;");
  client.println("}");
  client.println("</style>");
  client.println("</head>");
  client.print("LED status: ");

  if (val == 1)
  {
    client.print("ON");
  }
  else
  {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("<button><a href=\"/gpio/1\">TURN ON</a></button>");
  client.println("<button><a href=\"/gpio/0\">TURN OFF</a></button>");
  client.println("</html>");
  Serial.println("");

  // Set LED according to the request
  digitalWrite(2, val);
  client.flush();
  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below

  while (client.available())
  {
    // byte by byte is not very efficient
    client.read();
  }

  delay(1);
  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Client disconnect!"));
}
