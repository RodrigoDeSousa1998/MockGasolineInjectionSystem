//!TODO: Implement timers for angular speed calculations
//!TODO: Implement potentiometer for load detection (throttle valve opening)
//!TODO: Implement lookup tables for ignition timing and injection timing
//!TODO: Implement temperature correction for the timing calculations
//!TODO: Implement pushbuttons for encoder position zero-ing and gear change
//!TODO: Implement buzzer for rpm limit warning
//!TODO: Implement output with rpm and gear

//Macro Definitions
#define CRANK_ENCODER 0U
#define CAM_ENCODER   1U
#define FULL_TURN     30U
#define ONE_STEP      12U
#define INIT          0U
#define OFF           0U
#define ON            1U

//Macro Pin Definitions
#define ENCODER_CRK_DT   2U
#define ENCODER_CRK_CLK  3U
#define ENCODER_CRK_SW   4U
#define ENCODER_CAM_DT   5U
#define ENCODER_CAM_CLK  6U
#define RED_LED_1        8U
#define RED_LED_2        9U
#define RED_LED_3        10U
#define GREEN_LED_1      11U
#define GREEN_LED_2      12U
#define GREEN_LED_3      13U

//Auxiliary Functions Declarations
int encoderReader(int pinCLK, int pinDT);

//Global Variables

//Main Functions
void setup() 
{
  pinMode (RED_LED_1   , OUTPUT);
  pinMode (RED_LED_2   , OUTPUT);
  pinMode (RED_LED_3   , OUTPUT);
  pinMode (GREEN_LED_1 , OUTPUT);
  pinMode (GREEN_LED_2 , OUTPUT);
  pinMode (GREEN_LED_3 , OUTPUT);

  pinMode (ENCODER_CRK_DT  , INPUT);
  pinMode (ENCODER_CRK_CLK , INPUT);
  pinMode (ENCODER_CRK_SW  , INPUT);  
  pinMode (ENCODER_CAM_DT  , INPUT);
  pinMode (ENCODER_CAM_CLK , INPUT);

  
  Serial.begin (9600);
}

void loop() 
{  

  int encoder_pos_count = INIT;
  encoder_pos_count = encoderReader(ENCODER_CRK_CLK, ENCODER_CRK_DT);

  Serial.print("Encoder Pos: ");
  Serial.println(encoder_pos_count);

  // if (encoder_pos_count == 9)
  // {
  //   digitalWrite(GREEN_LED_1, ON);
  // }
  // else
  // {
  //   digitalWrite(GREEN_LED_1, OFF);
  // }

  // if (encoder_pos_count == 19)
  // {
  //   digitalWrite(GREEN_LED_2, ON);
  // }
  // else
  // {
  //   digitalWrite(GREEN_LED_2, OFF);
  // }

  // if (encoder_pos_count == 29)
  // {
  //   digitalWrite(GREEN_LED_3, ON);
  // }
  // else
  // {
  //   digitalWrite(GREEN_LED_3, OFF);
  // }

}

//Auxiliary Functions Implementations
int encoderReader(int pinCLK, int pinDT)
{
  static int last_switch_A = INIT;
  static int last_switch_B = INIT;
  static int encoder_pos_count = INIT;
  static int flip = INIT;
  
  int switch_A = digitalRead(pinCLK);
  int switch_B = digitalRead(pinDT);
  
  // int button   = digitalRead(ENCODER_CRK_SW);
  // if (button == 1U)
  // {
  //   encoder_pos_count = INIT;
  // }

  if (switch_A != last_switch_A)
  {    
    if ((switch_A > switch_B) || flip) 
    {
      encoder_pos_count++;
      
      // Serial.print("Encoder Pos: ");
      // Serial.println(encoder_pos_count);

      flip = ~flip;
    }  
    else
    {
      // Serial.print("Encoder Pos: ");
      // Serial.println(encoder_pos_count);
      encoder_pos_count--;
    }
  }

  if ((encoder_pos_count == FULL_TURN) || (encoder_pos_count < 0))
  {
    encoder_pos_count = INIT;
  }

  last_switch_A = switch_A;
  last_switch_B = switch_B;

  return encoder_pos_count;
}