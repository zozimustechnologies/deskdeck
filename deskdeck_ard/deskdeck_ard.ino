#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= Buttons =================
const int BTN_UP = 2;
const int BTN_DOWN = 3;
const int BTN_OK = 4;
const int BTN_BACK = 5;

// ================= Main Menu =================
String mainMenu[] = {
  "Media",
  "Browser",
  "Steam",
  "System Stats"
};

int mainIndex = 0;

// ================= Media Menu =================
String mediaMenu[] = {
  "Play/Pause",
  "Next Track",
  "Prev Track"
};

int mediaIndex = 0;

// ================= Browser Menu =================
String browserMenu[] = {
  "Safari",
  "Edge"
};

int browserIndex = 0;

// ================= Browser States =================
bool safariOpen = false;
bool edgeOpen = false;

// ================= Stats Menu =================
String statsMenu[] = {
  "CPU Load",
  "RAM Usage",
  "Storage"
};

int statsIndex = 0;

// ================= States =================
bool inMediaMenu = false;
bool inBrowserMenu = false;
bool inStatsMenu = false;
bool viewingStats = false;

// ================= Timing =================
unsigned long lastPress = 0;
const int debounce = 180;

// ================= Setup =================
void setup() {

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

  bootScreen();
  drawMainMenu();
}

// ================= Main Loop =================
void loop() {

  handleButtons();

  // ================= RECEIVE SERIAL =================
  while (Serial.available()) {

    String incoming = Serial.readStringUntil('\n');

    incoming.trim();

    // ================= BROWSER STATUS =================
    if (incoming == "SAFARI_OPEN") {

      safariOpen = true;

      if (inBrowserMenu)
        drawBrowserMenu();
    }

    else if (incoming == "SAFARI_CLOSED") {

      safariOpen = false;

      if (inBrowserMenu)
        drawBrowserMenu();
    }

    else if (incoming == "EDGE_OPEN") {

      edgeOpen = true;

      if (inBrowserMenu)
        drawBrowserMenu();
    }

    else if (incoming == "EDGE_CLOSED") {

      edgeOpen = false;

      if (inBrowserMenu)
        drawBrowserMenu();
    }

    // ================= STATS =================
    else if (viewingStats) {

      int splitIndex = incoming.indexOf('|');

      String line1 = incoming.substring(0, splitIndex);
      String line2 = incoming.substring(splitIndex + 1);

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print(line1);

      lcd.setCursor(0, 1);
      lcd.print(line2);
    }
  }

  // ================= LIVE STATS =================
  if (viewingStats) {

    switch (statsIndex) {

      case 0:
        Serial.println("GET_CPU");
        break;

      case 1:
        Serial.println("GET_RAM");
        break;

      case 2:
        Serial.println("GET_STORAGE");
        break;
    }

    delay(500);
  }
}

// ================= Boot Screen =================
void bootScreen() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("DeskDeck Mini");

  lcd.setCursor(0, 1);
  lcd.print("Booting...");

  delay(2000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("USB Serial OK");

  delay(1000);
}

// ================= Main Menu =================
void drawMainMenu() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("> ");
  lcd.print(mainMenu[mainIndex]);

  int next = mainIndex + 1;

  if (next > 3)
    next = 0;

  lcd.setCursor(0, 1);
  lcd.print("  ");
  lcd.print(mainMenu[next]);
}

// ================= Media Menu =================
void drawMediaMenu() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("> ");
  lcd.print(mediaMenu[mediaIndex]);

  int next = mediaIndex + 1;

  if (next > 2)
    next = 0;

  lcd.setCursor(0, 1);
  lcd.print("  ");
  lcd.print(mediaMenu[next]);
}

// ================= Browser Menu =================
void drawBrowserMenu() {

  lcd.clear();

  lcd.setCursor(0, 0);

  if (browserIndex == 0) {

    if (safariOpen)
      lcd.print("> Close Safari");
    else
      lcd.print("> Open Safari");

    lcd.setCursor(0, 1);

    if (edgeOpen)
      lcd.print("  Close Edge");
    else
      lcd.print("  Open Edge");
  }

  else {

    if (edgeOpen)
      lcd.print("> Close Edge");
    else
      lcd.print("> Open Edge");

    lcd.setCursor(0, 1);

    if (safariOpen)
      lcd.print("  Close Safari");
    else
      lcd.print("  Open Safari");
  }
}

// ================= Stats Menu =================
void drawStatsMenu() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("> ");
  lcd.print(statsMenu[statsIndex]);

  int next = statsIndex + 1;

  if (next > 2)
    next = 0;

  lcd.setCursor(0, 1);
  lcd.print("  ");
  lcd.print(statsMenu[next]);
}

// ================= Buttons =================
void handleButtons() {

  if (millis() - lastPress < debounce)
    return;

  // ================= BACK =================
  if (!digitalRead(BTN_BACK)) {

    if (viewingStats) {

      viewingStats = false;
      drawStatsMenu();

    } else if (inStatsMenu) {

      inStatsMenu = false;
      drawMainMenu();

    } else if (inMediaMenu) {

      inMediaMenu = false;
      drawMainMenu();

    } else if (inBrowserMenu) {

      inBrowserMenu = false;
      drawMainMenu();
    }

    lastPress = millis();
  }

  // ================= VIEWING STATS =================
  if (viewingStats)
    return;

  // ================= STATS MENU =================
  if (inStatsMenu) {

    if (!digitalRead(BTN_UP)) {

      statsIndex--;

      if (statsIndex < 0)
        statsIndex = 2;

      drawStatsMenu();

      lastPress = millis();
    }

    if (!digitalRead(BTN_DOWN)) {

      statsIndex++;

      if (statsIndex > 2)
        statsIndex = 0;

      drawStatsMenu();

      lastPress = millis();
    }

    if (!digitalRead(BTN_OK)) {

      viewingStats = true;

      lastPress = millis();
    }

    return;
  }

  // ================= BROWSER MENU =================
  if (inBrowserMenu) {

    if (!digitalRead(BTN_UP)) {

      browserIndex--;

      if (browserIndex < 0)
        browserIndex = 1;

      drawBrowserMenu();

      lastPress = millis();
    }

    if (!digitalRead(BTN_DOWN)) {

      browserIndex++;

      if (browserIndex > 1)
        browserIndex = 0;

      drawBrowserMenu();

      lastPress = millis();
    }

    if (!digitalRead(BTN_OK)) {

      executeBrowser();

      lastPress = millis();
    }

    return;
  }

  // ================= MEDIA MENU =================
  if (inMediaMenu) {

    if (!digitalRead(BTN_UP)) {

      mediaIndex--;

      if (mediaIndex < 0)
        mediaIndex = 2;

      drawMediaMenu();

      lastPress = millis();
    }

    if (!digitalRead(BTN_DOWN)) {

      mediaIndex++;

      if (mediaIndex > 2)
        mediaIndex = 0;

      drawMediaMenu();

      lastPress = millis();
    }

    if (!digitalRead(BTN_OK)) {

      executeMedia();

      lastPress = millis();
    }

    return;
  }

  // ================= MAIN MENU =================
  if (!digitalRead(BTN_UP)) {

    mainIndex--;

    if (mainIndex < 0)
      mainIndex = 3;

    drawMainMenu();

    lastPress = millis();
  }

  if (!digitalRead(BTN_DOWN)) {

    mainIndex++;

    if (mainIndex > 3)
      mainIndex = 0;

    drawMainMenu();

    lastPress = millis();
  }

  if (!digitalRead(BTN_OK)) {

    executeMain();

    lastPress = millis();
  }
}

// ================= Main Actions =================
void executeMain() {

  switch (mainIndex) {

    case 0:

      inMediaMenu = true;
      drawMediaMenu();

      break;

    case 1:

      inBrowserMenu = true;
      drawBrowserMenu();

      break;

    case 2:

      lcd.clear();
      lcd.print("Opening Steam");

      Serial.println("OPEN_STEAM");

      delay(1200);

      drawMainMenu();

      break;

    case 3:

      inStatsMenu = true;
      drawStatsMenu();

      break;
  }
}

// ================= Browser Actions =================
void executeBrowser() {

  lcd.clear();

  switch (browserIndex) {

    case 0:

      if (safariOpen) {

        lcd.print("Closing Safari");
        Serial.println("CLOSE_SAFARI");

      } else {

        lcd.print("Opening Safari");
        Serial.println("OPEN_SAFARI");
      }

      break;

    case 1:

      if (edgeOpen) {

        lcd.print("Closing Edge");
        Serial.println("CLOSE_EDGE");

      } else {

        lcd.print("Opening Edge");
        Serial.println("OPEN_EDGE");
      }

      break;
  }

  delay(800);

  drawBrowserMenu();
}

// ================= Media Actions =================
void executeMedia() {

  lcd.clear();

  switch (mediaIndex) {

    case 0:

      lcd.print("Play/Pause");

      Serial.println("MEDIA_PLAY");

      break;

    case 1:

      lcd.print("Next Track");

      Serial.println("MEDIA_NEXT");

      break;

    case 2:

      lcd.print("Prev Track");

      Serial.println("MEDIA_PREV");

      break;
  }

  delay(800);

  drawMediaMenu();
}