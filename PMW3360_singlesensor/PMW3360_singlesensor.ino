// Uncomment this line to activate an advanced mouse mode
// Need to install AdvMouse library (copy /library/AdvMouse to the Arduino libraries)
// AdvMouse module: https://github.com/SunjunKim/PMW3360_Arduino/tree/master/library/AdvMouse
#define ADVANCE_MODE

#include <SPI.h>
#include <avr/pgmspace.h>
#include <PMW3360.h>

#ifdef ADVANCE_MODE
#include <AdvMouse.h>
#define MOUSE_BEGIN       AdvMouse.begin()
#define MOUSE_PRESS(x)    AdvMouse.press_(x)
#define MOUSE_RELEASE(x)  AdvMouse.release_(x)
#else
#include <Mouse.h>
#define MOUSE_BEGIN       Mouse.begin()
#define MOUSE_PRESS(x)    Mouse.press(x)
#define MOUSE_RELEASE(x)  Mouse.release(x)
#endif

// User define values
#define DEFAULT_CPI  1200

#define SS  10          // Slave Select pin. Connect this to SS on the module. (Front sensor)
#define NUMBTN 2        // number of buttons attached
#define BTN1 2          // left button pin
#define BTN2 3          // right button pin
#define DEBOUNCE  15    // debounce itme in ms. Minimun time required for a button to be stabilized.

int btn_pins[NUMBTN] = { BTN1, BTN2 };
char btn_keys[NUMBTN] = { MOUSE_LEFT, MOUSE_RIGHT };

// Don't need touch below.
PMW3360 sensor;

// button pins & debounce buffers
bool btn_state[NUMBTN] = { false, false };
uint8_t btn_buffers[NUMBTN] = {0xFF, 0xFF};

// internal variables
unsigned long lastTS;
unsigned long lastButtonCheck = 0;

int remain_dx, remain_dy;

bool reportSQ = false;  // report surface quality

int current_cpi = DEFAULT_CPI;

void setup() {
  Serial.begin(9600);
  //while(!Serial);
  //sensor.begin(10, 1600); // to set CPI (Count per Inch), pass it as the second parameter

  if (sensor.begin(SS)) // 10 is the pin connected to SS of the module.
    Serial.println("Sensor1 initialization successed");
  else
    Serial.println("Sensor1 initialization failed");

  sensor.setCPI(DEFAULT_CPI);    // or, you can set CPI later by calling setCPI();

  remain_dx = remain_dy = 0;

  MOUSE_BEGIN;
  buttons_init();
}


bool sent = false;
void loop() {
  unsigned long elapsed = micros() - lastTS;

  check_buttons_state();

  if (elapsed > 900)
  {
    lastTS = micros();

    PMW3360_DATA data = sensor.readBurst();
    
    int dx = data.dx;
    int dy = data.dy;

    bool moved = (dx != 0) || (dy != 0);

    /*
    if(data.isOnSurface && moved)
    {
      Serial.print(micros());
      Serial.print('\t');
      Serial.print(data.dx);
      Serial.print('\t');
      Serial.println(data.dy);
    }
    */

#ifdef ADVANCE_MODE
    if (AdvMouse.needSendReport() || (data.isOnSurface && moved))
    {
      //if (AdvMouse.needSendReport() && !data.isMotion)
      //  Serial.println("Btn report");
      AdvMouse.move(data.dx, data.dy, 0);
    }
#else
    if (data.isOnSurface && moved)
    {
      signed char mdx = constrain(data.dx, -127, 127);
      signed char mdy = constrain(data.dy, -127, 127);

      Mouse.move(mdx, mdy, 0);
    }
#endif
  
    if (reportSQ && data.isOnSurface) // print surface quality
    {
      Serial.println(data.SQUAL);
    }
  }

  // command process routine
  while (Serial.available() > 0)
  {
    char c = Serial.read();

    if (c == 'Q') // Toggle reporting surface quality
    {
      reportSQ = !reportSQ;
    }
    else if (c == 'c') // report current CPI
    {
      Serial.print("Current CPI: ");
      Serial.println(sensor.getCPI());
    }
    else if (c == 'C')   // set CPI
    {
      int newCPI = readNumber();
      sensor.setCPI(newCPI);
      
      current_cpi = sensor.getCPI();
      
      Serial.println(sensor.getCPI());
    }
  }
}

void buttons_init()
{
  for (int i = 0; i < NUMBTN; i++)
  {
    pinMode(btn_pins[i], INPUT_PULLUP);
  }
}

// Button state checkup routine, fast debounce is implemented.
void check_buttons_state()
{
  unsigned long elapsed = micros() - lastButtonCheck;

  // Update at a period of 1/8 of the DEBOUNCE time
  if (elapsed < (DEBOUNCE * 1000UL / 8))
    return;

  lastButtonCheck = micros();

  // Fast Debounce (works with mimimal latency most of the time)
  for (int i = 0; i < NUMBTN ; i++)
  {
    int state = digitalRead(btn_pins[i]);
    btn_buffers[i] = btn_buffers[i] << 1 | state;

    // btn_buffer detects 0 when the switch shorts, 1 when opens.
    if (btn_state[i] == false && 
        (btn_buffers[i] == 0xFE || btn_buffers[i] == 0x00) )
        // 0xFE = 0b1111:1110 button pressed for the first time (for fast press detection w. minimum debouncing time)
        // 0x00 = 0b0000:0000 force press when consequent on state (for the DEBOUNCE time) is detected
    {
      MOUSE_PRESS(btn_keys[i]);
      btn_state[i] = true;
    }
    else if ( btn_state[i] == true && 
              (btn_buffers[i] == 0x07 || btn_buffers[i] == 0xFF) )
              // 0x07 = 0b0000:0111 button released consequently 3 times after stabilized press (not as fast as press to prevent accidental releasing during drag)
              // 0xFF = 0b1111:1111 force release when consequent off state (for the DEBOUNCE time) is detected
    {
      MOUSE_RELEASE(btn_keys[i]);
      btn_state[i] = false;
    }
  }
}


unsigned long readNumber()
{
  String inString = "";
  for (int i = 0; i < 10; i++)
  {
    while (Serial.available() == 0);
    int inChar = Serial.read();
    if (isDigit(inChar))
    {
      inString += (char)inChar;
    }

    if (inChar == '\n')
    {
      int val = inString.toInt();
      return (unsigned long)val;
    }
  }

  // flush remain strings in serial buffer
  while (Serial.available() > 0)
  {
    Serial.read();
  }
  return 0UL;
}
