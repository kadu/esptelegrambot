#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <StringTokenizer.h>
#include <secrets.h>

#define PIN       D6
#define NUMPIXELS 72
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_BMP085 bmp;

const unsigned long BOT_MTBS = 1000;
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;

const int ledPin = LED_BUILTIN;
int ledStatus = 0;
int red;
int green;
int blue;

int hexcolorToInt(char upper, char lower) {
  int uVal = (int)upper;
  int lVal = (int)lower;
  uVal = uVal > 64 ? uVal - 55 : uVal - 48;
  uVal = uVal << 4;
  lVal = lVal > 64 ? lVal - 55 : lVal - 48;
  return uVal + lVal;
}

void getRGB(String hexvalue) {
  hexvalue.toUpperCase();
  char c[7];
  hexvalue.toCharArray(c, 8);
  red = hexcolorToInt(c[1], c[2]);
  green = hexcolorToInt(c[3], c[4]);
  blue = hexcolorToInt(c[5], c[6]);
}

void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    StringTokenizer tokens(text, " ");
    String comando = tokens.nextToken();

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    Serial.println("** DEBUG **\n");
    Serial.printf("Message......: %s \n",   bot.messages[i].text);
    Serial.printf("ChatID.......: %s \n",   bot.messages[i].chat_id);
    Serial.printf("FromID.......: %s \n",   bot.messages[i].from_id);
    Serial.printf("FromName.....: %s \n",   bot.messages[i].from_name);
    Serial.printf("Type.........: %s \n",   bot.messages[i].type);
    Serial.printf("hasDocument..: %s \n",   bot.messages[i].hasDocument? "YES":"NO");
    Serial.printf("file_caption.: %s \n",   bot.messages[i].file_caption);
    Serial.printf("file_path....: %s \n",   bot.messages[i].file_path);
    Serial.printf("file_name....: %s \n",   bot.messages[i].file_name);
    Serial.printf("longitude....: %f \n",   bot.messages[i].longitude);
    Serial.printf("latitude.....: %f \n\n", bot.messages[i].latitude);

    if (comando == "/ledon") {
      digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (comando == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, HIGH); // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (comando == "/status") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Led is ON", "");
      }
      else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    if (comando == "/temp") {
      String retmessage = "Welcome to Weather Bulletin, sensor is in Limeira City (Brazil)\n";
      retmessage += "Current temperature :" + String(bmp.readTemperature()) + "ºC.";
      retmessage += "\nPressure: " + String(bmp.readPressure()) + " Pa";
      retmessage += "\nAltitude: " + String(bmp.readAltitude(102500)) + " meters";
      retmessage += "\nSea Level Pressure: " + String(bmp.readSealevelPressure()) + "Pa";
      retmessage += "\nThanks for asking. This Bulletin was sponsored by **Garage CI&T**";
      retmessage += "\nJoin on our [Workplace Group](https://ciandt.workplace.com/groups/475277656980794)";
      retmessage += "\nJoin on our [Google Meet Group](https://mail.google.com/chat/u/0/#chat/space/AAAASAmQt9I)";
      bot.sendMessage(chat_id, retmessage, "Markdown");
    }

    if(comando == "/strip") {
      Serial.println("STRIP");
      String pixelnumber = tokens.nextToken();  // # led
      String pixelcolor = tokens.nextToken();  // color
      getRGB(pixelcolor);
      pixels.setPixelColor(pixelnumber.toInt()-1, pixels.Color(red, green, blue));
      pixels.show();
      bot.sendMessage(chat_id, "Pixel is setted!", "");
    }

    if(comando == "/clear") {
      if(bot.messages[i].chat_id == BOT_OWNERID) {
        pixels.clear();
        pixels.show();
        bot.sendMessage(chat_id, "All pixels cleared", "");
      } else {
        bot.sendMessage(chat_id, "Only admin can clear pixels!", "");
      }

    }

    if (comando == "/start") {
      String welcome = "Hy, " + from_name + ".\n";
      welcome += "This is our Telegram BOT that test ESP8266/ESP32 microcontrollers.\n\n";
      welcome += "/ledon : to switch the internal Led ON\n";
      welcome += "/ledoff : to switch the internal Led OFF\n";
      welcome += "/status : Returns current status of LED\n";
      welcome += "/temp : Returns temperature of ROOM where MCU was installed\n";
      welcome += "/strip NUM COLOR: change Pixel Color, NUM is a value between 1-72 and COLOR must be in HEXADECIMAL like #FF00FF\n";
      welcome += "/clear : **Admin** Turn all led strip off * Only Chat Owner can execute!\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void checkTemperature() {
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");

  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");

// you can get a more precise measurement of altitude
// if you know the current sea level pressure which will
// vary with weather and such. If it is 1015 millibars
// that is equal to 101500 Pascals.
  Serial.print("Real altitude = ");
  Serial.print(bmp.readAltitude(102500));
  Serial.println(" meters");
}

void bot_setup() {
  const String commands = F("["
                            "{\"command\":\"ledon\",  \"description\":\"Switch the internal Led ON\"},"
                            "{\"command\":\"ledoff\", \"description\":\"Switch the internal Led OFF\"},"
                            "{\"command\":\"status\", \"description\":\"Returns current status of LED\"},"
                            "{\"command\":\"temp\", \"description\":\"Returns temperature of ROOM where MCU was installed\"},"
                            "{\"command\":\"strip\", \"description\":\"Change Pixel Color, 2 arguments are needed: NUM is a value between 1-72 and COLOR must be in HEXADECIMAL e.g. **/strip 2 #FF0000** - Turn Pixel #02 Red\"},"
                            "{\"command\":\"clear\",\"description\":\"**Admin** Turn all led strip off * Only Chat Owner can execute!\"}" // no comma on last command
                            "]");
  bot.setMyCommands(commands);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); // initialize pin as off (active LOW)

  pixels.begin();
  pixels.clear();
  pixels.setBrightness(10);

  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.setPixelColor(1, pixels.Color(0, 255, 0));
  pixels.setPixelColor(2, pixels.Color(0, 0, 255));
  pixels.show();
  delay(500);
  pixels.clear();
  pixels.show();

  if (!bmp.begin()) {
	  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

  // Conectando no WiFiClientSecure
// attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot_setup();

  getRGB("#FF00FF");
  checkTemperature();
}

void loop() {

  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}