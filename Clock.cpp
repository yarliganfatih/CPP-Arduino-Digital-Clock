// Digital Alarm Clock | CMP3010 LAB PROJECT | Fatih Yarlıgan 1805114

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);


int update_reset = 0;
int update_day = 0;
int update_time = 1;
int update_temp = 0;
int update_alarm = 0;

volatile int today = 0;
volatile int h_PM;
volatile int h = 23;
volatile int m = 45;
volatile int s = 00;
volatile int old_s = 0;

const int Buzzer = 10;
const int led_pin = PB5;
volatile boolean buzz = 0;
volatile boolean alarm_status = 0;
int alarm_h = 00;
int alarm_m = 30;
int alarm_ring = 0;

int temp;
int tempF;
int sensed_temp;
int old_temp = 100;
int CFoption = 0;

int lastState = LOW;
int currentState;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

boolean adjust_time = 0;
boolean adjust_case = 1;
boolean adjust_m = 0;
boolean adjust_h = 0;

// SETUP

void setup()
{
    cli();

    Serial.begin(9600);
    while (!Serial)
    {
        // Error Handle
    };

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Starting...");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("CMP3010 Project");
    lcd.setCursor(0, 1);
    lcd.print("Digital Clock");
    delay(5000);
    lcd.clear();
    dispClock(h, m, s);
    delay(100);
    dispToday();
    delay(100);
    TempMeasure();
    dispTemp();
    dispAlarm();

    DDRB |= (1 << led_pin);

    pinConfigCommon();
    pinMode(Buzzer, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(2), debounceButton, FALLING);

    TCCR1A = 0;
    TCNT1 = 0;
    OCR1A = 31250;
    TCCR1B &= ~(1 << WGM13);
    TCCR1B |= (1 << WGM12);
    TCCR1B = 0;
    TCCR1B |= (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
    sei();
}

// Display Function

void prefixZero(int n)
{

    if (n < 10)
    {
        lcd.print("0");
        lcd.print(n);
    }
    else
    {
        lcd.print(n);
    }
}

void dispHours(int h)
{
    if (update_time)
    {
        if (h >= 12)
        {
            h -= 12;
        }
    }
    prefixZero(h);
}

int dispClock(int h, int m, int s)
{
    lcd.setCursor(0, 0);
    if (update_time)
    {
        if (h < 12)
        {
            lcd.setCursor(8, 0);
            lcd.print("AM");
        }
        else
        {
            lcd.setCursor(8, 0);
            lcd.print("PM");
        }
    }
    else
    {
        lcd.setCursor(8, 0);
        lcd.print("    ");
    }

    lcd.setCursor(0, 0);
    dispHours(h);
    lcd.print(":");
    lcd.setCursor(3, 0);
    prefixZero(m);
    lcd.print(":");
    lcd.setCursor(6, 0);
    prefixZero(s);
    lcd.setCursor(10, 0);
    lcd.print("   ");
    lcd.setCursor(10, 1);
    lcd.print("  ");
    lcd.noCursor();
}

// Tempature

void TempMeasure()
{
    sensed_temp = analogRead(A0);
    temp = map(sensed_temp, 20, 358, -40, 125);
    if (old_temp != temp)
    {
        update_temp = 1;
    }
}

void dispTemp()
{
    lcd.setCursor(12, 0);
    lcd.print("    ");
    switch (CFoption)
    {
    case 0:
        if (temp < -10 || temp > 99)
        {
            lcd.setCursor(12, 0);
        }
        else
        {
            lcd.setCursor(13, 0);
        }
        lcd.print(temp);
        lcd.print("C");
        old_temp = temp;
        break;
    case 1:
        tempF = 32 + (temp * 9) / 5;
        if (tempF < -10 || tempF > 99)
        {
            lcd.setCursor(12, 0);
        }
        else
        {
            lcd.setCursor(13, 0);
        }
        lcd.print(tempF);
        lcd.print("F");
        old_temp = temp;
        break;
    }
    update_temp = 0;
}
// Display

void dispAlarm()
{
    if (alarm_status)
    {
        lcd.setCursor(0, 1);
        lcd.print("          ");
        if (update_time)
        {
            if (alarm_h < 12)
            {
                lcd.setCursor(5, 1);
                lcd.print("AM");
            }
            else
            {
                lcd.setCursor(5, 1);
                lcd.print("PM");
            }
            lcd.setCursor(0, 0);
        }

        lcd.setCursor(0, 1);

        dispHours(alarm_h);
        lcd.print(":");
        lcd.setCursor(3, 1);
        prefixZero(alarm_m);

        lcd.setCursor(8, 1);
        lcd.print("ON");
        update_alarm = 0;
    }
    if (alarm_status == 0)
    {
        lcd.setCursor(0, 1);
        lcd.print("          ");
        lcd.setCursor(7, 1);
        lcd.print("OFF");
        update_alarm = 0;
    }
}

void dispToday()
{
    lcd.setCursor(13, 1);
    switch (today)
    {
    case 0:
        lcd.print("Sun");
    case 1:
        lcd.print("Mon");
    case 2:
        lcd.print("Tue");
    case 3:
        lcd.print("Wed");
    case 4:
        lcd.print("Thu");
    case 5:
        lcd.print("Fri");
    case 6:
        lcd.print("Sat");
    }

    update_day = 0;
}

// Buttons

const int Interrupt_pin1 = 2;
const int ButtonPins[] = {16, 17, 18, 19};
const int HoldPressTime = 3000;

long debouncing_time = 20;
volatile unsigned long last_micros;

void debounceButton()
{
    if ((long)(micros() - last_micros) >= debouncing_time * 1000)
    {
        whichButton();
        last_micros = micros();
    }
}

void whichButton()
{
    pinConfigPress();

    for (int i = 0; i < sizeof(ButtonPins) / sizeof(int); i++)
    {
        if (!digitalRead(ButtonPins[i]))
        {
            shortPress(i);
        }
    }

    pinConfigCommon();
}

void pinConfigCommon()
{
    pinMode(Interrupt_pin1, INPUT_PULLUP);

    for (int i = 0; i < sizeof(ButtonPins) / sizeof(int); i++)
    {
        pinMode(ButtonPins[i], OUTPUT);
        digitalWrite(ButtonPins[i], LOW);
    }
}

void pinConfigPress()
{
    pinMode(Interrupt_pin1, OUTPUT);
    digitalWrite(Interrupt_pin1, LOW);

    for (int i = 0; i < sizeof(ButtonPins) / sizeof(int); i++)
    {
        pinMode(ButtonPins[i], INPUT_PULLUP);
    }
}

void shortPress(int button)
{
    switch (button)
    {
    case 0:
        shortButton1();
        break;
    case 1:
        shortButton2();
        break;
    case 2:
        shortButton3();
        break;
    case 3:
        shortButton4();
        break;
    }
}

void shortButton1()
{
    if (!adjust_time)
    {
        update_time = !update_time;
        lcd.setCursor(0, 0);
        lcd.print("          ");
        update_reset = 1;
    }
    else
    {
        if (adjust_h)
        {
            // close adjust mode
            adjust_time = 0;
            adjust_h = 0;
            adjust_m = 0;
        }
        else if (adjust_m)
        {
            // pass hours adjuster
            adjust_h = 1;
        }
        else
        {
            holdButton1();
        }
    }
}

void holdButton1()
{
    // time
    adjust_time = 1;
    adjust_case = 0;
    adjust_m = 1;
    adjust_h = 0;
}

void shortButton2()
{
    if (!adjust_time)
    {
        update_alarm = 1;
        alarm_status = !alarm_status;
        Serial.println(alarm_status);
        // alarm_status?close_alarm:NULL;
    }
    else
    {
        if (adjust_h)
        {
            // close adjust mode
            adjust_time = 0;
            adjust_h = 0;
            adjust_m = 0;
        }
        else if (adjust_m)
        {
            // pass hours adjuster
            adjust_h = 1;
        }
        else
        {
            holdButton2();
        }
    }
}
void holdButton2()
{
    // alarm
    adjust_m = 1;
    adjust_h = 0;
    adjust_time = 1;
    adjust_case = 1;
}

void shortButton3()
{
    if (adjust_time)
    {
        switch (adjust_case)
        {
        case 0:
            if (adjust_m)
            {
                m++;
            }
            if (adjust_h)
            {
                h++;
            }
            else
            {
                break;
            }
        case 1:
            if (adjust_m)
            {
                alarm_m += 5;
            }
            if (adjust_h)
            {
                alarm_h++;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        update_temp = 1;
        CFoption = !CFoption;
    }
}

void shortButton4()
{
    if (alarm_status)
    {
        alarm_m = m + 5;
        if (alarm_m >= 60)
        {
            alarm_m -= 60;
            alarm_h++;
        }
    }
    noTone(Buzzer);
    update_reset = 1;
}

void AlarmSound()
{
    dispClock(h, m, s);
    if (buzz)
    {
        tone(Buzzer, 16000);
    }
    if (s >= 60)
    {
        s = 0;
        m++;
    }
}

// Main Loop

void loop()
{
    if (update_reset)
    {
        lcd.clear();
        dispClock(h, m, s);
        dispAlarm();
        dispToday();
        dispTemp();
        update_reset = 0;
    }

    if (old_s != s)
    {
        dispClock(h, m, s);
    }
    if (update_alarm)
    {
        dispAlarm();
    }
    if (update_day)
    {
        dispToday();
    }
    TempMeasure();
    if (update_temp)
    {
        dispTemp();
    }

    if (h == alarm_h && m == alarm_m && alarm_status)
    {
        alarm_ring = 1;
        if (alarm_ring)
        {
            AlarmSound();
        }
        else
        {
            noTone(Buzzer);
        }
    }

    if (s >= 60)
    {
        s -= 60;
        m++;
    }

    if (m >= 60)
    {
        m -= 60;
        h++;
    }

    if (h >= 24)
    {
        h -= 24;
        update_day = 1;
        today++;
        if (today >= 7)
        {
            today = 0;
        }
    }
}

ISR(TIMER1_COMPA_vect)
{
    old_s = s;
    s++;
    PORTB ^= (1 << led_pin);
    buzz = !buzz;
}
