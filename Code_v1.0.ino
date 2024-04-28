//Macro Definitions
#define CRANK_ENCODER 0U
#define CAM_ENCODER   1U
#define FULL_TURN     30U
#define ONE_STEP      12U
#define INIT          0U

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

 
//Global Variables
int aVal;
boolean bCW;
int last_CAM_pos;
int last_CRK_pos;

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
  static int last_switch_A = INIT;
  static int last_switch_B = INIT;
  static int encoder_pos_count = INIT;
  static int flip = INIT;
  
  int switch_A = digitalRead(ENCODER_CRK_CLK);
  int switch_B = digitalRead(ENCODER_CRK_DT);
  
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
      
      Serial.print("Encoder Pos: ");
      Serial.println(encoder_pos_count);

      flip = ~flip;
    }  
    else
    {
      Serial.print("Encoder Pos: ");
      Serial.println(encoder_pos_count);
      encoder_pos_count--;
    }
  }

  if ((encoder_pos_count == FULL_TURN) || (encoder_pos_count < 0))
  {
    encoder_pos_count = INIT;
  }

  last_switch_A = switch_A;
  last_switch_B = switch_B;

  // aVal = digitalRead(ENCODER_CRK_CLK);
  // if (aVal != last_CRK_pos)
  // {                                     // Means the knob is rotating
  //                                       // if the knob is rotating, we need to determine direction
  //                                       // We do that by reading pin B.
  //   if (digitalRead(ENCODER_CRK_DT) != aVal)
  //   {                                     // Means pin A Changed first - We're Rotating Clockwise
  //     encoderPosCount++; 
  //     bCW = true;
  //   }  
  //   else 
  //   {                              // Otherwise B changed first and we're moving CCW bCW = false;
  //     encoderPosCount--;
  //   }
  
  //   Serial.print ("Rotated: ");

  //   if (bCW)
  //   {
  //     Serial.println ("clockwise");
  //   }
  //   else
  //   {
  //     Serial.println("counterclockwise");
  //   }

  //   Serial.print("Encoder Position: ");
  //   Serial.println(encoderPosCount);
  // }
  // last_CRK_pos = aVal;
}