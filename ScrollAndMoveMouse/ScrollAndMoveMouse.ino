/**
 * This example turns the ESP32 into a Bluetooth LE mouse that continuously moves the mouse.
 */

#include <ezButton.h>
#include <BleCombo.h>
#include <IRremote.h>

const int RECV_PIN = 33;

IRrecv irrecv(RECV_PIN);
decode_results results;

#define DEBUG 0
#define VRX_PIN 34  // ESP32 pin GIOP36 (ADC0) connected to VRX pin
#define VRY_PIN 35  // ESP32 pin GIOP39 (ADC0) connected to VRY pin
#define SW_PIN 14   // ESP32 pin GIOP17 connected to SW  pin
#define BUTTON_ONE_PIN 26
#define BUTTON_TWO_PIN 25

#define LEFT_THRESHOLD 500
#define RIGHT_THRESHOLD 4000
#define UP_THRESHOLD 500
#define DOWN_THRESHOLD 4000

#define COMMAND_NO 0x00
#define COMMAND_LEFT 0x01
#define COMMAND_RIGHT 0x02
#define COMMAND_UP 0x04
#define COMMAND_DOWN 0x08

#define REMOTE_KEYBORT_MOUSE_DELAY_TIME 100

int command = COMMAND_NO;

ezButton button(SW_PIN);

int valueX = 0;  // to store the X-axis value
int valueY = 0;  // to store the Y-axis value
int bValue = 0;  // To store value of the button

int leftButtonState = HIGH;
int leftButtonCurrentState;
int rightButtonState = HIGH;
int rightButtonCurrentState;

void debugger(String message){
  if(DEBUG == 1){
    Serial.println(message);
  }
}

void debugger(int message){
  if(DEBUG == 1){
    Serial.println(message);
  }
}

void debugger(int message, bool hex){
  if(DEBUG == 1 && hex){
    Serial.println(message, HEX);
  }
}

void setup() {
  Serial.begin(115200);
  debugger("Starting BLE work!");
  Keyboard.begin();
  Mouse.begin();
  irrecv.enableIRIn();
  pinMode(BUTTON_ONE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_TWO_PIN, INPUT_PULLUP);
}

void moveMouse(int time, int l, int r) {
  for(int i = 0; i < time;i++){
    Mouse.move(l, r);
  }
  
}


void loop() {
  button.loop();

  if (Keyboard.isConnected()) {

    // read X and Y analog values
    valueX = analogRead(VRX_PIN);
    valueY = analogRead(VRY_PIN);

    leftButtonCurrentState = digitalRead(BUTTON_ONE_PIN);
    rightButtonCurrentState = digitalRead(BUTTON_TWO_PIN);

    if (leftButtonState == LOW && leftButtonCurrentState == HIGH) {
      Keyboard.println("f");
      delay(1000);
      Keyboard.releaseAll();
      debugger("Left Button");
    }

    // save the last state
    leftButtonState = leftButtonCurrentState;

    if (rightButtonState == LOW && rightButtonCurrentState == HIGH) {
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_TAB);
      delay(100);
      Keyboard.releaseAll();
      debugger("Right Button");
    }

    // save the last state
    rightButtonState = rightButtonCurrentState;
    // converts the analog value to commands

    // reset commands
    command = COMMAND_NO;

    // check left/right commands
    if (valueX < LEFT_THRESHOLD)
      command = command | COMMAND_LEFT;
    else if (valueX > RIGHT_THRESHOLD)
      command = command | COMMAND_RIGHT;

    // check up/down commands
    if (valueY < UP_THRESHOLD)
      command = command | COMMAND_UP;
    else if (valueY > DOWN_THRESHOLD)
      command = command | COMMAND_DOWN;

    // Serial.print(valueX);
    // Serial.print(" X : ");
    // Serial.print(valueY);
    // debugger(" Y");

    // NOTE: AT A TIME, THERE MAY BE NO COMMAND, ONE COMMAND OR TWO COMMANDS

    // print command to serial and process command
    if (command & COMMAND_LEFT) {
      //debugger("COMMAND LEFT");
      Mouse.move(-1, 0);
    }

    if (command & COMMAND_RIGHT) {
      //debugger("COMMAND RIGHT");
      Mouse.move(1, 0);
    }

    if (command & COMMAND_UP) {
      //debugger("COMMAND UP");
      Mouse.move(0, -1);
    }

    if (command & COMMAND_DOWN) {
      //debugger("COMMAND DOWN");
      Mouse.move(0, 1);
    }

    // Read the button value
    bValue = button.getState();

    if (button.isPressed()) {
      //debugger("The button is pressed");
      Mouse.click(MOUSE_LEFT);
    }

    if (button.isReleased()) {
      //debugger("The button is released");
      // TODO do something here
    }

    //IR
    debugger(IrReceiver.decode());
    if (IrReceiver.decode()) {

      switch (IrReceiver.decodedIRData.command) {
        case 69:
          debugger(" CH-            ");
           Keyboard.press(KEY_ESC);

          Keyboard.releaseAll();
          break;
        case 70:
          debugger(" CH             ");
          Keyboard.press(KEY_LEFT_GUI);
          Keyboard.println("l");

          Keyboard.releaseAll();
          break;
        case 71:
          debugger(" CH+            ");
         
          break;
        case 68:
          debugger("|<<          ");
            Keyboard.write(KEY_LEFT_ARROW );
          delay(100);
          Keyboard.releaseAll();
          break;
        case 64:
          debugger(">>|        ");
            Keyboard.write(KEY_RIGHT_ARROW );
          delay(100);
          Keyboard.releaseAll();

          break;
        case 67:
          debugger(" PLAY/PAUSE     ");
           Keyboard.write(KEY_LEFT_GUI);
          delay(100);
          Keyboard.releaseAll();
          break;
        case 7:
          debugger(" VOL-           ");
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_F3);

          Keyboard.releaseAll();
          delay(200);

          Keyboard.write(KEY_DOWN_ARROW);
          Keyboard.write(KEY_ESC);
          break;
        case 21:
          debugger(" VOL+           ");

          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_F3);

          Keyboard.releaseAll();
          delay(200);

          Keyboard.write(KEY_UP_ARROW);
          Keyboard.write(KEY_ESC);
          break;
        case 9:
          debugger(" EQ             ");
          Keyboard.write(KEY_LEFT_GUI);
          delay(100);
          Keyboard.releaseAll();
          break;
        case 22:
          debugger(" 0              ");
           Mouse.click(MOUSE_RIGHT);
          break;
        case 25:
          debugger(" 100+           ");
           Keyboard.write(KEY_PAGE_UP );
          delay(100);
          Keyboard.releaseAll();
          break;
        case 13:
          debugger(" 200+           ");
           Keyboard.write(KEY_PAGE_DOWN);
          delay(100);
          Keyboard.releaseAll();
          break;
        case 12:
          debugger(" 1              ");
          moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,-1,0);
           moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,0,-1);
          break;
        case 24:
          debugger(" 2              ");
           moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,0,-1);
          break;
        case 94:
          debugger(" 3              ");
           moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,1,0);
            moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,0,-1);
          break;
        case 8:
          debugger(" 4              ");
           moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,-1,0);
          break;
         
        case 28:
          debugger(" 5              ");
          Mouse.click(MOUSE_LEFT);
          break;
        case 90:
          debugger(" 6              ");
           moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,1,0);
          break;
        case 66:
          debugger(" 7              ");
           moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,0,1);
            moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,-1,0);
          break;
        case 82:
          debugger(" 8              ");
           moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,0,1);
          break;
        case 74:
          debugger(" 9              ");
           moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,0,1);
            moveMouse(REMOTE_KEYBORT_MOUSE_DELAY_TIME,1,0);
          break;
        default:
          Serial.print(" unknown button   ");
          debugger(results.value, true);
      }

      irrecv.resume();
      delay(1000);
    }
  }
}
/*
USB CODE	Keyboard()	Key Location on US-en Keyboard
========	==========	==============================
0	0x00				Reserved (no event indicated) *9
1	0x01				Keyboard ErrorRollOver *9 
2	0x02				Keyboard POSTFail *9 
3	0x03				Keyboard ErrorUndefined *9 
4	0x04	140	0x8C	Keyboard a and A *4 
5	0x05	141	0x8D	Keyboard b and B 
6	0x06	142	0x8E	Keyboard c and C *4 
7	0x07	143	0x8F	Keyboard d and D 
8	0x08	144	0x90	Keyboard e and E 
9	0x09	145	0x91	Keyboard f and F 
10	0x0A	146	0x92	Keyboard g and G 
11	0x0B	147	0x93	Keyboard h and H 
12	0x0C	148	0x94	Keyboard i and I 
13	0x0D	149	0x95	Keyboard j and J 
14	0x0E	150	0x96	Keyboard k and K 
15	0x0F	151	0x97	Keyboard l and L 
16	0x10	152	0x98	Keyboard m and M *4 
17	0x11	153	0x99	Keyboard n and N 
18	0x12	154	0x9A	Keyboard o and O *4 
19	0x13	155	0x9B	Keyboard p and P *4 
20	0x14	156	0x9C	Keyboard q and Q *4
21	0x15	157	0x9D	Keyboard r and R
22	0x16	158	0x9E	Keyboard s and S *4
23	0x17	159	0x9F	Keyboard t and T
24	0x18	160	0xA0	Keyboard u and U
25	0x19	161	0xA1	Keyboard v and V
26	0x1A	162	0xA2	Keyboard w and W *4
27	0x1B	163	0xA3	Keyboard x and X *4
28	0x1C	164	0xA4	Keyboard y and Y *4
29	0x1D	165	0xA5	Keyboard z and Z *4
30	0x1E	166	0xA6	Keyboard 1 and ! *4
31	0x1F	167	0xA7	Keyboard 2 and @ *4
32	0x20	168	0xA8	Keyboard 3 and # *4
33	0x21	169	0xA9	Keyboard 4 and $ *4
34	0x22	170	0xAA	Keyboard 5 and % *4
35	0x23	171	0xAB	Keyboard 6 and ^ *4
36	0x24	172	0xAC	Keyboard 7 and & *4
37	0x25	173	0xAD	Keyboard 8 and * *4
38	0x26	174	0xAE	Keyboard 9 and ( *4
39	0x27	175	0xAF	Keyboard 0 and ) *4
40	0x28	176	0xB0	Keyboard Return (ENTER) *5 
41	0x29	177	0xB1	Keyboard ESCAPE
42	0x2A	178	0xB2	Keyboard DELETE (Backspace) *13 
43	0x2B	179	0xB3	Keyboard Tab
44	0x2C	180	0xB4	Keyboard Spacebar
45	0x2D	181	0xB5	Keyboard - and (underscore) *4 
46	0x2E	182	0xB6	Keyboard = and + *4
47	0x2F	183	0xB7	Keyboard [ and { *4
48	0x30	184	0xB8	Keyboard ] and } *4
49	0x31	185	0xB9	Keyboard \ and |
50	0x32	186	0xBA	Keyboard Non-US # and ~ *2 
51	0x33	187	0xBB	Keyboard ; and : *4
52	0x34	188	0xBC	Keyboard ‘ and “ *4
53	0x35	189	0xBD	Keyboard Grave Accent and Tilde *4 
54	0x36	190	0xBE	Keyboard , and < *4
55	0x37	191	0xBF	Keyboard . and > *4
56	0x38	192	0xC0	Keyboard / and ? *4
57	0x39	193	0xC1	Keyboard Caps Lock *11
58	0x3A	194	0xC2	Keyboard F1
59	0x3B	195	0xC3	Keyboard F2
60	0x3C	196	0xC4	Keyboard F3
61	0x3D	197	0xC5	Keyboard F4
62	0x3E	198	0xC6	Keyboard F5
63	0x3F	199	0xC7	Keyboard F6
64	0x40	200	0xC8	Keyboard F7
65	0x41	201	0xC9	Keyboard F8
66	0x42	202	0xCA	Keyboard F9
67	0x43	203	0xCB	Keyboard F10
68	0x44	204	0xCC	Keyboard F11
69	0x45	205	0xCD	Keyboard F12
70	0x46	206	0xCE	Keyboard PrintScreen *1 
71	0x47	207	0xCF	Keyboard Scroll Lock *11 
72	0x48	208	0xD0	Keyboard Pause *1
73	0x49	209	0xD1	Keyboard Insert *1
74	0x4A	210	0xD2	Keyboard Home *1
75	0x4B	211	0xD3	Keyboard PageUp *1 
76	0x4C	212	0xD4	Keyboard Delete Forward *1;*14 
77	0x4D	213	0xD5	Keyboard End *1
78	0x4E	214	0xD6	Keyboard PageDown *1 
79	0x4F	215	0xD7	Keyboard RightArrow *1 
80	0x50	216	0xD8	Keyboard LeftArrow *1 
81	0x51	217	0xD9	Keyboard DownArrow *1 
82	0x52	218	0xDA	Keyboard UpArrow *1
83	0x53	219	0xDB	Keypad Num Lock and Clear *11 
84	0x54	220	0xDC	Keypad / *1
85	0x55	221	0xDD	Keypad * 
86	0x56	222	0xDE	Keypad - 
87	0x57	223	0xDF	Keypad + 
88	0x58	224	0xE0	Keypad ENTER *5
89	0x59	225	0xE1	Keypad 1 and End
90	0x5A	226	0xE2	Keypad 2 and Down Arrow
91	0x5B	227	0xE3	Keypad 3 and PageDn
92	0x5C	228	0xE4	Keypad 4 and Left Arrow
93	0x5D	229	0xE5	Keypad 5 
94	0x5E	230	0xE6	Keypad 6 and Right Arrow
95	0x5F	231	0xE7	Keypad 7 and Home
96	0x60	232	0xE8	Keypad 8 and Up Arrow
97	0x61	233	0xE9	Keypad 9 and PageUp
98	0x62	234	0xEA	Keypad 0 and Insert 
99	0x63	235	0xEB	Keypad . and Delete
100	0x64	236	0xEC	Keyboard Non-US \ and | *3;*6
101	0x65	237	0xED	Application *10
102	0x66	238	0xEE	Keyboard Power *9
103	0x67	239	0xEF	Keypad =
104	0x68	240	0xF0	Keyboard F13
105	0x69	241	0xF1	Keyboard F14
106	0x6A	242	0xF2	Keyboard F15
107	0x6B	243	0xF3	Keyboard F16
108	0x6C	244	0xF4	Keyboard F17
109	0x6D	245	0xF5	Keyboard F18
110	0x6E	246	0xF6	Keyboard F19
111	0x6F	247	0xF7	Keyboard F20
112	0x70	248	0xF8	Keyboard F21
113	0x71	249	0xF9	Keyboard F22
114	0x72	250	0xFA	Keyboard F23
115	0x73	251	0xFB	Keyboard F24
116	0x74	252	0xFC	Keyboard Execute
117	0x75	253	0xFD	Keyboard Help
118	0x76	254	0xFE	Keyboard Menu
119	0x77	255	0xFF	Keyboard Select  

*/
