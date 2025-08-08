#include <LiquidCrystal.h>

// LCD Pinleri
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Joystick Pinleri
const int joyX = A0;
const int joyY = A1;

// Oyun Değişkenleri
bool gameOver = false;
bool gameStarted = false;
unsigned long prevTime = 0;
int gameSpeed = 5; // Başlangıç hızı (birim: hareket/saniye)

// Yılan Segment Yapısı
struct SnakeSegment {
  int x, y;
  int dir; // 0 = yukarı, 1 = aşağı, 2 = sağ, 3 = sol
  SnakeSegment *next;
};

SnakeSegment *head = nullptr;
SnakeSegment *tail = nullptr;

// Oyun Alanı
const int width = 16; // LCD sütun sayısı
const int height = 2; // LCD satır sayısı
int foodX, foodY;

// Fonksiyon Prototipleri
void startGame();
void gameLoop();
void moveAll();
void drawMatrix();
void spawnFood();
bool checkCollision(int x, int y);

void setup() {
  // LCD ve Joystick başlat
  lcd.begin(16, 2);
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);

  startGame();
}

void loop() {
  if (!gameOver && gameStarted) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime > 1000 / gameSpeed) {
      prevTime = currentTime;
      moveAll();
      drawMatrix();
    }

    // Joystick yön kontrolü
    int xVal = analogRead(joyX);
    int yVal = analogRead(joyY);
    int newDir = head->dir;

    if (xVal < 400 && head->dir != 2) newDir = 3; // Sol
    else if (xVal > 600 && head->dir != 3) newDir = 2; // Sağ
    if (yVal < 400 && head->dir != 1) newDir = 0; // Yukarı
    else if (yVal > 600 && head->dir != 0) newDir = 1; // Aşağı

    head->dir = newDir;
  } else if (gameOver) {
    lcd.clear();
    lcd.print("Game Over");
    delay(2000);
    startGame();
  }
}

void startGame() {
  gameOver = false;
  gameStarted = true;

  // Yılan başlat
  head = new SnakeSegment{7, 1, 2, nullptr};
  tail = head;
  spawnFood();

  lcd.clear();
  lcd.print("Game Started!");
  delay(1000);
}

void spawnFood() {
  do {
    foodX = random(0, width);
    foodY = random(0, height);
  } while (checkCollision(foodX, foodY)); // Yılanın üstüne düşmesin
}

bool checkCollision(int x, int y) {
  SnakeSegment *current = head;
  while (current != nullptr) {
    if (current->x == x && current->y == y) return true;
    current = current->next;
  }
  return false;
}

void moveAll() {
  int nextX = head->x;
  int nextY = head->y;

  // Baş kısmının yeni pozisyonu
  switch (head->dir) {
    case 0: nextY--; break; // Yukarı
    case 1: nextY++; break; // Aşağı
    case 2: nextX++; break; // Sağ
    case 3: nextX--; break; // Sol
  }

  // Çarpma kontrolü
  if (nextX < 0 || nextX >= width || nextY < 0 || nextY >= height || checkCollision(nextX, nextY)) {
    gameOver = true;
    return;
  }

  // Yeni kafa segmenti ekle
  SnakeSegment *newHead = new SnakeSegment{nextX, nextY, head->dir, head};
  head = newHead;

  // Yem yedi mi?
  if (nextX == foodX && nextY == foodY) {
    spawnFood();
    gameSpeed++; // Hız artır
  } else {
    // Kuyruk segmentini kaldır
    SnakeSegment *temp = tail;
    tail = tail->next;
    delete temp;
  }
}

void drawMatrix() {
  lcd.clear();

  // Oyun alanını çiz
  SnakeSegment *current = head;
  while (current != nullptr) {
    lcd.setCursor(current->x, current->y);
    lcd.write('#'); // Yılanın parçası
    current = current->next;
  }

  // Yem konumu
  lcd.setCursor(foodX, foodY);
  lcd.write('*'); // Yem
}
