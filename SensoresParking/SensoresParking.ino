#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <NewPing.h>
#include <TimerOne.h>
SoftwareSerial SerialESP8266(3,2); // RX, TX

String server = "restlosportales.azurewebsites.net";

const String GET = "GET";
const String POST = "POST";

 
#define SONIDO_NUM     1
#define MAX_DISTANCIA 200 
#define PING_INTERVALO 33

#define TRIGGER_PIN1    4 
#define ECHO_PIN1      5

#define TRIGGER_PIN2    6
#define ECHO_PIN2      7

#define TRIGGER_PIN3    8
#define ECHO_PIN3      9

NewPing sensor1(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCIA);
NewPing sensor2(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCIA);
NewPing sensor3(TRIGGER_PIN3, ECHO_PIN3, MAX_DISTANCIA);


int LEDS[3][2]={
 {12,13},
 {14,15},
 {16,17}
};

String js = ""; 
unsigned long pingTiempo[SONIDO_NUM];
unsigned int cm[SONIDO_NUM]; 
uint8_t sensorActual = 0; 
  /*
NewPing Emitir[SONIDO_NUM] = { 
  NewPing(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCIA),
  //NewPing(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCIA),
  //NewPing(TRIGGER_PIN3, ECHO_PIN3, MAX_DISTANCIA)
};
*/



void setup() {

  SerialESP8266.begin(9600);
  Serial.begin(9600);
  SerialESP8266.setTimeout(2000);


    //SerialESP8266.println("AT+RST");
   // SerialESP8266.println("AT+CIOBAUD=19200");
   


/*
  String input ="{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  JsonObject& root = jsonBuffer.parseObject(input);


  //long time = root[String("time")];
  //root[String("time")] = time;
  String sensor = root["sensor"];  
   Serial.println(sensor);

*/


   
  //Verificamos si el ESP8266 responde
  SerialESP8266.println("AT");
  

  if(SerialESP8266.find("OK")){
    Serial.println("Respuesta AT correcto");
  } else
    Serial.println("Error en ESP8266");

  //-----Configuración de red-------//Podemos comentar si el ESP ya está configurado

    //ESP8266 en modo estación (nos conectaremos a una red existente)
    SerialESP8266.println("AT+CWMODE=1");
    if(SerialESP8266.find("OK"))
      Serial.println("ESP8266 en modo Estacion");
      
    //Nos conectamos a una red wifi 
    SerialESP8266.println("AT+CWJAP=\"SSID_NOMBREWIFI\",\"passwordWifi\"");
    Serial.println("Conectandose a la red ...");
    SerialESP8266.setTimeout(20000); //Aumentar si demora la conexion
    if(SerialESP8266.find("OK"))
      Serial.println("WIFI conectado");
    else
      Serial.println("Error al conectarse en la red");
    SerialESP8266.setTimeout(2000);
    //Desabilitamos las conexiones multiples
    SerialESP8266.println("AT+CIPMUX=0");
    if(SerialESP8266.find("OK"))
      Serial.println("Multiconexiones deshabilitadas");
    
  //------fin de configuracion-------------------

/*
  pingTiempo[0] = millis() + 75; 
  for (uint8_t i = 1; i < SONIDO_NUM; i++)
    pingTiempo[i] = pingTiempo[i - 1] + PING_INTERVALO;
*/

//pinMode(TRIGGER_PIN, OUTPUT); //salida del pulso generado por el sensor ultrasónico
//pinMode(ECHO_PIN, INPUT);//entrada del pulso generado por el sensor ultrasónico


  //delay(1000);



}


 
void loop() {
  


 
  /*
      StaticJsonBuffer<200> jsonBuffer;
      
      JsonObject& root = jsonBuffer.createObject();
      root["total"] = 0;

      JsonArray& data = root.createNestedArray("disponibles");
      if(s1 >= 17 && s1 <= 22) data.add(2);
      if(s2 >= 17 && s2 <= 22) data.add(1);
      if(s3 >= 17 && s3 <= 22) data.add(0);
      */


      loop1();

}


void loop1() {
      String respuesta = RestCliente(POST, "/Estados.svc", "enviar");
      Serial.println("respuesta===> "+respuesta);


                          DynamicJsonBuffer jsonBuffer;
                          JsonObject& json = jsonBuffer.parseObject(respuesta);
                          if (json.success()) {
                          JsonArray& reservados = json["reservados"];
                          int val1 = digitalRead(LEDS[0][0]); int val2 = digitalRead(LEDS[1][0]); int val3 = digitalRead(LEDS[2][0]); 
                          if(val1 == HIGH) digitalWrite(LEDS[0][0], LOW);
                          if(val1 == HIGH) digitalWrite(LEDS[1][0], LOW);
                          if(val1 == HIGH) digitalWrite(LEDS[2][0], LOW);
                            
                            for (JsonVariant reserva : reservados) {
                                int v = reserva.as<int>()-1; //String : reserva.as<char*>();
                                if(v == 0) if(val1 == LOW) digitalWrite(LEDS[0][0], HIGH); 
                                if(v == 1) if(val2 == LOW) digitalWrite(LEDS[1][0], HIGH); 
                                if(v == 2) if(val3 == LOW) digitalWrite(LEDS[2][0], HIGH); 
                            }
                          }
                          
}
/*
void loop2() {

                            for (uint8_t i = 0; i < SONIDO_NUM; i++) {
    if (millis() >= pingTiempo[i]) {
      pingTiempo[i] += PING_INTERVALO * SONIDO_NUM;
      if (i == 0 && sensorActual == SONIDO_NUM - 1)
        ciclo(); 
      Emitir[sensorActual].timer_stop();
      sensorActual = i;
      cm[sensorActual] = 0;
      Emitir[sensorActual].ping_timer(echoCheck);
    }
  }
}

void echoCheck() { 
  if (Emitir[sensorActual].check_timer())
    cm[sensorActual] = Emitir[sensorActual].ping_result / US_ROUNDTRIP_CM;
}
*/
 
void ciclo() { 
  for (uint8_t i = 0; i < SONIDO_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");

  if(cm[i] >= 18 && cm[i] <= 23) digitalWrite(LEDS[i][1], LOW); else digitalWrite(LEDS[i][1], HIGH); 
    
  }
  Serial.println();
}







String RestCliente(String METODO, String servicio, String parametro) {

  
  //---------enviamos las variables al servidor---------------------

  
        unsigned int s1 = sensor1.ping_cm();unsigned int s2 = sensor2.ping_cm();unsigned int s3 = sensor3.ping_cm();

      js = ""; 
      digitalWrite(LEDS[0][1], LOW); digitalWrite(LEDS[1][1], LOW); digitalWrite(LEDS[2][1], LOW);
      if( !(s1 >= 17 && s1 <= 22)){  js += "2,"; digitalWrite(LEDS[0][1], HIGH);} 
      if( !(s2 >= 17 && s2 <= 22)){  js += "1,"; digitalWrite(LEDS[1][1], HIGH);} 
      if( !(s3 >= 17 && s3 <= 22)){  js += "0,"; digitalWrite(LEDS[2][1], HIGH);} 
        
        Serial.print(s1);
        Serial.print(" - ");
        Serial.print(s2);
        Serial.print(" - ");
        Serial.print(s3);
        Serial.println(" --- "+js);
        
        
              js = js.substring(0, js.length() - 1); 
        


      //Nos conectamos con el servidor:
      
      SerialESP8266.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");
      if( SerialESP8266.find("OK"))
      {  

          //Armamos el encabezado de la peticion http
          String peticionHTTP = METODO+" "+servicio+"/"+parametro;
          peticionHTTP = peticionHTTP +" HTTP/1.1\r\n";
          peticionHTTP = peticionHTTP +"Content-Type: application/json\r\n";
          peticionHTTP = peticionHTTP +"Host: restlosportales.azurewebsites.net\r\n";
          peticionHTTP = peticionHTTP +"Accept: application/json\r\n";
          peticionHTTP = peticionHTTP +"Content-Length: "+(28+js.length())+"\r\n\r\n";
          peticionHTTP = peticionHTTP+"{\"total\":2,\"disponibles\":["+js+"]}";
          //Enviamos el tamaño en caracteres de la peticion http:  
          SerialESP8266.print("AT+CIPSEND=");
          SerialESP8266.println(peticionHTTP.length());

          //esperamos a ">" para enviar la petcion  http
          if(SerialESP8266.find(">")) // ">" indica que podemos enviar la peticion http
          {
            //Serial.println("Enviando HTTP . . .");
            SerialESP8266.println(peticionHTTP);
            if( SerialESP8266.find("SEND OK"))
            {  
             
              boolean fin_respuesta=false; 
              long tiempo_inicio=millis(); 
              String cadena="";
              String cadena_json = "";
              boolean algo=false; 
              while(fin_respuesta==false)
              {
                  while(SerialESP8266.available()>0) 
                  {
                      char c=SerialESP8266.read();

                //Serial.write(c);//
               
                    if(!algo && c == '{') algo = true;
                    
                            if(algo) {
                             //   Serial.println(c);
                            
                            cadena_json.concat(c);
                               //guardamos la respuesta en el string "cadena"
                             }
                     
                     if(c == '}') algo = false;

                     
                     cadena.concat(c);
                  }

                  /*
                  //finalizamos si la respuesta es mayor a 500 caracteres
                  if(cadena.length()>500) //Pueden aumentar si tenen suficiente espacio en la memoria
                  {
                    Serial.println("La respuesta a excedido el tamaño maximo");
                    
                    SerialESP8266.println("AT+CIPCLOSE");
                    if( SerialESP8266.find("OK"))
                      Serial.println("Conexion finalizada");
                    fin_respuesta=true;
                  }
 */


                  
                  if(cadena.indexOf("CLOSED")>0) //si recibimos un CLOSED significa que ha finalizado la respuesta
                  {

                   //Serial.println("Cadena recibida correctamente, conexion finalizada");         
                    fin_respuesta=true;
                    return cadena_json;
                  }   
                  if((millis()-tiempo_inicio)>1000) //Finalizamos si ya han transcurrido 10 seg
                  {
                    //Serial.println("Tiempo de espera agotado");
                    SerialESP8266.println("AT+CIPCLOSE");
                    if( SerialESP8266.find("OK")){}// Serial.println("Conexion finalizada");
                    fin_respuesta=true;
                       //Serial.println(cadena_json);
                      return cadena_json;
                  }
                
              }

              
            }
            else
            {
              Serial.println("No se ha podido enviar HTTP.....");
           }            
          }
      }
      else
      {
        Serial.println("No se ha podido conectarse con el servidor");
      }
      


  
}

