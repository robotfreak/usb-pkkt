/*
    USB-PKKT:
    USB Programmble Keyboard Knob Thingy Example for Teensy

    You must select Serial+Keyboard+Mouse+Joystick from the "Tools > USB Type" menu
*/

#include <Bounce.h>
#include <Encoder.h>
#include <WS2812Serial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>  // https://github.com/wonho-maker/Adafruit_SH1106

const int numled = 3;
const int pin = 1;

// Usable pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33
//   Teensy 4.0:  1, 8, 14, 17, 20, 24, 29, 39

byte drawingMemory[numled * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[numled * 12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);

#define NEO_RED    0xFF0000
#define NEO_GREEN  0x00FF00
#define NEO_BLUE   0x0000FF
#define NEO_YELLOW 0xFFFF00
#define NEO_PINK   0xFF1088
#define NEO_ORANGE 0xE05800
#define NEO_WHITE  0xFFFFFF

Adafruit_SH1106 display(-1);

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knobLeft(11, 12);
Encoder knobRight(9, 10);

// Create Bounce objects for each button.  The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
Bounce button1 = Bounce(6, 10);
Bounce button2 = Bounce(5, 10);  // if a button is too "sensitive"
Bounce button3 = Bounce(4, 10);  // to rapid touch, you can
Bounce button4 = Bounce(8, 10);  // increase this time.
Bounce button5 = Bounce(7, 10);

long positionLeft  = -999;
long positionRight = -999;
  
void setup() {
  Serial.begin(9600);
  Serial.println("USB-PKKT Test v0.1");
  leds.begin();
  Mouse.screenSize(1920, 1080);  // configure screen size
  // Configure the pins for input mode with pullup resistors.
  // The pushbuttons connect from each pin to ground.  When
  // the button is pressed, the pin reads LOW because the button
  // shorts it to ground.  When released, the pin reads HIGH
  // because the pullup resistor connects to +5 volts inside
  // the chip.  LOW for "on", and HIGH for "off" may seem
  // backwards, but using the on-chip pullup resistors is very
  // convenient.  The scheme is called "active low", and it'shttps://www.pjrc.com/teensy/td_libs_Encoder_2.png
  // very commonly used in electronics... so much that the chip
  // has built-in pullup resistors!
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);  // Teensy++ LED, may need 1k resistor pullup
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  // text display tests
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(0, 0);
  display.println("USB-PKKT Test v0.1");
  display.setCursor(0, 20);
  display.println("Sigrok - Pulseview");
  
  display.drawLine(0, 50, 128, 50, WHITE);
  display.drawLine(112, 0, 112, 64, WHITE);
  display.drawLine(112, 25, 128, 25, WHITE);
  display.drawLine(38, 50, 38, 64, WHITE);
  display.drawLine(76, 50, 76, 64, WHITE);
  display.display();
}

void loop() {
  long newLeft, newRight;
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  if (newLeft != positionLeft) {
    Serial.print("Lower = ");
    Serial.print(newLeft);
    Serial.println();
    Keyboard.press(MODIFIERKEY_CTRL);
    if ((newLeft - positionLeft) < 0) {
      Keyboard.press(KEYPAD_MINUS);
      delay(10);
      Keyboard.release(KEYPAD_MINUS);
    }
    else {
      Keyboard.press(KEYPAD_PLUS);
      delay(10);
      Keyboard.release(KEYPAD_PLUS);
    }
    Keyboard.release(MODIFIERKEY_CTRL);
    positionLeft = newLeft;
  }

  if (newRight != positionRight) {
    Serial.print("Lower = ");
    Serial.print(newRight);
    Serial.println();
    if ((newRight - positionRight) < 0) {
      Keyboard.press(KEY_LEFT);
      delay(10);
      Keyboard.release(KEY_LEFT);
    }
    else {
      Keyboard.press(KEY_RIGHT);
      delay(10);
      Keyboard.release(KEY_RIGHT);
    }
    positionRight = newRight;
  }
  // Update all the buttons.  There should not be any long
  // delays in loop(), so this runs repetitively at a rate
  // faster than the buttons could be pressed and released.
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();

  // Check each button for "falling" edge.
  // Type a message on the Keyboard when each button presses
  // Update the Joystick buttons only upon changes.
  // falling = high (not pressed - voltage from pullup resistor)
  //           to low (pressed - button connects pin to ground)
  if (button1.fallingEdge()) {
    setLED(2, NEO_WHITE);
    Serial.println("B1 press");
    Keyboard.press(KEY_SPACE);
  }
  if (button2.fallingEdge()) {
    setLED(1, NEO_WHITE);
    Serial.println("B2 press");
  }
  if (button3.fallingEdge()) {
    setLED(1, NEO_WHITE);
    Serial.println("B3 press");
  }
  if (button4.fallingEdge()) {
    Serial.println("B4 press");
  }
  if (button5.fallingEdge()) {
    Serial.println("B5 press");
  }

  // Check each button for "rising" edge
  // Type a message on the Keyboard when each button releases.
  // For many types of projects, you only care when the button
  // is pressed and the release isn't needed.
  // rising = low (pressed - button connects pin to ground)
  //          to high (not pressed - voltage from pullup resistor)
  if (button1.risingEdge()) {
    setLED(2, 0);
    Serial.println("B1 release");
    Keyboard.release(KEY_SPACE);
  }
  if (button2.risingEdge()) {
    setLED(1, 0);
    Serial.println("B2 release");
  }
  if (button3.risingEdge()) {
    setLED(0, 0);
    Serial.println("B3 release");
  }
  if (button4.risingEdge()) {
    Serial.println("B4 release");
  }
  if (button5.risingEdge()) {
    Serial.println("B5 release");
  }
}

void setLED(int num, int colour) {
  leds.setPixel(num, colour);
  leds.show();

}
