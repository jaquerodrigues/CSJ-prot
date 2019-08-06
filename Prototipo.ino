/*
 * Esse código irá imprimir o endereço IP do NodeMCU (uma vez que é conectado a rede) no monitor serial. 
 * Então, é possível abrir esse endereço em um navegador web e ver as notificações.
 * O NodeMCU funciona como um receptor do sistema.
 * 
 * Esse exemplo é escrito para uma rede usando WEP.
 * 
 * Circuito:
 * Filamento interligado ao ArduinoUno(pino 13) e ao NodeMCU(pino 18).
 */

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const int   pinoFilamento        = 18;
const char* ssid                 = "teste";
const char* password             = "pf123";
const long  utcOffsetInSeconds   = 3600;

char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds); /*Link para chegar */
WiFiServer server(80);

void setup(){

    pinMode(pinoFilamento,INPUT);
    Serial.begin(115200);    
    delay(10);

    // We start by connecting to a WiFi network

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
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    timeClient.begin();
    server.begin();
}

int filamento = 0;
int value = 0;

void loop(){
   timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime());

  delay(1000);
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            
            // auto reload webpage every 5 second
            client.println("<META HTTP-EQUIV=REFRESH CONTENT=5 URL=>");
            
            // webpage title
            client.println("<center><p><h1>Arduino + NodeMCU  Application </h1></p><center><hr><br />");
            client.println("<center><p><h3>Login Autorizado </h3></p><center><hr><br />");
            
            client.println("<img border=\"0\" src=\"data:image/jpeg;base64,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\" /><br />");
                        
            filamento = digitalRead(pinoFilamento);

            if (filamento == LOW){
            //okk funcionou para 192.168.0.101
            Serial.println("Tentativa de desconexão do cabo.");
            client.println("<center><p><h2><font color=red>TENTATIVA DE DESCONEXAO DE CABO");
            client.println(daysOfTheWeek[timeClient.getDay()]);
            client.print(", ");
            client.print(timeClient.getHours());
            client.print(":");
            client.print(timeClient.getMinutes());
            client.print(":");
            client.println(timeClient.getSeconds());
            client.println("</font></h2></p>");
            }
            
            else if (filamento == HIGH){
            Serial.println("Cabo esta conectado");
            client.println("<center><p><h2><font color=green>CABO ESTA CONECTADO");
            client.println(daysOfTheWeek[timeClient.getDay()]);
            client.print(", ");
            client.print(timeClient.getHours());
            client.print(":");
            client.print(timeClient.getMinutes());
            client.print(":");
            client.println(timeClient.getSeconds());
            client.println("</font></h2></p>");
            }
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
