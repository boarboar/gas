
#define NCHRS 14
#define CHR_DOT 10
#define CHR_H 11
#define CHR_C 12
#define CHR_E 13
const int chrs[NCHRS] = { 
   //gfedcba.
    B01111110, //0
    B00001100, //1
    B10110110, //2
    B10011110, //3
    B11001100, //4
    B11011010, //5
    B11111010, //6
    B00001110, //7
    B11111110, //8
    B11011110, //9
    B00000001, //DOT
    B11101100, //H
    B01110010, //C
    B11110010, //E    
};

#define dataPin P2_0
#define clockPin P2_1
#define latchPin P2_2
#define soundPin P2_5
#define sensorPin  P1_4    // select the input pin for the potentiometer
#define ledPin RED_LED      // select the pin for the LED

#define WUP10  4000
#define UP_THR 4 
#define DN_THR 2
#define SMUL  4
#define NODATA  20

uint16_t sensorValue = 0;  // variable to store the value coming from the sensor
uint16_t base = 0;
uint16_t bl=0;
uint16_t alarm=0;
uint16_t hist[3];

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  Serial.begin(9600);

  digitalWrite(ledPin, HIGH);  

  pinMode(soundPin, OUTPUT);  
  tone(soundPin, 262, 1000);
  delay(1200);

  Serial.println("Heating...");

  display(CHR_H);  
  delay(WUP10); 
  for(uint16_t i=0; i<10; i++) {
    delay(WUP10);
    display(9-i);
  }
  delay(WUP10);
  
  Serial.println("Calibrating...");
  display(CHR_C);
  delay(1000);
  uint32_t acc=0;
  for(uint16_t i=0; i<100; i++) {
    acc+=analogRead(sensorPin);
    delay(100);
    if(!(i%10))
      display(9-i/10);
  }

  base=acc/100;
    
  Serial.print("Base=");
  Serial.println(base);

  if(base<NODATA) {
    // no data
    while(1) {
      display(CHR_E);
      tone(soundPin, 196, 500);
      delay(500);
      display(CHR_DOT);
      noTone(soundPin);
      delay(500);
    }
  }
  
  tone(soundPin, 196, 1000);
  delay(1200); 
  noTone(soundPin);

//hist[0]=hist[1]=hist[2]=base;
  hist[0]=hist[1]=hist[2]=0;
  
  digitalWrite(ledPin, LOW);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin); 
  Serial.print(sensorValue);
  Serial.print("  ");
  sensorValue=sensorValue>base ? (sensorValue-base)*SMUL/base : 0;
    
  //sensorValue=sensorValue*4/base;
  Serial.println(sensorValue);
  
  hist[0]=hist[1]; hist[1]=hist[2]; hist[2]=sensorValue;
  
  sensorValue=(hist[0]+hist[1]+hist[2])/3;
  
  if(sensorValue>9) sensorValue=9; 
  display(sensorValue);
  
  if(sensorValue>=UP_THR) alarm=1;
  
  if(alarm) {
    tone(soundPin, bl ? 196 : 262, 500);
    if(alarm && sensorValue<=DN_THR) alarm=0; 
  }
  bl=!bl;
  digitalWrite(ledPin, bl ? HIGH : LOW); 
  delay(500);
}

void display(uint16_t c) { 
    if(c>=NCHRS) return;
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, chrs[c]); 
    digitalWrite(latchPin, HIGH);
}
