#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

char inputCode[16];

bool activated = false;

int counter = 0;
int timerTicks = 0;

void setup()
{
  lcd.begin(16,2);
  lcd.clear();
  
  WriteString("");
  Serial.begin(9600);
  
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  StartTimer();

}

void loop()
{
  while (!Serial.available()) {}
  
  char value = Serial.read();
  

  AddToInputCode(value);
  
  WriteString(inputCode);
  
  if(strcmp(inputCode, " 7 3 5 5 6 0 8") == 0){
    delay(100);
    WriteString("");
    inputCode[0] = '\0';
    
    if(activated) {
      activated = false;
      counter = 0;
    }else {
      activated = true;
      
      StartTimer();
    }
  }

  
}

void WriteString(char* string) {
  lcd.clear();
  for(int i = 2; i < 16 - strlen(string); i+=2) {
  	lcd.setCursor(i, 0);
    lcd.print(" *"); 
  }
  
  
  lcd.setCursor(16 - strlen(string), 0);
  lcd.print(string); 


}

void AddToInputCode(char inputChar) {
  char temp[] = " ";
  char charToString[2] = {inputChar};
  
  strcat(temp, charToString);
  
  strcat(inputCode, temp);

}


ISR(TIMER1_COMPA_vect){
  timerTicks++;
  
  if(activated) {
    counter = timerTicks / 20;

    int beepThreshold = 20 * max(0.1 + 0.9 * ((40-counter)/40.0), 0.15);
    
    if(timerTicks % beepThreshold == 0) {
      sei();
      digitalWrite(6, LOW);

      tone(10, 2500);

      digitalWrite(7, HIGH);
      delay(125);
      noTone(10);
      digitalWrite(7, LOW);
      cli();
      
      if(counter == 40) {
        exit(0);
      }
    }


  }else {
    digitalWrite(7, LOW);
    digitalWrite(6, HIGH);
    delay(300);
    digitalWrite(6, LOW);
  }
  
  
}

void StartTimer() {
  //stop interrupts
  cli();
  // set entire TCCR1A register to 0
  TCCR1A = 0;
  // same for TCCR1B
  TCCR1B = 0;
  //initialize counter value to 0
  TCNT1 = 0;
  // set compare match register
  OCR1A = 15624 / 20;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12)| (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  //allow interrupts 
  sei();
}
