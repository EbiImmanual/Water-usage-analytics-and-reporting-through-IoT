#define BLYNK_TEMPLATE_ID "TMPL3nhkrmene"
#define BLYNK_TEMPLATE_NAME "Water level"
#define BLYNK_AUTH_TOKEN "F9AI-z0hIoxtLap4I8s20HKIIS0oq-3n"

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <BlynkSimpleEsp32.h>

#define RELAY_PIN 19
#define TRIG_PIN 5
#define ECHO_PIN 18

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define MAX_DEPTH_CM 17.0
#define MIN_DISTANCE_CM 3.0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char ssid[] = "Ebi Immanual";
char pass[] = "12345678";

BlynkTimer timer;

bool motorState = false;

int targetLevel = 100;
bool targetMode = false;
int activeButton = 0;

// -------- WATER USAGE TRACKING --------
int prevZone = -1;
float dailyUsage = 0;

// -------- WEEKLY STORAGE --------
float weeklyUsage[7] = {0};
int dayIndex = 0;

unsigned long lastDayTime = 0;
#define DAY_INTERVAL 10000UL   // 1 minute = 1 day (for testing)
// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED init failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Water Monitor Init");
  display.display();
  display.setTextColor(SSD1306_WHITE);

  timer.setInterval(2000L, measureWaterLevel);
}

// ---------------- LOOP ----------------
void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Blynk.run();
  }
  timer.run();
}

// ---------------- SENSOR + LOGIC ----------------
void measureWaterLevel()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  float distance = duration * 0.034 / 2;

  if(distance < MIN_DISTANCE_CM)
    distance = MIN_DISTANCE_CM;

  if(distance > MAX_DEPTH_CM)
    distance = MAX_DEPTH_CM;

  int percent = map(distance, MIN_DISTANCE_CM, MAX_DEPTH_CM, 100, 0);

  Serial.print("Water Level: ");
  Serial.print(percent);
  Serial.println("%");

  // -------- ZONE LOGIC --------
  int currentZone;

  if(percent > 75) currentZone = 0;
  else if(percent > 50) currentZone = 1;
  else if(percent > 25) currentZone = 2;
  else if(percent > 10) currentZone = 3;
  else currentZone = 4;

  if(prevZone == -1)
  {
    prevZone = currentZone;
  }
  else if(currentZone > prevZone)
  {
    float usage = 0;

    if(currentZone == 1) usage = 125;
    else if(currentZone == 2) usage = 125;
    else if(currentZone == 3) usage = 125;
    else if(currentZone == 4) usage = 75;

    dailyUsage += usage;

    Serial.print("Used: ");
    Serial.print(usage);
    Serial.println(" L");

    Serial.print("Total: ");
    Serial.print(dailyUsage);
    Serial.println(" L");

    prevZone = currentZone;
  }
  else if(currentZone < prevZone)
  {
    prevZone = currentZone;
  }

  // -------- DAILY RESET --------
  if(millis() - lastDayTime >= DAY_INTERVAL)
  {
    lastDayTime = millis();

    weeklyUsage[dayIndex] = dailyUsage;

    dayIndex++;
    if(dayIndex >= 7) dayIndex = 0;

    dailyUsage = 0;

    Serial.println("New Day → Reset");
  }

  updateDisplay(percent);

  // -------- TARGET MODE --------
  if(targetMode)
  {
    if(percent >= targetLevel && motorState)
    {
      digitalWrite(RELAY_PIN, HIGH);
      motorState = false;

      if(activeButton == 2) Blynk.virtualWrite(V2, 0);
      if(activeButton == 3) Blynk.virtualWrite(V3, 0);
      if(activeButton == 4) Blynk.virtualWrite(V4, 0);

      Blynk.virtualWrite(V1, 0);
      targetMode = false;
      activeButton = 0;
    }
  }
  else
  {
    if(percent >= 95 && motorState)
    {
      digitalWrite(RELAY_PIN, HIGH);
      motorState = false;
      Blynk.virtualWrite(V1, 0);
    }
  }

  // -------- SEND TO BLYNK --------
  if(WiFi.status() == WL_CONNECTED)
  {
    Blynk.virtualWrite(V0, percent);

    // Weekly data
    Blynk.virtualWrite(V5, weeklyUsage[0]);
    Blynk.virtualWrite(V6, weeklyUsage[1]);
    Blynk.virtualWrite(V7, weeklyUsage[2]);
    Blynk.virtualWrite(V8, weeklyUsage[3]);
    Blynk.virtualWrite(V9, weeklyUsage[4]);
    Blynk.virtualWrite(V10, weeklyUsage[5]);
    Blynk.virtualWrite(V11, weeklyUsage[6]);
  }
}

// ---------------- OLED ----------------
void updateDisplay(int percent)
{
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Lvl:");

  display.setCursor(0,20);
  display.print(percent);
  display.print("%");

  display.setTextSize(1);
  display.setCursor(70,0);
  display.print("T:");
  display.print(targetLevel);
  display.print("%");

  display.setCursor(0,50);
  if(motorState)
    display.print("Motor: ON");
  else
    display.print("Motor: OFF");

  int barHeight = map(percent,0,100,0,40);
  display.drawRect(100,10,20,40,SSD1306_WHITE);
  display.fillRect(100,50-barHeight,20,barHeight,SSD1306_WHITE);

  display.display();
}

// ---------------- MANUAL ----------------
BLYNK_WRITE(V1)
{
  int value = param.asInt();

  if(value == 1)
  {
    digitalWrite(RELAY_PIN, LOW);
    motorState = true;
    targetMode = false;
  }
  else
  {
    digitalWrite(RELAY_PIN, HIGH);
    motorState = false;
  }
}

// ---------------- TARGET BUTTONS ----------------

BLYNK_WRITE(V2)
{
  if(param.asInt() == 1)
  {
    targetLevel = 25;
    targetMode = true;
    activeButton = 2;

    digitalWrite(RELAY_PIN, LOW);
    motorState = true;

    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, 0);
  }
}

BLYNK_WRITE(V3)
{
  if(param.asInt() == 1)
  {
    targetLevel = 50;
    targetMode = true;
    activeButton = 3;

    digitalWrite(RELAY_PIN, LOW);
    motorState = true;

    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V4, 0);
  }
}

BLYNK_WRITE(V4)
{
  if(param.asInt() == 1)
  {
    targetLevel = 75;
    targetMode = true;
    activeButton = 4;

    digitalWrite(RELAY_PIN, LOW);
    motorState = true;

    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, 0);
  }
}