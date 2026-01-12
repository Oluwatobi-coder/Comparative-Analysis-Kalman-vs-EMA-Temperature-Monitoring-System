/*
 * Project: Implementation of Kalman and EMA Filters for a Low-Cost Temperature Monitoring System
 * Author: Bello Oluwatobi
 * Last Updated: January 11, 2026
 * Hardware: Arduino Mega 2560, DHT11, Active Buzzer, LED, Standard LCD (16x2)
 * * Description: 
 * Implements a 1D Kalman Filter and Exponential Moving Average (EMA) 
 * to process noisy temperature data. Triggers audio-visual alarm (Buzzer & Alarm)
 * when filtered temperature value crosses the set threshold (T = 40 degC) 
 */

#include "DHT.h"
#include <LiquidCrystal.h> // Standard Library

// defining the hardware
#define DHTPIN 2       
#define DHTTYPE DHT11  
#define BUZZER_PIN 8   
#define LED_PIN 13     

// setting up the LCD for filtered temperature display
// mapping the pins: RS->12, E->11, D4->7, D5->6, D6->5, D7->4
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

// setting the temperature threshold
const float TEMP_THRESHOLD = 40.0; 

// 2. EMA Filter Variables
float Temp_EMA = 0;       // The filtered result
float alpha = 0.05;         // Smoothing Factor (0.1 = Very smooth/slow, 0.9 = Fast/noisy)

// setting the kalman filter variables
float R = 2.0;   // measurement noise covariance 
float Q = 0.05;  // process noise covariance 
float P = 1.0;   // estimation error covariance 
float Temp_K = 0;    // estimated state 
float K = 0;     // kalman gain

// setting the time check variable
unsigned long previousTime = 0;

// setting the sampling time for the system
const long interval = 2000; 

// defining the DHT11 class
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // setting the baud rate
  Serial.begin(9600);

  Serial.println("Temp_Raw,EMA_Filter,Kalman_Filter,Threshold");

  // initializing the DHT11 temperature sensor
  dht.begin();
  
  // specifying the active buzzer and LED output pins 
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // initializing LCD columns and rows
  lcd.begin(16, 2); 
  
  // printing to LCD
  lcd.setCursor(0,0);
  lcd.print("System Start...");
  
  // reading initial temperature value from the sensor
  float initialTemp = dht.readTemperature();

  // initial feeding of sensor reading to estimated state variable
  if (!isnan(initialTemp)) {
    Temp_K = initialTemp;
    Temp_EMA = initialTemp;       
  }
  
  // applying a 2s delay and clearing the screen
  delay(2000);
  lcd.clear();
}

void loop() {
  // getting the current system time
  unsigned long currentTime = millis();

  // performing temperature signal processing after every 2s 
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;

    delay(2000);
    // getting raw temperature value from sensor
    float Temp_Raw = dht.readTemperature();
    
    // printing error message to screen when there is a faulty value
    if (isnan(Temp_Raw)) {
      lcd.setCursor(0,0);
      lcd.print("Sensor Error!");
      return;
    }

    // if (!isnan(Temp_Raw)) {
    //   Temp_K = Temp_Raw;
    //   Temp_EMA = Temp_Raw;       
    // }

    // --- STEP 2: ALGORITHM A - EXPONENTIAL MOVING AVERAGE (EMA) ---
    // Logic: New Average = (Weight * New Input) + ((1-Weight) * Old Average)
    Temp_EMA = (alpha * Temp_Raw) + ((1.0 - alpha) * Temp_EMA);

    // carrying out signal processing using 1D kalman filter
    float Temp_K_pred = Temp_K;      
    float P_pred = P + Q;    

    K = P_pred / (P_pred + R);           
    Temp_K = Temp_K_pred + K * (Temp_Raw - Temp_K_pred); 
    P = (1 - K) * P_pred;                

    // clearing LCD screen for temperature display
    lcd.clear();
    
    // displaying the filtered temperature value on LCD 1st line
    lcd.setCursor(0,0);   
    lcd.print("T_Kalman: ");
    lcd.print(Temp_K); 
    lcd.print("C");

    // displaying system status based on temperature threshold value on LCD 2nd line
    lcd.setCursor(0,1);
    if (Temp_K >= TEMP_THRESHOLD) {
      // specifying actions and display text during critical state
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      lcd.print("STATUS: CRITICAL");
    } else {
      // specifying actions and display text during normal state
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      lcd.print("STATUS: NORMAL");
    }

    // printing values to Serial Plotter and Serial Monitor
    Serial.print("Raw:"); 
    Serial.print(Temp_Raw);
    Serial.print(",");
    Serial.print("EMA:"); 
    Serial.print(Temp_EMA);
    Serial.print(",");
    Serial.print("Kalman:"); 
    Serial.print(Temp_K);
    Serial.print(",");
    Serial.print("Threshold:"); 
    Serial.println(TEMP_THRESHOLD);
  }
}