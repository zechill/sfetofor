#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Ошибка запуска SSD1306 по адресу 0x3C"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("OLED готов"));
  display.println(F("Введите строку:"));
  display.display();

  Serial.println(F("Введите строку для вывода на OLED:"));
}

void loop() {
  if (Serial.available() > 0) {
    String inputText = Serial.readStringUntil('\n');
    inputText.trim();

    if (inputText.length() > 0) {
      Serial.print(F("Выведено на OLED: "));
      Serial.println(inputText);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println(inputText);
      display.display();
    }
  }
}