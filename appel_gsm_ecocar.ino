// Définition du modem SIM800 - DOIT ÊTRE AVANT les includes
#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 1024

// Librairies nécessaires
#include <Wire.h>
#include <TinyGsmClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>

// Configuration pour le module SIM800L

// S'il vous plait ecrire le code pin s'il existe
const char simPIN[] = "0000"; // Laissez vide si pas de PIN
// S'il vous plait ecrire le seul numero (complet +xxx) à appeler 
#define PHONE_NUMBER "+21621324404" // Votre numéro de téléphone
// S'il vous plait ecrire le seul numero (non complet sans +xxx) à appeler 
#define PHONE_NUMBER2 "21324404" // Votre numéro de téléphone

// TTGO T-Call pins
#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_POWER_ON 23
#define MODEM_TX 27
#define MODEM_RX 26
#define I2C_SDA 21
#define I2C_SCL 22

// TTGO Buttons' pins
#define green 14
#define red 12

// Définition des ports série
#define SerialAT Serial1

// Initialisation du modem
TinyGsm modem(SerialAT);

#define IP5306_ADDR 0x75
#define IP5306_REG_SYS_CTL0 0x00

// Taille de l'écran OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Adresse I2C de l'écran (souvent 0x3C)
#define OLED_ADDRESS 0x3C

// Création de l'objet écran avec broches I2C personnalisées
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



// Fonction pour maintenir l'alimentation
bool setPowerBoostKeepOn(int en) {
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en) {
    Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    Wire.write(0x35); // 0x37 is default reg value
  }
  return Wire.endTransmission() == 0;
}


// Fonction pour vérifier les réponses AT
int checkATResponse(String command) {
  if (SerialAT.available()) {
    String response = SerialAT.readString();
    response.trim();
    if (response.indexOf("OK") != -1) {
      return 0; // good
    } else if (response.indexOf("ERROR") != -1) {
      return 1; // echec
    }
  } else {
    return -1; // pas de reponse
  }
}



void makePhoneCall() {
  // Vérifier la qualité du signal avant l'appel
  int rssi = modem.getSignalQuality();
  String quality = String(rssi*100.0/31.0);

  if (rssi > 0) {
    // Configuration audio avant l'appel
    SerialAT.println("AT+CHFA=1");
    delay(400);
    SerialAT.println("AT+CLVL=100");
    delay(400);
    int vol = checkATResponse("Volume 100%");
    delay(100);
    if (vol==0) {
      display.clearDisplay();
      display.setCursor(0, 15);
      display.println("audio ok");
      display.print("signal ");
      display.print(quality);
      display.println(" %");
      display.display();
    }
    else if (vol==1) {
      display.clearDisplay();
      display.setCursor(0, 15);
      display.println("err audio");
      display.print("signal ");
      display.print(quality);
      display.println(" %");
      display.display();
    }
    else if (vol==-1){
      display.clearDisplay();
      display.setCursor(0, 15);
      display.println("no audio");
      display.print("signal ");
      display.print(quality);
      display.println(" %");
      display.display();
    }

    delay(100);
    // Commande AT pour lancer l'appel
    SerialAT.println("ATD" + String(PHONE_NUMBER) + ";");
    display.println("appel lancee");
    display.display();
    // Attendre la réponse
    unsigned long timeout = millis() + 15000; // 15 secondes timeout
    String response = "";
    bool callStarted = false;
    
    while (millis() < timeout) {
      if (SerialAT.available()) {
        char c = SerialAT.read();
        response += c;
        //SerialMon.print(c); // Afficher la réponse pour debug
        
        if (response.indexOf("OK") != -1) {
          display.clearDisplay();
          display.setCursor(0, 15);
          display.println("Appel ...");
          display.display();
          callStarted = true;
          break;
        }
        if (response.indexOf("ERROR") != -1) {
          display.clearDisplay();
          display.setCursor(0, 15);
          display.println("Erreur appel");
          display.display();
          break;
        }
      }
      delay(100);
    }
    
    if (callStarted) {
      // Attendre que l'appel soit connecté
      display.println("Attente cnx...");
      display.display();
      delay(3000);
      
      // Reconfigurer l'audio après connexion
      display.clearDisplay();
      display.setCursor(0, 15);
      display.println("reconfig audio...");
      display.display();
      delay(100);
      SerialAT.println("AT+CHFA=1");
      delay(500);
      SerialAT.println("AT+CLVL=100");
      delay(500);
    }
  } else {
      display.clearDisplay();
      display.setCursor(0, 15);
      display.println("reseau bas !!");
      display.display();
  }
}


void hangUpCall() {
  SerialAT.println("ATH");
  
  // Attendre la confirmation
  unsigned long timeout = millis() + 5000;
  while (millis() < timeout) {
    if (SerialAT.available()) {
      String response = SerialAT.readString();
      if (response.indexOf("OK") != -1) {
        display.clearDisplay();
        display.setCursor(0, 15);
        display.println("Appel termine");
        display.display();
        break;
      }
    }
    delay(100);
  }
}


void setup() {
  // Maintenir l'alimentation en fonctionnant sur batterie
  Wire.begin(I2C_SDA, I2C_SCL);
  bool isOk = setPowerBoostKeepOn(1);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();

  // Paramètres du texte
  display.setTextSize(1);              // Taille du texte
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(SSD1306_WHITE); // Couleur du texte
  display.display();
  
  // Configuration des pins du modem
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  
  // Configuration des pins des boutons
  pinMode(red, INPUT_PULLUP);
  pinMode(green, INPUT_PULLUP);


  // Configuration de la communication avec le module SIM800L
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);
  
  // Initialisation du modem
  display.clearDisplay();
  display.setCursor(0, 15);
  display.println("init modem...");
  display.display();
  modem.restart();
  delay(4500); // Attendre plus longtemps pour l'initialisation


  // Déverrouillage de la carte SIM si nécessaire

  if (strlen(simPIN) && modem.getSimStatus() != 3) {
    delay(100);
    display.clearDisplay();
    display.setCursor(0, 15);
    display.println("deblocage");
    display.println("     simPIN...");
    display.display();
    delay(100);
    modem.simUnlock(simPIN);
  }
  delay(4500);
  
  // Vérifier l'enregistrement sur le réseau
  if (modem.isNetworkConnected()) {
    delay(100);
    display.clearDisplay();
    display.setCursor(0, 15);
    display.println("Connecte");
    display.display();
  } else {
    delay(100);
    display.clearDisplay();
    display.setCursor(0, 15);
    display.println("Pas de cnx");
    display.display();
  }
  delay(100);
  display.println("systeme pret.");
  display.display();
}

void loop() {
  static bool finappel = false;
  static long start = millis();;
  if (finappel==true){
    if (millis()-start > 3000){
      display.clearDisplay();
      display.setCursor(0, 15);
      display.println("Systeme pret.");
      display.display();
      finappel = false;
    }
  }
    if (digitalRead(green)==LOW) {
      display.clearDisplay();
      display.setCursor(0, 15);
      display.println("appel...");
      display.display();
      delay(100);
      makePhoneCall();
      delay(100);
    } else if (digitalRead(red)==LOW) {
      delay(100);
      hangUpCall();
      delay(100);
      display.clearDisplay();
      display.setCursor(0, 15);
      display.println("Appel termine");
      display.display();
      finappel = true;
      start = millis();
    }
    if (SerialAT.available()){
      String response = SerialAT.readString();
      if (response.indexOf("NO CARRIER") != -1){
        delay(100);
        display.clearDisplay();
        display.setCursor(0, 15);
        display.println("Appel termine");
        display.display();
        finappel = true;
        start = millis();
      }
      if (response.indexOf("RING") != -1){
        delay(100);
        if (response.indexOf(PHONE_NUMBER2) != -1){
          SerialAT.println("AT+CHFA=1"); // haut parleur
          delay(100);
          display.clearDisplay();
          display.setCursor(0, 15);
          display.println("H-parl. active");
          display.display();
          delay(100);
          SerialAT.println("AT+CLVL=100"); // volume 100%
          display.println(" vol. max");
          display.display();
          delay(100);
          display.println("appel commence");
          display.display();
          delay(100);
          SerialAT.println("ATA");
        }
        else {
          SerialAT.println("ATH"); // Termine l’appel
          delay(250);
          display.clearDisplay();
          display.setCursor(0, 15);
          display.print("appel etranger...");
          display.println("decrochage...");
          display.display();
          finappel = true;
          start = millis();
        }
      }
    }
  delay(100);
}
