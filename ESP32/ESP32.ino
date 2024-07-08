#include <WiFi.h>

#include <SocketIoClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTPIN 32     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

const int buttonDen = 12;
const int buttonQuat = 13;
const int buttonBom = 14;
const int quatPin = 25;
const int denPin = 27;
const int bomPin = 26;
int nhietdo = 0;
int doam = 0;
long last = 0;
int stateDen = 0;  
int stateBom = 0;  
int stateQuat = 0; 
int isPress1 = 0;  
int isPress2 = 0;  
int isPress3 = 0;  

// khai báo mảng kết nối wifi
const char* ssid = "CR7";
const char* pass = "15072024";

// tạo ra biến để lưu thông số server
// Server Ip
const char* server = "192.168.1.27";
// Server port
int port = 3000;

SocketIoClient socket;


String DataJson = "";
String Data = "";


void setup()
{
  Serial.begin(115200);
  pinMode(buttonDen, INPUT_PULLUP);
  pinMode(buttonBom, INPUT_PULLUP);
  pinMode(buttonQuat, INPUT_PULLUP);
  pinMode(denPin, OUTPUT);
  pinMode(bomPin, OUTPUT);
  pinMode(quatPin, OUTPUT);
  dht.begin();
  last = millis();
  setupNetwork();
  
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
  nutnhan();

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

    if (JSON["Den"] == "0")
    {
     

      digitalWrite(denPin, LOW);
      stateDen = 0;
      Serial.println("Den OFF!!!");

    }
    else if (JSON["Den"] == "1")
    {
    
      digitalWrite(denPin, HIGH);
      stateDen = 1;
      Serial.println("Den ON!!!");
    }
    else if (JSON["Bom"] == "0")
    {
      
      digitalWrite(bomPin, LOW);
      stateBom = 0;
      Serial.println("Bom OFF!!!");
    }
    else if (JSON["Bom"] == "1")
    {
     

      digitalWrite(bomPin, HIGH);
      stateBom = 1;
      Serial.println("Bom ON!!!");
    }
    else if (JSON["Quat"] == "0")
    {
     
      digitalWrite(quatPin, LOW);
      stateQuat = 0;
      Serial.println("Quat OFF!!!");
    }
    else if (JSON["Quat"] == "1")
    {
  

      digitalWrite(quatPin, HIGH);
      stateQuat = 1;
      Serial.println("Quat ON!!!");
    }
  }
}

void Datajson()
{
  DataJson  = "{\"ND\":\"" + String(nhietdo) + "\"," +
                  "\"DA\":\"" + String(doam) + "\"," +
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
    Chuongtrinhcambien();
    Datajson();
    last = millis();
  }
}


void Chuongtrinhcambien()
{

  nhietdo = dht.readTemperature();
  doam = dht.readHumidity();
  
  // Kiểm tra nếu có lỗi trong quá trình đọc giá trị từ DHT11
  if (isnan(nhietdo) || isnan(doam)) {
    Serial.println("Loi: Khong the doc du lieu tu DHT11");
  }
}

void nutnhan() {
  // Nút nhấn 1 (đèn)
  if (digitalRead(buttonDen) == LOW) {
    delay(20);
    if ((digitalRead(buttonDen) == LOW) && (isPress1 == 0)) {
      stateDen = (stateDen == LOW) ? HIGH : LOW;
      isPress1 = 1;
    }
  } else {
    isPress1 = 0;
  }

  // Nút nhấn 2 (bơm)
  if (digitalRead(buttonBom) == LOW) {
    delay(20);
    if ((digitalRead(buttonBom) == LOW) && (isPress2 == 0)) {
      stateBom = (stateBom == LOW) ? HIGH : LOW;
      isPress2 = 1;
    }
  } else {
    isPress2 = 0;
  }

  // Nút nhấn 3 (quạt)
  if (digitalRead(buttonQuat) == LOW) {
    delay(20);
    if ((digitalRead(buttonQuat) == LOW) && (isPress3 == 0)) {
      stateQuat = (stateQuat == LOW) ? HIGH : LOW;
      isPress3 = 1;
    }
  } else {
    isPress3 = 0;
  }

  // Điều khiển các thiết bị dựa trên trạng thái hiện tại
  digitalWrite(denPin, stateDen);
  digitalWrite(bomPin, stateBom);
  digitalWrite(quatPin, stateQuat);
}

