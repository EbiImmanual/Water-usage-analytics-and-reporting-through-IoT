# Water-usage-analytics-and-reporting-through-IoT
An IoT-based water monitoring system built using ESP32 that not only tracks tank water levels but also estimates consumption and generates weekly usage reports. The system integrates real-time sensing, automated motor control, and cloud visualization using Blynk.
🚀 Features
📊 Real-time Water Level Monitoring
📉 Water Usage Estimation (Zone-Based)
📅 Daily Tracking & Weekly Report Generation
📱 Remote Monitoring via Blynk App
⚙️ Automatic Motor Control
🎯 Target-Based Filling System (25%, 50%, 75%)
🖥️ OLED Display Visualization
🧠 System Overview
This system uses an ultrasonic sensor to measure water level inside a tank. Based on level changes, it estimates water consumption using predefined zones and stores:
Daily Usage
7-Day Weekly Usage Report
The ESP32 sends this data to Blynk, where it can be visualized as a graph.
🛠️ Hardware Components
ESP32 Microcontroller
Ultrasonic Sensor (HC-SR04)
Relay Module (Motor Control)
OLED Display (SSD1306)
WiFi Network
🔌 Pin Configuration
Component	ESP32 Pin
Trigger (Ultrasonic)	GPIO 5
Echo (Ultrasonic)	GPIO 18
Relay Module	GPIO 19
OLED (I2C)	SDA/SCL
⚙️ Software & Libraries
Arduino IDE
Blynk IoT Platform
Required Libraries:
WiFi.h
BlynkSimpleEsp32.h
Adafruit_SSD1306.h
Wire.h
📊 Working Principle
1. Water Level Measurement
Ultrasonic sensor measures distance
Converted into percentage (0–100%)
2. Usage Estimation (Core Logic)
Tank divided into zones
When level drops between zones → usage is calculated
Example:
Zone drop → adds predefined liters (125L / 75L)
3. Daily Tracking
Accumulates total usage for the day
4. Weekly Report Generation
Stores 7 days of usage in array
Automatically resets daily
Sent to Blynk via virtual pins V5–V11
5. Motor Control
Manual control via app
Automatic stop when:
Tank reaches 95%, or
Selected target level
📱 Blynk Configuration
Virtual Pin	Function
V0	Water Level (%)
V1	Motor ON/OFF
V2	Target 25%
V3	Target 50%
V4	Target 75%
V5–V11	Weekly Usage Data
🖥️ OLED Display
Displays:
Current water level (%)
Target level
Motor status (ON/OFF)
Graphical level bar
⏱️ Testing Note
In this code:
#define DAY_INTERVAL 10000UL
10 seconds = 1 day (for testing)
Change this to real-time:
#define DAY_INTERVAL 86400000UL  // 24 hours
📈 Key Innovation
Unlike traditional water level systems, this project focuses on:
"Water Consumption Awareness rather than just Monitoring"
Estimates actual usage
Tracks patterns
Enables smarter water management
🧪 Future Improvements
Mobile dashboard with advanced analytics
Leak detection system
AI-based usage prediction
Integration with smart home systems
📌 Project Status
✅ Core system completed
✅ Usage tracking implemented
✅ Weekly reporting working
🔄 UI/Visualization improvements ongoing
👨‍💻 Author
Ebi Immanual
ECE Student | VLSI Enthusiast
