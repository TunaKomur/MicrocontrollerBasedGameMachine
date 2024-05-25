#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define INITIAL_LIVES 3
int lives = INITIAL_LIVES;

#define NUM_OPTIONS 2
const char *options[NUM_OPTIONS] = {"Baslat", "Cikis"};
int selectedOption = 0;

#define BUTTON_UP_PIN 11
#define BUTTON_DOWN_PIN 12
#define BUTTON_SELECT_PIN 10
#define DEBOUNCE_DELAY 200

#define BLOCK_WIDTH 10 // Blok genişliği
#define BLOCK_HEIGHT 6 // Blok yüksekliği
#define NUM_BLOCKS_X (SCREEN_WIDTH / BLOCK_WIDTH) // Blok sayısı X ekseni boyunca
#define NUM_BLOCKS_Y 2 // Blok sayısı Y ekseni boyunca
bool blocks[NUM_BLOCKS_X][NUM_BLOCKS_Y];

int barPosition = SCREEN_WIDTH / 2;
#define POTENTIOMETER_PIN A0
#define POTENTIOMETER_MIN 0
#define POTENTIOMETER_MAX 1023

float ballX = SCREEN_WIDTH / 2;
float ballY = SCREEN_HEIGHT / 2;
float ballSpeedX = 3;
float ballSpeedY = 3;
int ballRadius = 3;

#define LED_1_PIN 13
#define LED_2_PIN 8
#define LED_3_PIN 9
#define SEVENSEGMENT_MAX 9 // Yedi segmentin gösterebileceği maksimum değer
int score = 0; // Blok kırıldıkça artacak olan skor değişkeni#define SEVENSEGMENT_MAX 9 // Yedi segmentin gösterebileceği maksimum değer
#define TORCH_BUTTON_PIN A1 // Fener düğmesinin bağlı olduğu pin
 bool torchState = false ;

int a = 7;
int b = 6;
int c = 5;
int d = 4;
int e = 3;
int f = 2;
int g = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(TORCH_BUTTON_PIN, INPUT_PULLUP); // Düğmeyi giriş olarak ayarla ve dahili pull-up direnç kullan


  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Menü");
  display.display();
  generateBlocks();
  digitalWrite(LED_1_PIN, HIGH);
  digitalWrite(LED_2_PIN, HIGH);
  digitalWrite(LED_3_PIN, HIGH);
  }

void loop() {
  display.clearDisplay();
  display.setCursor(0, 0);
  
 if (digitalRead(TORCH_BUTTON_PIN) == LOW) {
 // goto tunaosman;
    // Düğme basıldıysa fener durumunu tersine çevir
    torchState = !torchState;
    toggleDisplayColors(torchState); // Ekran renklerini tersine çeviren işlevi çağır
    delay(200); // Buton debouncing için kısa bir gecikme ekle
  }
  
  display.println("Menü");
  for (int i = 0; i < NUM_OPTIONS; i++) {
    if (selectedOption == i) {
      display.print(">");
    } else {
      display.print(" ");
    }
    display.println(options[i]);
  }
  display.display();
  if (digitalRead(BUTTON_UP_PIN) == LOW) {
    selectedOption = (selectedOption - 1 + NUM_OPTIONS) % NUM_OPTIONS;
    delay(DEBOUNCE_DELAY);
  }
  if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
    selectedOption = (selectedOption + 1) % NUM_OPTIONS;
    delay(DEBOUNCE_DELAY);
  }
  if (digitalRead(BUTTON_SELECT_PIN) == LOW) {
    executeSelectedOption(selectedOption);
    delay(DEBOUNCE_DELAY);
  }
  if (Serial.available() > 0) {
    char key = Serial.read();
    if (key == '\n') {
      executeSelectedOption(selectedOption);
    }
  }
}


void toggleDisplayColors(bool reverseColors) {
  if (reverseColors) {
    display.invertDisplay(true); // Ekranın rengini tersine çevir (beyazı siyah yap, siyahı beyaz yap)
  } else {
    display.invertDisplay(false); // Ekranın rengini normal hale getir (beyazı beyaz yap, siyahı siyah yap)
  }
}

void executeSelectedOption(int option) {
  if (option == 0) {
    startGame();
  } else if (option == 1) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Oyunumuza gosterdiginiz ilgi icin tesekkurler");
    display.display();
    delay(2000);
    while (true);
  }
}

int level = 2;
void startGame() {

  // Topun hızına göre blok boyutunu belirle
  int blockSizeFactor = int(ballSpeedX / 3); // Topun hızına bağlı olarak blok boyutunu belirle
  int currentBlockWidth = BLOCK_WIDTH + blockSizeFactor;
  int currentBlockHeight = BLOCK_HEIGHT + blockSizeFactor;
   ballX = SCREEN_WIDTH / 2; // Topun X pozisyonunu ekranın ortasına ayarla
  ballY = SCREEN_HEIGHT - BLOCK_HEIGHT - ballRadius; // Topun Y pozisyonunu barın üstüne ayarla

  while (true) {
    display.clearDisplay();
    drawBlocks();
    readPotentiometer();
    drawBar();
    moveBall();
    drawBall();
    display.display();
    updateSevenSegment(); // Yedi segmenti güncelle
    delay(50);
    if (checkLevelClear()) { // Eğer tüm bloklar kırıldıysa
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Level ");
      display.println(level); // Level numarasını ekrana yazdır
      display.display();
      delay(2000); // 3 saniye bekle
      level++; // Leveli artır
      increaseGameSpeed(); // Oyun hızını artır
      generateBlocks(); // Yeni bloklar oluştur
      break; // Döngüyü sonlandır
    }
  }
}
bool checkLevelClear() {
  for (int x = 0; x < NUM_BLOCKS_X; x++) {
    for (int y = 0; y < NUM_BLOCKS_Y; y++) {
      if (blocks[x][y]) {
        return false; // Hala kırılmamış blok bulundu, level tamamlanmamış
      }
    }
  }
  return true; // Hiç kırılmamış blok kalmadı, level tamamlandı
}


void increaseGameSpeed() {
  // Her seviye için topun hızına 0.2 ekleyin
  ballSpeedX += ballSpeedX*20/100;
  ballSpeedY += ballSpeedY*20/100;
}

void generateBlocks() {
  // Blokların boyutlarını topun hızına göre ayarla
  int blockSizeFactor = int(ballSpeedX / 3); // Topun hızına bağlı olarak blok boyutunu belirle
  int currentBlockWidth = BLOCK_WIDTH + blockSizeFactor;
  int currentBlockHeight = BLOCK_HEIGHT + blockSizeFactor;

  for (int x = 0; x < NUM_BLOCKS_X; x++) {
    for (int y = 0; y < NUM_BLOCKS_Y; y++) {
      blocks[x][y] = random(2);
    }
  }
}

void drawBlocks() {
  for (int x = 0; x < NUM_BLOCKS_X; x++) {
    for (int y = 0; y < NUM_BLOCKS_Y; y++) {
      if (blocks[x][y]) {
        // Bloğun sol, üst, sağ ve alt kenarlarını çiz
        display.drawFastHLine(x * BLOCK_WIDTH, y * BLOCK_HEIGHT, BLOCK_WIDTH, SSD1306_WHITE);
        display.drawFastHLine(x * BLOCK_WIDTH, (y + 1) * BLOCK_HEIGHT - 1, BLOCK_WIDTH, SSD1306_WHITE);
        display.drawFastVLine(x * BLOCK_WIDTH, y * BLOCK_HEIGHT, BLOCK_HEIGHT, SSD1306_WHITE);
        display.drawFastVLine((x + 1) * BLOCK_WIDTH - 1, y * BLOCK_HEIGHT, BLOCK_HEIGHT, SSD1306_WHITE);
  }
}
}
}
void drawBar() {
  // Çubuğun yalnızca üst kısmını çiz
  display.fillRect(barPosition, SCREEN_HEIGHT - BLOCK_HEIGHT, BLOCK_WIDTH * 3, BLOCK_HEIGHT, SSD1306_WHITE);
}
float objectX = -100;
float objectY = -100; // Yükseklik (dikey) konumu
float objectSpeedY = 2; // Nesnenin yavaş yavaş aşağı düşmesi için hız

void moveObject() {
  // Nesneyi yukarıya hareket ettir
  objectY += objectSpeedY;

  // Nesneyi ekranın alt sınırına çarptığını kontrol et
  if (objectY >= SCREEN_HEIGHT - 10) { // Eğer nesne ekranın alt kısmına ulaştıysa
    objectY = SCREEN_HEIGHT - 10; // Nesneyi ekranın altına oturt
    return; // Nesne ekranın en altına geldiğinde hareketi durdur
  }
  
  // Nesne ile çubuğun çarpışmasını kontrol et
  if (objectY + 10 >= SCREEN_HEIGHT - BLOCK_HEIGHT && objectY <= SCREEN_HEIGHT) {
    if (objectX >= barPosition && objectX <= barPosition + (BLOCK_WIDTH * 3)) {
      // Nesneyi ekran dışına çıkararak sil
      objectX = -20; // Ekran dışına yerleştir
      objectY = -20;
      drawObject(); // Silmek için tekrar çiz
      if(lives<3)
      lives++;
            updateLivesIndicator();


      return; // Nesne silindiği için fonksiyondan çık
    }
  }

  // Nesneyi çiz
  drawObject();
}

void drawObject() {
  // Kalbin dış çizgisini çiz
  display.drawLine(objectX + 3, objectY, objectX + 6, objectY, SSD1306_WHITE);
  display.drawLine(objectX + 2, objectY + 1, objectX + 7, objectY + 1, SSD1306_WHITE);
  display.drawLine(objectX + 1, objectY + 2, objectX + 8, objectY + 2, SSD1306_WHITE);
  display.drawLine(objectX, objectY + 3, objectX + 9, objectY + 3, SSD1306_WHITE);
  display.drawLine(objectX, objectY + 4, objectX + 9, objectY + 4, SSD1306_WHITE);
  display.drawLine(objectX + 1, objectY + 5, objectX + 8, objectY + 5, SSD1306_WHITE);
  display.drawLine(objectX + 2, objectY + 6, objectX + 7, objectY + 6, SSD1306_WHITE);
  display.drawLine(objectX + 3, objectY + 7, objectX + 6, objectY + 7, SSD1306_WHITE);
  
  // Kalbin iç kısmını boş bırak
  display.drawLine(objectX + 4, objectY + 1, objectX + 5, objectY + 1, SSD1306_BLACK);
  display.drawLine(objectX + 3, objectY + 2, objectX + 6, objectY + 2, SSD1306_BLACK);
  display.drawLine(objectX + 2, objectY + 3, objectX + 7, objectY + 3, SSD1306_BLACK);
  display.drawLine(objectX + 2, objectY + 4, objectX + 7, objectY + 4, SSD1306_BLACK);
  display.drawLine(objectX + 3, objectY + 5, objectX + 6, objectY + 5, SSD1306_BLACK);
  display.drawLine(objectX + 4, objectY + 6, objectX + 5, objectY + 6, SSD1306_BLACK);
}


void moveBall() {
  // Topun pozisyonunu güncelle
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Nesne hareketini çağır
  moveObject();
  
  // Topun ekran sınırlarına çarpıp çarpmadığını kontrol et
  if (ballX - ballRadius <= 0 || ballX + ballRadius >= SCREEN_WIDTH) {
    ballSpeedX = -ballSpeedX; // X hızını tersine çevir
  }
  
  // Topun üst sınıra çarpıp çarpmadığını kontrol et
  if (ballY - ballRadius <= 0) {
    ballSpeedY = -ballSpeedY; // Y hızını tersine çevir
    ballY = ballRadius; // Topu üst sınırın içine it
  }
  
  // Topun alt sınırı geçip geçmediğini kontrol et (kaybetme durumu)
  if (ballY + ballRadius >= SCREEN_HEIGHT) {
    // Canı azalt
    lives--;
    // Canı kontrol et
    if (lives >0) {
      // Can varsa topu başlangıç konumuna geri döndür
      ballX = barPosition + (BLOCK_WIDTH * 3) / 2; // Topun başlangıç x pozisyonunu belirle
      ballY = SCREEN_HEIGHT - BLOCK_HEIGHT - ballRadius; // Topun başlangıç y pozisyonunu belirle
      ballSpeedY = -ballSpeedY; // Y hızını tersine çevir

      // Can göstergesini güncelle
      updateLivesIndicator();
    } else {
      // Can yoksa oyunu bitir
      gameOver();
      return;
    }
  }
  
  // Çubuğun üst kısmıyla ve alt sınırıyla çarpışmayı kontrol et
  if (ballY + ballRadius >= SCREEN_HEIGHT - BLOCK_HEIGHT && ballY - ballRadius <= SCREEN_HEIGHT) {
    if (ballX >= barPosition && ballX <= barPosition + (BLOCK_WIDTH * 3)) {
      ballSpeedY = -ballSpeedY; // Y hızını tersine çevir
      ballY = SCREEN_HEIGHT - BLOCK_HEIGHT - ballRadius; // Topu çubuğun üstüne it
    }
  }
  
  // Bloklarla olan çarpışmayı kontrol et
  for (int y = 0; y < NUM_BLOCKS_Y; y++) {
    for (int x = 0; x < NUM_BLOCKS_X; x++) {
      if (blocks[x][y]) {
        int blockLeft = x * BLOCK_WIDTH;
        int blockRight = blockLeft + BLOCK_WIDTH;
        int blockTop = y * BLOCK_HEIGHT;
        int blockBottom = blockTop + BLOCK_HEIGHT;
        
        // Blokla teması kontrol et
        if (ballX + ballRadius >= blockLeft && ballX - ballRadius <= blockRight && ballY + ballRadius >= blockTop && ballY - ballRadius <= blockBottom) {
          // Blok kırıldığında skoru artır,
          score++;
          // Bloğu kır
          blocks[x][y] = false;

          if (random(3) == 1) {
          // Blok kırıldığında skoru artır
          
          // Nesneyi bloğun kırıldığı yerden başlat
          objectX = blockLeft + BLOCK_WIDTH / 2;
          objectY = blockTop + BLOCK_HEIGHT;
          // Nesneyi hareket ettirme fonksiyonunu çağır
          moveObject();
          drawObject();
        }
          // Çarpışma durumuna göre topun yönünü değiştir
          if (ballX >= blockLeft && ballX <= blockRight) {
            ballSpeedY = -ballSpeedY; // Y hızını tersine çevir
          } else if (ballY >= blockTop && ballY <= blockBottom) {
            ballSpeedX = -ballSpeedX; // X hızını tersine çevir
          } else {
            // Çarpışma durumuna göre topun yönünü değiştir
            if (ballX < blockLeft && ballY < blockTop) {
              ballSpeedX = -ballSpeedX; // X hızını tersine çevir
              ballSpeedY = -ballSpeedY; // Y hızını tersine çevir
            } else if (ballX > blockRight && ballY < blockTop) {
              ballSpeedX = -ballSpeedX; // X hızını tersine çevir
              ballSpeedY = -ballSpeedY; // Y hızını tersine çevir
            } else if (ballX < blockLeft && ballY > blockBottom) {
              ballSpeedX = -ballSpeedX; // X hızını tersine çevir
              ballSpeedY = -ballSpeedY; // Y hızını tersine çevir
            } else if (ballX > blockRight && ballY > blockBottom) {
              ballSpeedX = -ballSpeedX; // X hızını tersine çevir
              ballSpeedY = -ballSpeedY; // Y hızını tersine çevir
            }
          }
          break; // Blok kırıldığında döngüden çık
        }
      }
    }
  }
   // Yedi segmenti güncelle (her çarpışma sonrası)
  updateSevenSegment();
}
#define MENU_TIMEOUT 3000 // 3 saniye

void gameOver() {
  digitalWrite(LED_1_PIN, LOW);
  unsigned long gameOverTime = millis(); // Oyun bittiğindeki zamanı kaydet
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Oyun Bitti");
  display.setCursor(0, 10); // Skoru biraz aşağıya kaydır
  display.print("Skor: ");
  display.println(score);
    display.display();

   delay(MENU_TIMEOUT); // Belirtilen süre boyunca bekleyin
 display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Menü");
  for (int i = 0; i < NUM_OPTIONS; i++) {
    if (selectedOption == i) {
      display.print(">");
    } else {
      display.print(" ");
    }
    display.println(options[i]);
  }
  display.display();
  delay(2000);
  digitalWrite(LED_1_PIN, LOW); // Tüm LED'leri sıfırla
  digitalWrite(LED_2_PIN, LOW);
  digitalWrite(LED_3_PIN, LOW);
  while (true);
}
 

void drawBall() {
  display.fillCircle(ballX, ballY, ballRadius, SSD1306_WHITE);
}

void readPotentiometer() {
  int potValue = analogRead(POTENTIOMETER_PIN);
  barPosition = map(potValue, POTENTIOMETER_MIN, POTENTIOMETER_MAX, 0, SCREEN_WIDTH - (BLOCK_WIDTH * 3));
}

void updateLivesIndicator() {
  switch (lives) {
    case 3:
      digitalWrite(LED_1_PIN, HIGH);
      digitalWrite(LED_2_PIN, HIGH);
      digitalWrite(LED_3_PIN, HIGH);
      break;
    case 2:
      digitalWrite(LED_1_PIN, HIGH);
      digitalWrite(LED_2_PIN, HIGH);
      digitalWrite(LED_3_PIN, LOW);
      break;
    case 1:
      digitalWrite(LED_1_PIN, HIGH);
      digitalWrite(LED_2_PIN, LOW);
      digitalWrite(LED_3_PIN, LOW);
      break;
    case 0:
      digitalWrite(LED_1_PIN, LOW);
      digitalWrite(LED_2_PIN, LOW);
      digitalWrite(LED_3_PIN, LOW);
      break;
    default:
      break;
  }
}

void updateSevenSegment() {
   
  int score_seven = score%10;
  switch (score_seven) {
    case 0:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, LOW);
      break;
    case 1:
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
      break;
    case 2:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, LOW);
      digitalWrite(g, HIGH);
      break;
    case 3:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, HIGH);
      break;
    case 4:
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
    case 5:
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, LOW);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
    case 6:
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
    case 7:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
      break;
    case 8:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
    case 9:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, LOW);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
    default:
break;
}
}