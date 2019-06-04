/*

 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69


 */
#include <SoftwareSerial.h>
#include <NewPing.h>


/***************************************************************************
*             - Ultrasonik Mesafe Okuma  & Pin  Sabitleri-
***************************************************************************/
const int SagOnUltraSonikPin   = 13;
const int SolOnUltrasonikPin   = 12;
const int SagArkaUltrasonicPin = 11;
const int SolArkaUltrasonicPin = 10;

const int JetsonaGonderRxPin   = 50;
const int JetsonaGonderTxPin   = 51;
const int JetsondanAlRxPin     = 52;
const int JetsondanAlTxPin     = 53;


const int SagOnOkumayiBaslat   = 0;
const int SolOnOkumayiBaslat   = 1;
const int SagArkaOkumayiBaslat = 2;
const int SolArkaOkumayiBaslat = 3;


const int MaximumDistance      = 200;
const int SensorSayisi         = 4;

/***************************************************************************
*                    -Serial İşlemleri Sabitler-
***************************************************************************/
uint8_t  SagOnMesafe     = 0;
uint8_t  SagArkaMesafe   = 0;
uint8_t  SolOnMesafe     = 0;
uint8_t  SolArkaMesafe   = 0;
uint16_t SagToplamMesafe = 0;
uint16_t SolToplamMesafe = 0;
uint32_t DataFormat      = 0;
uint8_t  BaslangicByte   = 0;

int      BaundRateSerial = 9600 ;  // Serial İçin BaundRate
int      BaundRateJetson = 9600 ;// Jetson İçin BaundRate

SoftwareSerial JetsonaGonder (JetsonaGonderRxPin, JetsonaGonderTxPin); // RX, TX
SoftwareSerial JetsondanAl   (JetsondanAlRxPin, JetsondanAlTxPin    ); // RX, TX


typedef  struct
{
  uint8_t Paket[4];

} DataPack ;
DataPack veriler;
/***************************************************************************
*                       -Fonksiyon Prototipleri -
***************************************************************************/
uint32_t VerileriTopla();
void OkumaLoop ();
void DataFormatStruct();

/***************************************************************************
*                       -Ping.h Sensor Tanımlmaları -
***************************************************************************/
/*NewPing sonar[0](SagOnUltraSonikPin,SagOnUltraSonikPin);
NewPing sonar[1](SolOnUltrasonikPin,SolOnUltrasonikPin);*/
NewPing sonar[SensorSayisi] = {
  NewPing(SagOnUltraSonikPin, SagOnUltraSonikPin, MaximumDistance),
  NewPing(SolOnUltrasonikPin, SolOnUltrasonikPin, MaximumDistance),
  NewPing(SagArkaUltrasonicPin, SagArkaUltrasonicPin, MaximumDistance),
  NewPing(SolArkaUltrasonicPin, SolArkaUltrasonicPin, MaximumDistance)
};
/***************************************************************************
*                     - Void Setup & Void Loop-
***************************************************************************/
void setup()
{
  Serial.begin(BaundRateSerial);
  while (!Serial)
  {
    ; // Usb Portun Baglanmasını Bekle
  }

  Serial.println(  "Arduino Baglandi");
  JetsonaGonder.begin(BaundRateJetson);
  JetsondanAl.begin(  BaundRateJetson);
  delay (10);
  JetsonaGonder.println("Jetsona Mesafeler Gonderiliyor") ;
  JetsondanAl.println("Jetsondan YonBilgisi Aliniyor")    ;
  }

void loop() {
 delay(50);
 OkumaLoop();
 DataFormatStruct();




/*  if (JetsonaGonder.available()) {
    Serial.write(JetsonaGonder.read());
  }
  if (Serial.available()) {
    JetsonaGonder.write(Serial.read());
  }*/
  }

  /***************************************************************************
  *                      - Sensor Verileri Paketleme -
  ***************************************************************************/

  uint32_t VerileriTopla(uint8_t SagOnMesafe,uint8_t SagArkaMesafe ,uint8_t SolOnMesafe ,uint8_t SolArkaMesafe)
  {
    SagToplamMesafe = (SagOnMesafe     << 8 ) | SagArkaMesafe  ;
    SagToplamMesafe = (SolOnMesafe     << 8 ) | SolArkaMesafe  ;
    DataFormat      = (SagToplamMesafe << 16) | SolToplamMesafe;
    return DataFormat ;
  };

  /***************************************************************************
  *                      - Arduinodan Mesafe Okuma -
  ***************************************************************************/
  void OkumaLoop ()
  {
// delay(50);
 uint8_t SagOnMesafeSure   = sonar[SagOnOkumayiBaslat].ping()  ;
 uint8_t SolOnMesafeSure   = sonar[SolOnOkumayiBaslat].ping()  ;
 uint8_t SagArkaMesafeSure = sonar[SagArkaOkumayiBaslat].ping();
 uint8_t SolArkaMesafeSure = sonar[SolArkaOkumayiBaslat].ping();

 SagOnMesafe   = (SagOnMesafe / US_ROUNDTRIP_CM)  ; // Sure Mesafe Donusumleri
 SolOnMesafe   = (SolOnMesafe / US_ROUNDTRIP_CM)  ; // Sure Mesafe Donusumleri
 SagArkaMesafe = (SagArkaMesafe / US_ROUNDTRIP_CM); // Sure Mesafe Donusumleri
 SolArkaMesafe = (SolArkaMesafe / US_ROUNDTRIP_CM); // Sure Mesafe Donusumleri


 Serial.print(" SAG ON : ");
 Serial.print(SagOnMesafe);
 Serial.print("cm");
 Serial.print(" SOL ON : ");
 Serial.print(SolOnMesafe);
 Serial.print("cm");
 Serial.print(" SAG ARKA : ");
 Serial.print(SagArkaMesafe);
 Serial.print("cm");
 Serial.print(" SOL ARKA : ");
 Serial.print(SolArkaMesafe);
 Serial.println("cm");
  };

void DataFormatStruct()
  {
  veriler.Paket[0] = BaslangicByte;
  veriler.Paket[1] = SagOnMesafe;
  veriler.Paket[2] = SolOnMesafe;
  veriler.Paket[3] = SagArkaMesafe;
  veriler.Paket[4] = SolArkaMesafe;

  for (int i = 0; i < 4; i++)
  {
  JetsonaGonder.write(veriler.Paket[i]);
  };
  };
