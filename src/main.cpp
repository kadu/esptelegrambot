#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <secrets.h>

#define PIN       D1
#define NUMPIXELS 20
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const unsigned long BOT_MTBS = 1000;
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;

const int ledPin = LED_BUILTIN;
int ledStatus = 0;

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
  int red = hexcolorToInt(c[1], c[2]);
  int green = hexcolorToInt(c[3], c[4]);
  int blue = hexcolorToInt(c[5], c[6]);
  Serial.print("REED: "); Serial.println(red);
  Serial.print("green: "); Serial.println(green);
  Serial.print("blue: "); Serial.println(blue);
}

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

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

    if (text == "/ledon")
    {
      digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/ledoff")
    {
      ledStatus = 0;
      digitalWrite(ledPin, HIGH); // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/status")
    {
      if (ledStatus)
      {
        bot.sendMessage(chat_id, "Led is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    if (text == "/start")
    {
      String welcome = "Hy, " + from_name + ".\n";
      welcome += "This is our Telegram BOT that test ESP8266/ESP32 microcontrollers.\n\n";
      welcome += "/ledon : to switch the internal Led ON\n";
      welcome += "/ledoff : to switch the internal Led OFF\n";
      welcome += "/status : Returns current status of LED\n";
      welcome += "/temp : Returns temperature of ROOM where MCU was installed\n";
      welcome += "/strip NUM COLOR: change Pixel Color, NUM is a value between 1-20 and COLOR must be in HEXADECIMAL like #FF00FF\n";
      welcome += "/clear : **Admin** Turn all led strip off * Only Chat Owner can execute!\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void bot_setup()
{
  const String commands = F("["
                            "{\"command\":\"ledon\",  \"description\":\"Switch the internal Led ON\"},"
                            "{\"command\":\"ledoff\", \"description\":\"Switch the internal Led OFF\"},"
                            "{\"command\":\"status\", \"description\":\"Returns current status of LED\"},"
                            "{\"command\":\"temp\", \"description\":\"Returns temperature of ROOM where MCU was installed\"},"
                            "{\"command\":\"strip\", \"description\":\"Change Pixel Color, 2 arguments are needed: NUM is a value between 1-20 and COLOR must be in HEXADECIMAL e.g. **/strip 2 #FF0000** - Turn Pixel #02 Red\"},"
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
  pixels.setPixelColor(0, pixels.Color(0, 150, 0));
  pixels.show();

  // Conectando no WiFiClientSecure
// attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot_setup();

  getRGB("#FF00FF");
}

void loop() {

  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}