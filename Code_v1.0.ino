//Pin Definition
int encoderCRK_DT = 2; // Connected to DT
int encoderCRK_CLK = 3; // Connected to CLK 
int encoderCAM_DT = 4; // Connected to DT 
int encoderCAM_CLK = 5; // Connected to CLK 
int red_led_1 = 6;
int red_led_2 = 7;
int red_led_3 = 8;
int red_led_4 = 9;
int green_led_1 = 10;
int green_led_2 = 11;
int green_led_3 = 12;
int green_led_4 = 13; 
 
//Global Variables
int aVal;
boolean bCW;
int last_CAM_pos;
int last_CRK_pos;
int encoderPosCount = 0;

//Main Functions
void setup() 
{
  pinMode (red_led_1   , OUTPUT);
  pinMode (red_led_2   , OUTPUT);
  pinMode (red_led_3   , OUTPUT);
  pinMode (red_led_4   , OUTPUT);
  pinMode (green_led_1 , OUTPUT);
  pinMode (green_led_2 , OUTPUT);
  pinMode (green_led_3 , OUTPUT);
  pinMode (green_led_4 , OUTPUT);

  pinMode (encoderCRK_CLK , INPUT);
  pinMode (encoderCRK_DT  , INPUT);
  pinMode (encoderCAM_CLK , INPUT);
  pinMode (encoderCAM_DT  , INPUT);
  
  /* Read Pin A
  Whatever state it's in will reflect the last position
  */
  last_CAM_pos = digitalRead(encoderCAM_CLK);
  last_CRK_pos = digitalRead(encoderCRK_CLK);


  Serial.begin (9600);
}

void loop() 
{  
  aVal = digitalRead(encoderCRK_CLK);
  if (aVal != last_CRK_pos)
  {                                     // Means the knob is rotating
                                        // if the knob is rotating, we need to determine direction
                                        // We do that by reading pin B.
    if (digitalRead(encoderCRK_DT) != aVal)
    {                                     // Means pin A Changed first - We're Rotating Clockwise
      encoderPosCount ++; bCW = true;
    }  
    else 
    {                              // Otherwise B changed first and we're moving CCW bCW = false;
      encoderPosCount--;
    }
  
    Serial.print ("Rotated: ");

    if (bCW)
    {
      Serial.println ("clockwise");
    }
    else
    {
      Serial.println("counterclockwise");
    }

    Serial.print("Encoder Position: ");
    Serial.println(encoderPosCount);

  }
  last_CRK_pos = aVal;
}
