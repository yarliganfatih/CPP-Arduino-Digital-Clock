
//########################################################################################
//##################################    Digital Alarm Clock     ##########################
//##################################    CMP3006 TERM PROJECT    ##########################
//##################################     Onur Guzel 1400369     ##########################                                                 
//########################################################################################
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

const int Buzzer = 10;     
const int led_pin = PB5;
const int Interrupt_pin1 = 2;
const int ButtonPins[] = {16,17,18,19};
const int LongPressTime = 3000; 


volatile int flag_reset=0;

int temp;
int tempF;
int sensed_temp;
int old_temp = 126;
int flag_temp = 0;
int CF_flag = 0;


int cur_day = 0;
int flag_day = 0;
boolean flag_time = true;
volatile int h_PM;
volatile int h = 01;
volatile int m = 30;
volatile int s = 00;
volatile int old_s = 0;


volatile int buzz = 0;
volatile int alarm_status = 0;
volatile int flag_alarm = 0;
int alarm_h = 01;
int alarm_m = 35;
int alarm_ring = 0;

int lastState = LOW;  
int currentState;     
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

boolean adjust_time = 0;
boolean adjust_case = 1;
boolean adjust_min = 0;
boolean adjust_h = 0;


long debouncing_time = 20; 
volatile unsigned long last_micros;

//########################################################################################
//##################################    SETUP      #######################################
//########################################################################################

void setup() {
  cli();
  
  Serial.begin(9600);
  while (!Serial){};
  
 lcd.begin(16, 2);
 lcd.setCursor(0,0);
 lcd.print("Starting System");
 delay(2000);
 lcd.setCursor(0,0);
 lcd.print("CMP3006  Project");
 lcd.setCursor(0,1);
 lcd.print("Digital   Clock");
 delay(5000);
 lcd.clear();
 Disp(h,m,s);
 delay(100);
 Day();
 delay(100);
 TempMeasure();
 TempPrint();
 AlarmDisp();
 
DDRB |= (1 << led_pin);

PinConfig_Common();  
pinMode(Buzzer, OUTPUT); 
  
attachInterrupt(digitalPinToInterrupt(2), debounceButton, FALLING);

TCCR1A = 0; 
TCNT1  = 0; 
OCR1A = 31250; 
TCCR1B &= ~(1 << WGM13);
TCCR1B |= (1 << WGM12);
TCCR1B = 0;
TCCR1B |= (1 << CS10); ///
TIMSK1 |= (1 << OCIE1A);
 sei();
}


//########################################################################################
//##################################   DISPLAY FUCNTION   ################################
//########################################################################################



int Disp(int h,int m,int s)
{
 lcd.setCursor(0,0);
  if(flag_time==true)
 {   
   if(h<12)
  {
   lcd.setCursor(8,0);
  lcd.print("AM");
  }
  else
  {
  lcd.setCursor(8,0);
  lcd.print("PM");
  }
      if(h<10)
  {  
  lcd.setCursor(0,0);
  lcd.print("0");
    lcd.print(h);
  }
    else if(h>9 && h<13)
    {
    lcd.setCursor(0,0);
    lcd.print(h);
    }
    else
    {
    h_PM = map(h, 13, 23, 1, 11);
   lcd.setCursor(0,0);  
   if(h_PM <10)
   {
   lcd.setCursor(0,0);
   lcd.print("0");
   lcd.print(h_PM);
   }
       else
      {
      lcd.setCursor(0,0);
     lcd.print(h);
      }
    }
  }

 if(flag_time==false)
 {
 lcd.setCursor(8,0);  
 lcd.print("    ");  
 lcd.setCursor(0,0);
 if(h<10)
 {  
 lcd.setCursor(0,0);
 lcd.print("0");
 lcd.print(h);
 }
 else
 {
 lcd.print(h);
 }
 }
 lcd.print(":");
  if(m<10)
 {
  lcd.setCursor(3,0);
  lcd.print("0");
 lcd.print(m);
 }
 else
 {
 lcd.print(m);
 }
 lcd.print(":");
 if(s<10)
 {
  lcd.setCursor(6,0);
  lcd.print("0");
 lcd.print(s);
 }
 else
 {
 lcd.print(s);
 }
  lcd.setCursor(10,0);
  lcd.print("   ");
    lcd.setCursor(10,1);
  lcd.print("  ");
 lcd.noCursor();
}


//########################################################################################
//##################################   TEMPRATURE ########################################
//########################################################################################

void TempMeasure()
{
sensed_temp = analogRead(A0);
temp = map(sensed_temp, 20, 358, -40, 125);
if(old_temp!=temp)
{
  flag_temp = 1;
}
}


void TempPrint()
{
  lcd.setCursor(12,0);
  lcd.print("    ");
    switch(CF_flag)
{
 case 0:
        if(temp < -10 || temp > 99)
         {
           lcd.setCursor(12,0); 
         }
       else 
         {
       lcd.setCursor(13,0);
         }
  lcd.print(temp);
  lcd.print("C");
  old_temp = temp;
  break;
case 1:
  tempF = temp*1.8+32;
      if(tempF < -10 || tempF > 99)
         {
           lcd.setCursor(12,0); 
         }
       else 
         {
       lcd.setCursor(13,0);
         }
  lcd.print(tempF);
  lcd.print("F");
  old_temp = temp;
  break;
}
flag_temp = 0;
}
//########################################################################################
//###############################   DISPLAY FUNCTION   ###################################
//########################################################################################

void AlarmDisp()
{
  if(alarm_status==1)
  {
   lcd.setCursor(0,1);
   lcd.print("          ");
    if(flag_time==1)
    {  
         if(alarm_h<12)
       {
        lcd.setCursor(5,1);
        lcd.print("AM");
       }
   else
   {
   lcd.setCursor(5,1);
   lcd.print("PM");
    }
   lcd.setCursor(0,0);
   if(alarm_h<10)
   {  
     lcd.setCursor(0,1);
     lcd.print("0");
       lcd.print(alarm_h);
   }
    else if(alarm_h>9 && alarm_h<13)
    {
       lcd.setCursor(0,0);
       lcd.print(alarm_h);
    }
   else
   {
      lcd.print(alarm_h);
   }
  }
  if(flag_time==0)
  {
   lcd.setCursor(0,1);
   if(alarm_h<10)
   {  
  lcd.setCursor(0,1);
   lcd.print("0");
   lcd.print(alarm_h);
   }
   else
   {
   lcd.print(alarm_h);
   }
   }
   lcd.print(":");
  if(alarm_m<10)
 {
  lcd.setCursor(3,1);
  lcd.print("0");
 lcd.print(alarm_m);
 }
 else
 {
 lcd.print(alarm_m);
 }
  lcd.setCursor(8,1);
  lcd.print("ON");
  flag_alarm = 0;
 }
 if(alarm_status==0)
 {
  lcd.setCursor(0,1);
  lcd.print("          ");
  lcd.setCursor(7,1);
  lcd.print("OFF");
  flag_alarm = 0;
 }
}


//########################################################################################
//##################################   DAYS FUCNTION   ###################################
//########################################################################################


void Day()
{
  lcd.setCursor(13,1);
  switch(cur_day)
  {
    case 0: 
    lcd.print("Mon");
    break;
    case 1: 
    lcd.print("Tue");
    break;
    case 2: 
    lcd.print("Wed");
    break;
    case 3: 
    lcd.print("Thu");
    break;
    case 4: 
    lcd.print("Fri");
    break;
    case 5: 
    lcd.print("Sat");
    break;
    case 6: 
    lcd.print("Sun");
    break;
  }

  flag_day=0;
}


//########################################################################################
//##################################   BUTTONS   #########################################
//########################################################################################


void debounceButton()
{
   if((long)(micros() - last_micros) >= debouncing_time * 1000) 
   {
   WhichButton();
   last_micros = micros();
   }
}




void WhichButton()
{
  PinConfig_Press(); 

  for (int i = 0; i < sizeof(ButtonPins) / sizeof(int); i++)
  { 
    if (!digitalRead(ButtonPins[i])) 
      {
        shortpress(i);
      }
  }


  PinConfig_Common();
}





void shortpress(int button)
{
 switch (button)
 {
  case 0:
  Button1();
  break;
    case 1:
  Button2();
  break;
    case 2:
  Button3();
  break;
    case 3:
  Button4();
  break;
 }
}


//########################################################################################

void Button1()
{
flag_time = !flag_time;
 lcd.setCursor(0,0);
 lcd.print("          ");
 flag_reset = 1; 
}

//########################################################################################

void Button2()
{
  flag_alarm = 1;
  if(alarm_status == 1)
  { alarm_status = 0;}
  else 
  {
    alarm_status =1;
  }
  Serial.println(alarm_status);
}

//########################################################################################

void Button3()
{
  if(adjust_time ==1)
  {
    switch (adjust_case)
    {
      case 0:
      if(adjust_min == 1)
      {
        m=m+1;
      }
      if(adjust_h == 1)
      {
        h=h+1;
      }
      else
      {
        break;
      }
            case 1:
      if(adjust_min == 1)
      {
        m=m+1;
      }
      if(adjust_h == 1)
      {
        h=h+1;
      }
      else
      {
        break;
      }
    }
  }
  else
  {
      flag_temp = 1;
  if(CF_flag == 1)
  { CF_flag = 0;}
  else 
  {
    CF_flag =1;
  }
  }
}

//########################################################################################
void Button4()
{
  if(alarm_status == 1)
  {
  alarm_m = alarm_m + 5;
    if(alarm_m >59)
    {
      alarm_m = 0;
      alarm_h = alarm_h + 1;
    }
  }
  noTone(Buzzer);
  flag_reset=1;
  
}

//########################################################################################

void LButton1()
{
Serial.print("UP");
}

//########################################################################################

void LButton2()
{
Serial.print("UP");
  
}

//########################################################################################

void LButton3()
{
Serial.print("UP");
  
}

//########################################################################################

void LButton4()
{
Serial.print("UP");
  
  
}




//########################################################################################
//##################################  PIN CONFIG   #######################################
//########################################################################################


void PinConfig_Common() {
  pinMode(Interrupt_pin1, INPUT_PULLUP);

  for (int i = 0; i < sizeof(ButtonPins) / sizeof(int); i++) {
    pinMode(ButtonPins[i], OUTPUT);
    digitalWrite(ButtonPins[i], LOW);
  }
}




void PinConfig_Press() {
  pinMode(Interrupt_pin1, OUTPUT);
  digitalWrite(Interrupt_pin1, LOW);

  for (int i = 0; i < sizeof(ButtonPins) / sizeof(int); i++) {
    pinMode(ButtonPins[i], INPUT_PULLUP);
  }
}

void AlarmSound()
{
  Disp(h,m,s);
    if(buzz==1)
    {
      tone(Buzzer, 16000);    
  }
  if(s>=60)
  {
    s=0;
    m=m+1;
  }
}



//########################################################################################
//##################################    LOOP  ############################################
//########################################################################################

void loop() {
if(flag_reset == 1)
{
  lcd.clear();
  Disp(h,m,s);
  AlarmDisp();
  Day();
  TempPrint();
  flag_reset = 0;
}
  
if(old_s!=s)
{
Disp(h,m,s);
}
if(flag_alarm==true)
{
  AlarmDisp();
}
if(flag_day==1)
{
Day();
}
TempMeasure();
if(flag_temp == 1)
{
  TempPrint();
}

if(h == alarm_h && m == alarm_m && alarm_status == 1)
{
  alarm_ring = 1;
  if(alarm_ring == 1)
  {
  AlarmSound();
  }
  else
  {
   noTone(Buzzer );
  
  }
}
  
if(s>=60)
{
  s=0;
  m=m+1;
}
  
if(m==60)
{
  m=0;
  h=h+1;
}
  
if(h == 24)
{
  h=0;
  flag_day=1;
    cur_day++;
  if (cur_day>6)
      {
        cur_day=0;
      }
}
}



ISR(TIMER1_COMPA_vect)
{
  old_s=s;
   s++;
 PORTB ^= (1 << led_pin);
   if(buzz == 1)
  { buzz = 0;}
  else 
  {
    buzz = 1;
  }
   
}
