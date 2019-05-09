/* Prototype Name : To detect volatiles in the air (H2S in toilet)
   Input Sensor   : TGS2602 sensor
   Output Device  : Multicolor LED
   Libraries Used : LiquidCrystalPCF584 ,FastLED
*/
#include <LiquidCrystal_PCF8574.h> // https://github.com/mathertel/LiquidCrystal_PCF8574
LiquidCrystal_PCF8574 lcd(0x3F); // Check the sticker on the LCD to know the address

#include <FastLED.h> //
#define DATA_PIN 9// Digital pin number for LED (NeoPixel)
#define NUM_LEDS 3
CRGB leds[NUM_LEDS];
int tgs_pin = 1; //Analog A1
int sensorValue = 0;
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  lcd.print("Detect volatile");
  lcd.setCursor(0, 1);
  lcd.print("in air with TGS2602");
  delay(1000);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

// the loop routine runs over and over again forever:
void loop() {
  leds[0] = CRGB::Black;
  FastLED.show();
  // read the input on analog pin 0:
  sensorValue = analogRead(tgs_pin);
  // print out the value you read:
  Serial.println(sensorValue);

  lcd.setCursor(0, 0);
  lcd.print("H2S Val:");
  lcd.print(sensorValue);
  delay(1000);        // delay in between reads for stability
  func_tgs();

}
void func_tgs() {
  if (sensorValue > 0 && sensorValue < 200)
  {

    //delay(000); // Beeps if its Salt water.
    lcd.setCursor(0, 1);
    lcd.print("No Smell");
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(1000);
  }
  else if (sensorValue >= 201 && sensorValue <= 600) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Medium smell");
    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(1000);
  }
  else if(sensorValue >= 601 && sensorValue <= 1023) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Foul smell");
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(1000);
  }
}

