const int BTN_MOTOR_PIN = PUSH1; //pin tastera
const int BTN_BELT_PIN = PUSH2; //pin tastera
const int BUZZER_PIN = 27; //pin buzzera

const long MAX_motorStatus = 50; //max. trajanje treptaja tastera paljenja/gašenja motora
const long MAX_beltStatus = 50; //max. trajanje treptaja tastera vezivanja motora

const long BIP_DURATION = 50;    //trajanje bip tona
const long SILENCE_DURATION = 450; //trajanje pauze izmedju bip tonova

//stanja konacnog automata:

typedef enum {MS0, MS1, MS2, MS3} Btn1FsmStateType;       //stanja Debouncer FSM
typedef enum {BS0, BS1, BS2, BS3} Btn2FsmStateType;       //stanja Debouncer FSM
typedef enum {SILENCE,BIP } BuzzFsmStateType; //stanja Buzzer FSM
Btn1FsmStateType btn1FsmState = MS0;      //tekuce stanje MOTORA FSM
Btn2FsmStateType btn2FsmState = BS0;      //tekuce stanje POJASA FSM
BuzzFsmStateType buzzFsmState = SILENCE; //tekuce stanje Buzzer FSM

int btnMOTOR_Value = LOW;  //trenutni nivo pina tastera
int btnBELT_Value = LOW;  //trenutni nivo pina tastera
int btnMOTOR_State = LOW;     //stanje tastera (sa otklonjenim treperenjem)
int btnBELT_State = LOW;     //stanje tastera (sa otklonjenim treperenjem)
int buzzerState = LOW;  //stanje buzzera

//tajmerske promenljive
long currentMOTOR_Time = 0;          //tekuce vreme
long currentBELT_Time = 0;          //tekuce vreme
long currentTime=0;
long btnMotorTimeout = 0; //trenutak isteka cekanja na treptaj
long btnBeltTimeout = 0; //trenutak isteka cekanja na treptaj
long bipTimeout = 0;       //trenutak zavrsetka bip tona
long silenceTimeout = 0; //trenutak zavrsetka pauze izmedju bip tonova



void setup()
{
  pinMode(BTN_MOTOR_PIN,INPUT);
  pinMode(BTN_BELT_PIN,INPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  Serial.begin(9600);
}
void loop()
{
  //--- Ulaz: --------------------------------
  btnMOTOR_Value = digitalRead(BTN_MOTOR_PIN);
  currentMOTOR_Time = millis();
  //------ MOTOR FSM----------------------
  btnMOTOR_State = LOW;

  switch(btn1FsmState){
    case MS0:
      if(btnMOTOR_Value == HIGH){
        btnMotorTimeout = currentMOTOR_Time + MAX_motorStatus;
        btn1FsmState = MS1;
      }
      break;
    case MS1:
      if(btnMOTOR_Value == LOW){
        btn1FsmState = MS0;
      } else if(currentMOTOR_Time >= btnMotorTimeout){
        btn1FsmState = MS2;
      }
      break;
    case MS2:
      btnMOTOR_State = HIGH;
      if(btnMOTOR_Value == LOW){
        btnMotorTimeout = currentMOTOR_Time + MAX_motorStatus;
        btn1FsmState = MS3;
      }
      break;
    case MS3:
      btnMOTOR_State = HIGH;
      if(btnMOTOR_Value == HIGH){
        btn1FsmState = MS2;
      } else if(currentMOTOR_Time >= btnMotorTimeout){
        btn1FsmState = MS0;
      }
      break;
  }
 //--- Ulaz: --------------------------------
  btnBELT_Value = digitalRead(BTN_BELT_PIN);
  currentBELT_Time = millis();
  //------ BELT FSM----------------------
  btnBELT_State = LOW;

  switch(btn2FsmState){
    case BS0:
      if(btnBELT_Value == HIGH){
        btnBeltTimeout = currentBELT_Time + MAX_beltStatus;
        btn2FsmState = BS1;
      }
      break;
    case BS1:
      if(btnBELT_Value == LOW){
        btn2FsmState = BS0;
      } else if(currentBELT_Time >= btnBeltTimeout){
        btn2FsmState = BS2;
      }
      break;
    case BS2:
      btnBELT_State = HIGH;
      if(btnBELT_Value == LOW){
        btnBeltTimeout = currentBELT_Time + MAX_beltStatus;
        btn2FsmState = BS3;
      }
      break;
    case BS3:
      btnBELT_State = HIGH;
      if(btnBELT_Value == HIGH){
        btn2FsmState = BS2;
      } else if(currentBELT_Time >= btnBeltTimeout){
        btn2FsmState = BS0;
      }
      break;
  }
  //----  BUZZER FSM ------------------------
  buzzerState = LOW;
  switch(buzzFsmState){
   case SILENCE:
     if(btnBELT_State == HIGH || btnMOTOR_State == LOW){
       buzzFsmState = SILENCE;
     }
     else if(btnMOTOR_State == HIGH && btnBELT_State == LOW){
          buzzFsmState = BIP; 
     }
     break;
   case BIP: 
     buzzerState = HIGH;
     if(btnBELT_State == HIGH || btnMOTOR_State == LOW){
       buzzFsmState = SILENCE;
     }
     break;
   
  }
  Serial.println(buzzerState);
  Serial.print("stanje motora: ");
  Serial.println(btnMOTOR_State);
  Serial.print("pojas: ");
  Serial.println(btnBELT_State);
  //--- Izlaz: --------------------------------
  digitalWrite(BUZZER_PIN,buzzerState);
  
}
