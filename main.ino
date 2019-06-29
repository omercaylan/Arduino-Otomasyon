
#include "Stepper.h"
#define SW1_Malzeme 9 // Malzeme algýlama swich'inin baglý olduðu pin girilecek. (dgtPÝN X - GND)

#define SW2_Motor_home 6 // Motor konum algýlama swich'inin baðlý olduðu pin girilecek. ( ditPÝN X - GND)

#define SW3_Motor_last 8 // motor konum pini

#define ROLE1_Kaynak_Cihazi 3 // Cihazýn baðlý olduðu rolenin pini girilecek.

#define ROLE2_Valf 5 // Selenoidvalfin baðlý olduðu pin girilecek.

#define ROLE3_Vakum_Valf 4 // Vakumun baðlý olduðu role girilecek.

const int PIN_START = A5; // Start butonunun baðlý olduðu analog pin girilecek.
const int PIN_STOP = A4;  //stop pin
int xPin = A0;            // A0-A5 analog pinlerinden herhangi birine baðlanabilir.
int yPin = A1;            // A0-A5 analog pinlerinden herhangi birine baðlanabilir.
//int butonPin = 2; // Joystick buton pini arduino baðlantýsý (Joystick SW çýkýþý)
const int stepsPerRevolution = 200; // change this to fit the number of steps per revolution
static int state = 0;
static int Pause_Mode = 0;
static int baslangic;
static int button = 0;
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);

void Emergency()
{
  unsigned long i;
  delay_1();
}
void delay_1()
{
  delay(3000);
  delay(3000);
  Serial.println("Emergency");
}

void setup()
{
  attachInterrupt(digitalPinToInterrupt(2), Emergency, 0);

  pinMode(ROLE1_Kaynak_Cihazi, OUTPUT);

  pinMode(ROLE2_Valf, OUTPUT);

  pinMode(ROLE3_Vakum_Valf, OUTPUT);

  pinMode(PIN_START, INPUT);

  pinMode(PIN_STOP, INPUT);

  pinMode(SW1_Malzeme, INPUT_PULLUP);

  pinMode(SW2_Motor_home, INPUT_PULLUP);

  pinMode(SW3_Motor_last, INPUT_PULLUP);

  myStepper.setSpeed(1200);

  Serial.begin(9600);
}

void loop()
{

  int swich1_param;

  int swich2_param;

  int swich3_param;

  while (1)
  {

    int pin_start = analogRead(PIN_START);

    if (pin_start >= 1015)

    {
      Serial.print("pin start->");
      Serial.println(pin_start);
      Serial.println("pin start girdi");
      state = 1;
      Pause_Mode = 0;
    }

    //en son iþlemini bitirip pause'a geçecek
    int pin_stop = analogRead(PIN_STOP);

    //stop interrupt'a baðlanacak
    if (pin_stop >= 1015)

    {
      Serial.print("pin_stop ->");
      Serial.println(PIN_STOP);
      //   state =0;
      Pause_Mode = 1;
    }

    switch (state)
    {

      //bu state'in görevi boþta durmak

    case 0:

      state = 0;
      pin_stop = analogRead(PIN_STOP);
      if ((pin_stop >= 1015))
      {
        state = 4;
        Pause_Mode = 1;
        digitalWrite(ROLE2_Valf, HIGH);
        break;
      }
      Serial.println("case 0 IDLE");
      digitalWrite(ROLE1_Kaynak_Cihazi, HIGH);

      digitalWrite(ROLE2_Valf, HIGH);

      digitalWrite(ROLE3_Vakum_Valf, HIGH);
      delay(500);

      break;

    case 1:
      Serial.println("state 1");
      digitalWrite(ROLE1_Kaynak_Cihazi, HIGH);

      digitalWrite(ROLE2_Valf, HIGH);

      digitalWrite(ROLE3_Vakum_Valf, HIGH);

      swich2_param = digitalRead(SW2_Motor_home);
      Serial.print("swich2_param ->");
      Serial.println(swich2_param);
      while (swich2_param <= 0)
      {
        Serial.println("state 1->motor geri dönüyor -10");
        myStepper.step(200);

        swich2_param = digitalRead(SW2_Motor_home);
        // delay(500);
      }

      state = 3;

      break;

    case 2:

      printf("case 2\n");

      state = 3;

      break;

    case 3:
      Serial.println("case 3 running");
      swich1_param = digitalRead(SW1_Malzeme);
      Serial.print("swich1_param->");
      Serial.println(swich1_param);
      if (swich1_param == 1)
      {

        state = 4;
      }
      else
      {

        state = 0;
      }
      break;

    case 4:
      //state = 5;
      int xPozisyonu = 0;
      int yPozisyonu = 0;
      int butonDurum = 0;

      while (Pause_Mode == 1)
      {

        pin_start = analogRead(PIN_START);
        if (pin_start >= 1015)
        {
          Serial.println("pin start girdi -> Pause_Mode");
          state = 4;
          Pause_Mode = 0;
        }

        Serial.println("pause mode");
        xPozisyonu = analogRead(xPin);
        yPozisyonu = analogRead(yPin);
        //sol
        Serial.print("xPozisyonu->");
        Serial.println(xPozisyonu);
        Serial.print("yPozisyonu->");
        Serial.println(yPozisyonu);
        swich2_param = digitalRead(SW2_Motor_home);
        swich3_param = digitalRead(SW3_Motor_last);
        if ((swich2_param == 0) && (xPozisyonu > 1000))
        {

          myStepper.step(200);
        }
        else if ((xPozisyonu < 10) && (swich3_param == 0))
        {

          myStepper.step(-200);
        }
        if (yPozisyonu > 1000)
        {
          Serial.println("Y ekseninde Yukarý -> röle kapat");
          digitalWrite(ROLE2_Valf, LOW);
        }
        else if (yPozisyonu < 10)
        {

          Serial.println("Y ekseninde Aþagý -> röle aç");
          digitalWrite(ROLE2_Valf, HIGH);
        }
        pin_start = analogRead(PIN_START);
        if (pin_start > 1015)
        {
          state = 3;
          break;
        }
      }
      // if(state != 3){
      Serial.println("state 4 running");
      Serial.println("ROLE3_Vakum_Valf açilmasi beklendi");
      delay(1000);
      digitalWrite(ROLE3_Vakum_Valf, LOW);

      digitalWrite(ROLE1_Kaynak_Cihazi, LOW);
      Serial.println("ROLE1_Kaynak_Cihazi açildi");
      delay(250);

      digitalWrite(ROLE1_Kaynak_Cihazi, HIGH);
      Serial.println("ROLE1_Kaynak_Cihazi HIGH");
      // delay(2000);
      //  Serial.println("2000 beklendi");
      state = 5;
      // }
      // break;

    case 5:
      Serial.println("state 5 running");
      swich2_param = digitalRead(SW2_Motor_home);

      while (swich2_param <= 0)
      {

        Serial.println("state 5-> -200 motor dönüyor");
        myStepper.step(200);

        swich2_param = digitalRead(SW2_Motor_home);
      }
      Serial.println("state 6");
      state = 6;

      //break;

    case 6:
      Serial.println("state 6 running");
      delay(1000);

      digitalWrite(ROLE3_Vakum_Valf, HIGH);

      digitalWrite(ROLE2_Valf, LOW);

      delay(1000);
      myStepper.step(-300);
      state = 7;

      // break;

    case 7:

      Serial.println("state 7 running");

      swich3_param = digitalRead(SW3_Motor_last);

      //delay(500);
      swich1_param = digitalRead(SW1_Malzeme);
      // delay(500);

      Serial.print("state 7 swich3_param->");
      Serial.println(swich3_param);
      Serial.print("state 7 swich1_param->");
      Serial.println(swich1_param);

      delay(1000);
      while ((swich3_param == 0) || (swich1_param == 1))
      {
        pin_stop = analogRead(PIN_STOP);
        if ((pin_stop > 1015))
        {
          state = 4;
          Pause_Mode = 1;
          break;
        }

        Serial.println("state 7 -200");
        myStepper.step(-stepsPerRevolution);
        swich3_param = digitalRead(SW3_Motor_last);
        swich1_param = digitalRead(SW1_Malzeme);

        if (swich1_param == 1)
        {
          state = 4;
          break;
        }

        if (swich3_param == 1)
        {
          break;
        }
      }
      if ((swich3_param == 1) && (swich1_param == 1))
      {
        state = 4;
      }

      if ((swich3_param == 1) && (swich1_param == 0))
      {
        while (1)
        {
          digitalWrite(ROLE2_Valf, HIGH);
          delay(1000);
          Serial.println("state 7 200");
          myStepper.step(200);

          digitalWrite(ROLE2_Valf, LOW);
          delay(1000);

          while (1)
          {
            Serial.println("state 7 -200");
            pin_stop = analogRead(PIN_STOP);
            if ((pin_stop >= 1015))
            {
              state = 4;
              Pause_Mode = 1;
              break;
            }
            myStepper.step(-200);

            if (digitalRead(SW1_Malzeme) == 1)
            {
              Serial.println("state 7 -200 -> SW1_Malzeme = 1");
              digitalWrite(ROLE2_Valf, HIGH);
              //delay(1000);
              state = 4;

              break;
            }
            //  Serial.println("state 7 -200 -> ikinci if");
            if (digitalRead(SW3_Motor_last) == 1)
            {
              //  Serial.println("state 7 -200 -> W3_Motor_last) == 1");
              state = 7;
              break;
            }
            // Serial.println("state 7 -200 -> W3_Motor_last) == 1--> ELSE");
          }
          //  break;
          if (digitalRead(SW1_Malzeme) == 1)
          {
            state = 4;
            break;
          }
          pin_stop = analogRead(PIN_STOP);
          if ((pin_stop >= 1015))
          {
            state = 4;
            Pause_Mode = 1;
            digitalWrite(ROLE2_Valf, HIGH);
            break;
          }
        }
      }
      break;
    default:
      Serial.println("default");
      break;
    }
  }
}