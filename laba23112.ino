// Битовые маски цифр 0..9 для порядка сегментов {A, F, B, E, D, DP, C, G}
const uint8_t digits[10] = {
  0b11111010, // 0
  0b00100010, // 1
  0b10111001, // 2
  0b10101011, // 3
  0b01100011, // 4
  0b11001011, // 5
  0b11011011, // 6
  0b10100010, // 7
  0b11111011, // 8
  0b11101011  // 9
};

// Пины сегментов и разрядов
const uint8_t G = 5, C = 6, DP = 7, D = 8, E = 9, B = 10, F = 11, A = 12;
const uint8_t segPins[8] = {A, F, B, E, D, DP, C, G};

const int digit1 = 1; // замкнутый разряд — не задействуем
const int digit2 = 3;
const int digit3 = 2;
const int digit4 = 4;

int number = 0; // текущее число из Serial Monitor

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 8; i++) {
    pinMode(segPins[i], 1); // 1 = OUTPUT
    digitalWrite(segPins[i], 0);
  }

  pinMode(digit1, 1);
  pinMode(digit2, 1);
  pinMode(digit3, 1);
  pinMode(digit4, 1);

  digitalWrite(digit1, 0);
  digitalWrite(digit2, 0);
  digitalWrite(digit3, 0);
  digitalWrite(digit4, 0);

  Serial.println("Введите число (до 4 цифр):");
}

// Выводит одну цифру (0..9) в выбранный разряд
void showDigit(int digitPin, int val) {
  uint8_t mask = digits[val];

  for (int i = 0; i < 8; i++) {
    int bitState = (mask >> (7 - i)) & 1;
    digitalWrite(segPins[i], bitState);
  }

  digitalWrite(digitPin, 1);
  delay(3);
  digitalWrite(digitPin, 0);
}

void loop() {
  digitalWrite(digit1, 0);

  if (Serial.available() > 0) {
    String text = Serial.readStringUntil('\n');
    text.trim();
    if (text.length() > 0) {
      number = text.toInt() % 10000;
      Serial.print("Введено число: ");
      Serial.println(number);
    }
  }

  int lastThree = number % 1000;
  int hundreds  = (lastThree / 100) % 10;
  int tens      = (lastThree / 10) % 10;
  int units     = lastThree % 10;

  // Слева направо по активным разрядам: сотни -> десятки -> единицы
  showDigit(digit4, hundreds);
  showDigit(digit3, tens);
  showDigit(digit2, units);
}