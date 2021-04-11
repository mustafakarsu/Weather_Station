#include <FirebaseArduino.h>
#include <DHT.h> 
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>                                                                                                         

#define I2C_SCL 5                   
#define I2C_SDA 4
#define FIREBASE_HOST "firebase-host-name"                          
#define FIREBASE_AUTH "firebse-host-password"            
#define WIFI_SSID "Wifi-ssid"                                             
#define WIFI_PASSWORD "Wifi-password"                                    
#define DHTPIN 2                                                           
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);      
Adafruit_BMP085 bmp;    
bool bmp085_present=true;      
const long utcOffsetInSeconds = 10800;                                         
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(9600); 
  Serial.println("Uyandım");              
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     
  Serial.print("Bağlantı Kuruldu  ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Bağlandı ");
  Serial.println(WIFI_SSID);
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());                                         
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                           
  dht.begin(); 
  timeClient.begin();
  timeClient.forceUpdate();
  //______________________________ BMP180 _______________________________________
  if (!bmp.begin()) 
          {
              Serial.println("BMP180 Bulunamadı Kontrol Et!!");
              while (1) {}
          }
      int hpa=bmp.readPressure()/100;
      int bmpderece=bmp.readTemperature();
      int rakim=bmp.readAltitude();
      int denizsevbasinc=bmp.readSealevelPressure()/100;
      int gercekrakim=bmp.readAltitude(101500);
 // ________________________________________DHT11____________________________________________________________
   float h = dht.readHumidity();                                              
   float t = dht.readTemperature();                                           
    int nem=h/1;
    int derece=t/1;
  if (isnan(h) || isnan(t)) {                                               
    Serial.println(F("DHT Hatası !"));
    return;
  }
   //______________________________ Çiğ Noktası______________________________________________________
          float gamma = log(h/100) + ((17.62*t) / (243.5+t));
          float dp = 243.5*gamma / (17.62-gamma);
            /*24 ve üstü = son derece rahatsız edici,bunaltıcı hava
          21-23      = çok nemli,bunaltıcı hava
          18-20 Çoğu kişi için bunaltıcı hava
          15-17 çoğu kişi için iyi,anaca 
          12-14 rahat hava
          9-11  çok rahat hava
          8 ve altı  kuru hava*/     
 //______________________________________________________Yağmur Sensörü_____________________________
          int rain= analogRead(A0);
 // ____________________________________Sis________________________________________________
          float sis=derece-dp;
  //______________________________________________________________________________________________                   
  Serial.print("Derece: ");  Serial.print(t);  Serial.println(" °C ");
  Serial.print("Nem: ");  Serial.print(h); Serial.println("% ");  
  Serial.print("BMP180 Derece: ");  Serial.print(bmpderece);  Serial.println(" °C ");
  Serial.print("BMP180 Rakım ");  Serial.print(rakim);  Serial.println(" metre ");  
  Serial.print("BMP180 Deniz Seviyesi Basınç ");  Serial.print(denizsevbasinc);  Serial.println(" Pa ");  
  Serial.print("BMP180 Gerçek Rakım ");  Serial.print(gercekrakim);  Serial.println(" metre ");    
  Serial.print("Dew Point: ");Serial.print(dp); Serial.println(" dp "); 
  Serial.print("Sis: ");Serial.print(sis); Serial.println(" sis ");                    
  Serial.print("Basınç: ");  Serial.print(hpa); Serial.println(" hPa ");
  Serial.print("Yağmur: ");Serial.print(rain); Serial.println(" rain "); 
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
                                                
  Firebase.setInt("nem",nem);                                
  Firebase.setInt("derece",derece); 
  Firebase.setInt("bmpderece",bmpderece); 
  Firebase.setInt("bmprakim",rakim); 
  Firebase.setInt("bmpdenizsevbasinc",denizsevbasinc); 
  Firebase.setInt("bmpgercekrakim",gercekrakim);   
  Firebase.setInt("Dp",dp);
  Firebase.setInt("sis",sis);
  Firebase.setInt("rain",rain);  
  Firebase.setInt("hpa",hpa); 
  Firebase.setInt("saat",timeClient.getHours());
  Firebase.setInt("dakika",timeClient.getMinutes() );     
  Serial.println("Uykudayım");
  ESP.deepSleep(30*60*1000000); //30*60*1000000=30min }
 void loop(){}
