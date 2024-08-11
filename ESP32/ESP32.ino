#include <WiFi.h>

#include <SocketIoClient.h>
#include <ArduinoJson.h>
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

DHTesp dht;

/** Pin number for DHT11 data pin */
int dhtPin = 33;

const int buttonDen = 13;
const int buttonQuat = 14;
const int buttonBom = 12;
const int buttonAuto = 32;
const int quatPin = 25;
const int denPin = 27;
const int bomPin = 26;
int nhietdo = 0;
int doam = 0;
long last = 0;
int stateAuto = 0;
int stateDen = 0;  
int stateBom = 0;  
int stateQuat = 0; 
int isPress0 = 0;
int isPress1 = 0;  
int isPress2 = 0;  
int isPress3 = 0;  

// khai báo mảng kết nối wifi
const char* ssid = "0C";
const char* pass = "00000000";

// tạo ra biến để lưu thông số server
// Server Ip
const char* server = "192.168.133.94";
// Server port
int port = 3000;

SocketIoClient socket;


String DataJson = "";
String Data = "";

void initTemp() {
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT initiated");
}

/**
 * getTemperatureAndHumidity
 * Reads temperature and humidity from DHT11 sensor
 * @return bool
 *    true if temperature and humidity could be acquired
 *    false if acquisition failed
 */
bool getTemperatureAndHumidity() {
  // Reading temperature and humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  TempAndHumidity newValues = dht.getTempAndHumidity();
  // Check if any reads failed and exit early (to try again).
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return false;
  }

  // Assign temperature and humidity to the variables
  nhietdo = newValues.temperature;
  doam = newValues.humidity;
  return true;
}

void setup()
{
  Serial.begin(115200);
  pinMode(buttonAuto, INPUT_PULLUP);
  pinMode(buttonDen, INPUT_PULLUP);
  pinMode(buttonBom, INPUT_PULLUP);
  pinMode(buttonQuat, INPUT_PULLUP);
  pinMode(denPin, OUTPUT);
  pinMode(bomPin, OUTPUT);
  pinMode(quatPin, OUTPUT);
  last = millis();
  setupNetwork();
  initTemp();
  // kết nối server nodejs
  socket.begin(server, port);
  // lắng nghe sự kiện server gửi
  socket.on("ESP", handleMessage);
  last = millis();
  Serial.println("ESP Start");
}

void loop()
{
  DuytriServer();
  SendDataNodeJS();
  if(stateAuto == 0)
  {
    nutnhan();
  }
  else{
    handleAutoMode();
    nutnhan();
  }
}

void setupNetwork()
{
  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Wifi connected!");
}

void handleMessage(const char* message , size_t length)
{
  last = millis();
  Serial.print("Data nhận được: ");
  Serial.println(message);
  Data = message;


  ParseJson(String(Data));
  

  Data = "";
  last = millis();
}

void DuytriServer()
{
  socket.loop();
}

void ParseJson(String Data)
{
  // đưa dữ liệu json vào thư viện json để kiểm tra đúng hay sai , đúng thì tách dữ liệu => điều khiển

  const size_t capacity = JSON_OBJECT_SIZE(2) + 256;
  DynamicJsonDocument JSON(capacity);
  DeserializationError error = deserializeJson(JSON, Data);
  if (error)
  {
    Serial.println("Data JSON Error!!!");
    return;
  }
  else
  {
    Serial.println();
    Serial.println("Data JSON ESP: ");
    serializeJsonPretty(JSON, Serial);
    if (JSON.containsKey("Auto")) {
    stateAuto = JSON["Auto"] == "0" ? LOW : HIGH;
    }

    // Cập nhật trạng thái Đèn
    if (JSON.containsKey("Den")) {
    stateDen = JSON["Den"] == "0" ? LOW : HIGH;
    digitalWrite(denPin, stateDen);
    Serial.println("Den " + String(stateDen == LOW ? "OFF" : "ON"));
    }

    // Cập nhật trạng thái Bơm
    if (JSON.containsKey("Bom")) {
    stateBom = JSON["Bom"] == "0" ? LOW : HIGH;
    digitalWrite(bomPin, stateBom);
    Serial.println("Bom " + String(stateBom == LOW ? "OFF" : "ON"));
    }

    // Cập nhật trạng thái Quạt
    if (JSON.containsKey("Quat")) {
    stateQuat = JSON["Quat"] == "0" ? LOW : HIGH;
    digitalWrite(quatPin, stateQuat);
    Serial.println("Quat " + String(stateQuat == LOW ? "OFF" : "ON"));
    }
  }
}

void Datajson()
{
  DataJson  = "{\"ND\":\"" + String(nhietdo) + "\"," +
                  "\"DA\":\"" + String(doam) + "\"," +
                  "\"Auto\":\"" + String(stateAuto) + "\"," +
                  "\"Den\":\"" + String(stateDen) + "\"," +
                  "\"Bom\":\"" + String(stateBom) + "\"," +
                  "\"Quat\":\"" + String(stateQuat) + "\"}";
  Serial.println();
  Serial.print("DataJson: ");
  Serial.println(DataJson);

  socket.emit("ESPJSON", DataJson.c_str());

}

void SendDataNodeJS()
{
  if (millis() - last >= 1000)
  {
    getTemperatureAndHumidity();
    Datajson();
    last = millis();
  }
}


void nutnhan() {
  // Xử lý nút Auto
  if (digitalRead(buttonAuto) == LOW) {
    delay(20); // Debounce
    if (digitalRead(buttonAuto) == LOW && isPress0 == 0) {
      stateAuto = (stateAuto == LOW) ? HIGH : LOW;
      isPress0 = 1;
      Serial.println("Auto button pressed, stateAuto: " + String(stateAuto));
    }
  } else {
    isPress0 = 0;
  }

  // Xử lý nút Đèn
  if (digitalRead(buttonDen) == LOW) {
    delay(20); // Debounce
    if (digitalRead(buttonDen) == LOW && isPress1 == 0) {
      stateDen = (stateDen == LOW) ? HIGH : LOW;
      isPress1 = 1;
      Serial.println("Den button pressed, stateDen: " + String(stateDen));
      // Điều khiển đèn
      digitalWrite(denPin, stateDen);
    }
  } else {
    isPress1 = 0;
  }

  // Xử lý nút Bơm
  if (digitalRead(buttonBom) == LOW) {
    delay(20); // Debounce
    if (digitalRead(buttonBom) == LOW && isPress2 == 0) {
      stateBom = (stateBom == LOW) ? HIGH : LOW;
      isPress2 = 1;
      Serial.println("Bom button pressed, stateBom: " + String(stateBom));
      // Điều khiển bơm
      digitalWrite(bomPin, stateBom);
    }
  } else {
    isPress2 = 0;
  }

  // Xử lý nút Quạt
  if (digitalRead(buttonQuat) == LOW) {
    delay(20); // Debounce
    if (digitalRead(buttonQuat) == LOW && isPress3 == 0) {
      stateQuat = (stateQuat == LOW) ? HIGH : LOW;
      isPress3 = 1;
      Serial.println("Quat button pressed, stateQuat: " + String(stateQuat));
      // Điều khiển quạt
      digitalWrite(quatPin, stateQuat);
    }
  } else {
    isPress3 = 0;
  }
  
  // Điều khiển các thiết bị khác
  // Điều khiển đèn, bơm và quạt dựa trên trạng thái
  digitalWrite(denPin, stateDen);
  digitalWrite(bomPin, stateBom);
  digitalWrite(quatPin, stateQuat);
}
// Nếu ở chế độ AutoMode thì quạt và bơm sẽ hoạt động dựa theo thông số cảm biến
void handleAutoMode() {
  if (nhietdo >= 30) {
    digitalWrite(quatPin, 1);
    digitalWrite(bomPin, 1);
    stateQuat = 1;
    stateBom = 1;
  } 
  else if (nhietdo <= 25) {
    digitalWrite(quatPin, 0);
    digitalWrite(bomPin, 0);
    stateQuat = 0;
    stateBom = 0;
  }
}
