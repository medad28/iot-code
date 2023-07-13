#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
bool isTempDisplayed = true;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// OLED VCC to 3.3V or 5V
// OLED GND to GND
// OLED SDA to GPIO pin (e.g., D2) on ESP8266
// OLED SCL to GPIO pin (e.g., D1) on ESP8266

#define BLYNK_TEMPLATE_ID "TMPL6CQxUBLrP"
#define BLYNK_TEMPLATE_NAME "Temperature and Humidity Monitor"
#define BLYNK_AUTH_TOKEN "PQcmSckHjuwefpuAXl40sDnJL59dh4Jg"

#define DHTPIN 2  // D4 pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "Poco X3 Pro";
char pass[] = "user4321";

BlynkTimer timer;

void displayWiFiStatus()
{
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (WiFi.status() == WL_CONNECTED)
  {
    display.println("WiFi: Connected");
    display.print("IP: ");
    display.println(WiFi.localIP());
  }
  else
  {
    display.println("WiFi: Disconnected");
  }

  display.display();
}

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature() - 4.00;  // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t))
  {
    display.clearDisplay();
    displayWiFiStatus();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println("Sensor Error!");
    display.display();
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Update OLED display
  display.clearDisplay();
  displayWiFiStatus();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  if (isTempDisplayed) {
    display.println("Temp: \n" + String(t) + " C");
  } else {
    display.println("Humidity: " + String(h) + " %");
  }
  display.display();
  isTempDisplayed = !isTempDisplayed;
  // Send values to Blynk
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" C   Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  if (t > 35)
  {
    Blynk.logEvent("temp_alert", "Temp above 35 degrees");
  }
}

void setup()
{
  Serial.begin(115200);

  // Initialize the OLED display
  //display.begin(SSD1306_SWITCHCAPVCC, OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();

  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(1000L, sendSensor); // Perubahan interval menjadi 1000 ms (1 detik)
}

void loop()
{
  Blynk.run();
  timer.run();
}
