//TODO*: Implement timers for angular speed calculations                         | (DONE)
//TODO*: Implement potentiometer for load detection (throttle valve opening)     | (DONE)
//TODO*: Implement lookup tables for ignition timing and injection timing        | (DONE)
//TODO!: Implement motor control with pwm having load as an input                |
//TODO!: Implement temperature correction for the timing calculations            |
//TODO!: Implement pushbuttons for encoder position zero-ing and gear change     |
//TODO!: Implement buzzer for rpm limit warning                                  |
//TODO!: Implement output with rpm and gear                                      |

//Macro Definitions
#define FULL_TURN           30U
#define INIT                0U
#define OFF                 0U
#define ON                  1U
#define FALSE               0U
#define TRUE                1U
#define POT_MIN             12.00F   
#define POT_MAX             1023.00F
#define SCALE_MAX           100.00F
#define ENCODER_RESOLUTION  12U
#define ADMISSION           0U
#define COMPRESSION         1U
#define EXPANSION           2U
#define EXHAUST             3U
#define FIRST_CYLINDER      0U
#define SECOND_CYLINDER     1U
#define THIRD_CYLINDER      2U
#define NUMBER_OF_CYLINDERS 3U

//Macro Pin Definitions
#define ENCODER_CRK_CLK  2U
#define ENCODER_CRK_DT   3U
#define ENCODER_CRK_SW   4U
#define ENCODER_CAM_DT   5U
#define ENCODER_CAM_CLK  6U
#define ENCODER_CAM_SW   7U
#define RED_LED_1        8U
#define RED_LED_2        9U
#define RED_LED_3        10U
#define GREEN_LED_1      11U
#define GREEN_LED_2      12U
#define GREEN_LED_3      13U
#define POTENTIOMETER    A0

//Type Definitions
typedef struct
{
  int full_turn;
  int position;
} shaft;

typedef struct
{
  int number;
  int cycle_point;
  int position;
  void (*update_data)(int);
} cylinder;

//Auxiliary Functions Declarations
shaft encoderReader(int pinCLK, int pinDT);
void update (int cylinder);
void spark (int cylinder, int state);
void inject (int cylinder, int state);

//Global Variables      

                                                                               /*Throttle Position Switch*/
const int ignition_map[11][10] = {{ 12 , 12 , 12 , 24 , 24 , 36 , 36 , 60 , 60 , 60 },  /*  0%  */   
                                  { 12 , 12 , 12 , 24 , 36 , 36 , 36 , 60 , 60 , 60 },  /*  10% */
                                  { 12 , 12 , 24 , 24 , 36 , 36 , 36 , 48 , 60 , 60 },  /*  20% */
                                  { 12 , 24 , 24 , 36 , 36 , 36 , 36 , 48 , 48 , 60 },  /*  30% */
                                  { 12 , 24 , 24 , 36 , 36 , 36 , 48 , 48 , 48 , 48 },  /*  40% */
                                  { 12 , 24 , 24 , 36 , 36 , 36 , 48 , 48 , 48 , 48 },  /*  50% */
                                  { 12 , 24 , 24 , 36 , 36 , 36 , 48 , 48 , 48 , 48 },  /*  60% */
                                  { 12 , 12 , 24 , 24 , 36 , 36 , 36 , 48 , 48 , 48 },  /*  70% */
                                  { 12 , 12 , 24 , 24 , 24 , 36 , 36 , 36 , 48 , 48 },  /*  80% */
                                  { 12 , 12 , 12 , 24 , 24 , 24 , 36 , 36 , 36 , 48 },  /*  90% */
                                  { 12 , 12 , 12 , 24 , 24 , 24 , 24 , 36 , 36 , 36 }}; /* 100% */  
                                  /*10   20   30   40   50   60   70   80   90   100*/
                                                         /*RPM*/

                                                                                  /*Throttle Position Switch*/
const int injection_map[10][10] = {{ 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 10% */ 
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 20% */
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 30% */
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 40% */
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 50% */
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 60% */
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 70% */
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 80% */
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }, /* 90% */
                                   { 96 , 96 , 96 , 96 , 132 , 168 , 168 , 192 , 192 , 192 }};/*100% */
                                   /*10   20   30   40   50    60    70    80    90    100*/
                                                             /*RPM*/                                   

                                                                          /*Throttle Position Switch*/
const int fuel_map[11][10] = {{ 12 , 12 , 12 , 24 , 24 , 24 , 24 , 36 , 36 , 36 }, /*  0%  */  
                              { 12 , 12 , 12 , 24 , 24 , 24 , 36 , 36 , 36 , 48 }, /*  10% */
                              { 12 , 12 , 24 , 24 , 24 , 36 , 36 , 36 , 48 , 48 }, /*  20% */
                              { 12 , 12 , 24 , 24 , 36 , 36 , 36 , 48 , 48 , 48 }, /*  30% */
                              { 12 , 24 , 24 , 36 , 36 , 36 , 48 , 48 , 48 , 48 }, /*  40% */
                              { 12 , 24 , 24 , 36 , 36 , 36 , 48 , 48 , 48 , 48 }, /*  50% */
                              { 12 , 24 , 24 , 36 , 36 , 36 , 48 , 48 , 48 , 48 }, /*  60% */
                              { 12 , 24 , 24 , 36 , 36 , 36 , 36 , 48 , 48 , 60 }, /*  70% */
                              { 12 , 12 , 24 , 24 , 36 , 36 , 36 , 48 , 60 , 60 }, /*  80% */
                              { 12 , 12 , 12 , 24 , 36 , 36 , 36 , 60 , 60 , 60 }, /*  90% */
                              { 12 , 12 , 12 , 24 , 24 , 36 , 36 , 60 , 60 , 60 }};/* 100% */   
                              /*10   20   30   40   50   60   70   80   90   100*/
                                                     /*RPM*/

static cylinder first_cyl  = {FIRST_CYLINDER , EXPANSION  , 0  , update}; /* Cycle Positions at beginning*/
static cylinder second_cyl = {SECOND_CYLINDER, EXHAUST    , 240, update};
static cylinder third_cyl  = {THIRD_CYLINDER , ADMISSION  , 480, update};

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
  static int spark_advance = INIT;
  static int start_of_injection = INIT;
  static int fuel_mass = INIT;
  static int cylinder = FIRST_CYLINDER;

  unsigned long int init_time = millis();
  shaft cranckshaft = {FALSE, INIT};
  do
  {
    cranckshaft = encoderReader(ENCODER_CRK_CLK, ENCODER_CRK_DT);

    for (int i = 0; i < NUMBER_OF_CYLINDERS; i++)
    {
      switch (i)
      {
      case FIRST_CYLINDER:
        if (first_cyl.cycle_point == ADMISSION)
        {
          static int spark_start = INIT; 
          int ingnition_point = (360 - spark_advance) / ENCODER_RESOLUTION;
          if (cranckshaft.position == ingnition_point)
          {
            spark_start = cranckshaft.position;
            spark(FIRST_CYLINDER, ON);
          }

          static int injection_start = INIT;
          int injection_point = (360 - start_of_injection) / ENCODER_RESOLUTION;
          if (cranckshaft.position == injection_point)
          {
            injection_start = cranckshaft.position;
            inject(FIRST_CYLINDER, ON);
          }

          if ((cranckshaft.position - spark_start) > 1)
          {
            spark(FIRST_CYLINDER, OFF);
          }

          if ((cranckshaft.position - injection_start) > (fuel_mass / ENCODER_RESOLUTION))
          {
            inject(FIRST_CYLINDER, OFF);
          }
        }
        break;
      case SECOND_CYLINDER:
        if (second_cyl.cycle_point == ADMISSION)
        {
          static int spark_start = INIT; 
          int ingnition_point = (360 - spark_advance) / ENCODER_RESOLUTION;
          if (cranckshaft.position == ingnition_point)
          {
            spark_start = cranckshaft.position;
            spark(SECOND_CYLINDER, ON);
          }

          static int injection_start = INIT;
          int injection_point = (360 - start_of_injection) / ENCODER_RESOLUTION;
          if (cranckshaft.position == injection_point)
          {
            injection_start = cranckshaft.position;
            inject(SECOND_CYLINDER, ON);
          }

          if ((cranckshaft.position - spark_start) > 1)
          {
            spark(SECOND_CYLINDER, OFF);
          }

          if ((cranckshaft.position - injection_start) > (fuel_mass / ENCODER_RESOLUTION))
          {
            inject(SECOND_CYLINDER, OFF);
          }
        }
        break;
      case THIRD_CYLINDER:
        if (third_cyl.cycle_point == ADMISSION)
        {
          static int spark_start = INIT; 
          int ingnition_point = (360 - spark_advance) / ENCODER_RESOLUTION;
          if (cranckshaft.position == ingnition_point)
          {
            spark_start = cranckshaft.position;
            spark(THIRD_CYLINDER, ON);
          }

          static int injection_start = INIT;
          int injection_point = (360 - start_of_injection) / ENCODER_RESOLUTION;
          if (cranckshaft.position == injection_point)
          {
            injection_start = cranckshaft.position;
            inject(THIRD_CYLINDER, ON);
          }

          if ((cranckshaft.position - spark_start) > 1)
          {
            spark(THIRD_CYLINDER, OFF);
          }

          if ((cranckshaft.position - injection_start) > (fuel_mass / ENCODER_RESOLUTION))
          {
            inject(THIRD_CYLINDER, OFF);
          }
        }
        break;
      
      default:
        break;
      }
    }

  } while (cranckshaft.full_turn != TRUE);
  unsigned long int final_time = millis();

                                    /*Conversion Factor from milliseconds to minutes*/
  float elapsed_time = (float)(final_time - init_time) * (0.001/60);
  unsigned int rpm = 1/elapsed_time;

  int pot_val = analogRead(POTENTIOMETER);
                          /*Conversion Factor Calculation*/
  int load = SCALE_MAX * (float)(pot_val/POT_MAX);

  int load_idx = load/10;
  int rpm_idx = (rpm/10) - 1;

  spark_advance      = ignition_map [load_idx][rpm_idx];
  start_of_injection = injection_map[load_idx][rpm_idx];
  fuel_mass          = fuel_map     [load_idx][rpm_idx];
}

//Auxiliary Functions Implementations
shaft encoderReader(int pinCLK, int pinDT)
{
  static int last_switch_A = INIT;
  static int last_switch_B = INIT;
  static shaft encoder = {FALSE, INIT};
  static int flip = INIT;
 
  int switch_A = digitalRead(pinCLK);
  int switch_B = digitalRead(pinDT);
  
  // int button   = digitalRead(ENCODER_CRK_SW);
  // if (button == 1U)
  // {
  //   encoder.position = INIT;
  // }

  if (switch_A != last_switch_A)
  {    
    if ((switch_A > switch_B) || flip) 
    {
      encoder.position++;
      
      first_cyl.update_data(FIRST_CYLINDER);
      second_cyl.update_data(SECOND_CYLINDER);
      third_cyl.update_data(THIRD_CYLINDER);
      
      Serial.print("Encoder Pos: ");
      Serial.println(encoder.position);

      flip = ~flip;
    }  
  }

  if (encoder.position == FULL_TURN)
  {
    encoder.full_turn = TRUE;
    encoder.position = INIT;
  }

  if ((encoder.position == FULL_TURN) || (encoder.position < 0))
  {
    encoder.position = INIT;
  }

  last_switch_A = switch_A;
  last_switch_B = switch_B;

  return encoder;
}

void update (int cylinder)
{
  switch (cylinder)
  {
  case FIRST_CYLINDER:
    first_cyl.position++;
    if (first_cyl.position == (720 / ENCODER_RESOLUTION))
    {
      first_cyl.position = 0;
      first_cyl.cycle_point = EXPANSION;
    }
    else if (first_cyl.position == (180 / ENCODER_RESOLUTION))
    {
      first_cyl.cycle_point == EXHAUST;
    }
    else if (first_cyl.position == (360 / ENCODER_RESOLUTION))
    {
      first_cyl.cycle_point == ADMISSION;
    }
    else if (first_cyl.position == (540 / ENCODER_RESOLUTION))
    {
      first_cyl.cycle_point == COMPRESSION;
    }
    else
    {
      /*DO NOTHING*/
    }
    break;
  case SECOND_CYLINDER:
    second_cyl.position++;
    if (second_cyl.position == (720 / ENCODER_RESOLUTION))
    {
      second_cyl.position = 0;
      second_cyl.cycle_point = EXPANSION;
    }
    else if (second_cyl.position == (180 / ENCODER_RESOLUTION))
    {
      second_cyl.cycle_point == EXHAUST;
    }
    else if (second_cyl.position == (360 / ENCODER_RESOLUTION))
    {
      second_cyl.cycle_point == ADMISSION;
    }
    else if (second_cyl.position == (540 / ENCODER_RESOLUTION))
    {
      second_cyl.cycle_point == COMPRESSION;
    }
    else
    {
      /*DO NOTHING*/
    }
    break;
  case THIRD_CYLINDER:
    third_cyl.position++;
    if (third_cyl.position == (720 / ENCODER_RESOLUTION))
    {
      third_cyl.position = 0;
      third_cyl.cycle_point = EXPANSION;
    }
    else if (third_cyl.position == (180 / ENCODER_RESOLUTION))
    {
      third_cyl.cycle_point == EXHAUST;
    }
    else if (third_cyl.position == (360 / ENCODER_RESOLUTION))
    {
      third_cyl.cycle_point == ADMISSION;
    }
    else if (third_cyl.position == (540 / ENCODER_RESOLUTION))
    {
      third_cyl.cycle_point == COMPRESSION;
    }
    else
    {
      /*DO NOTHING*/
    }
    break;
  
  default:
    break;
  }
}

//Firing Order: 1-3-2
void spark (int cylinder, int state)
{
  switch (cylinder)
  {
  case FIRST_CYLINDER:
    if (state == ON)
    {
      digitalWrite(RED_LED_1, TRUE);
    }
    else /*state == OFF*/
    {
      digitalWrite(RED_LED_1, FALSE);
    }
    break;

  case SECOND_CYLINDER:
    if (state == ON)
    {
      digitalWrite(RED_LED_2, TRUE);
    }
    else /*state == OFF*/
    {
      digitalWrite(RED_LED_2, FALSE);
    }    
    break;
  
  case THIRD_CYLINDER:
    if (state == ON)
    {
      digitalWrite(RED_LED_3, TRUE);
    }
    else /*state == OFF*/
    {
      digitalWrite(RED_LED_3, FALSE);
    }
    break;

  default:
    break;
  }
}

//Firing Order: 1-3-2
void inject (int cylinder, int state)
{
  switch (cylinder)
  {
  case FIRST_CYLINDER:
    if (state == ON)
    {
      digitalWrite(GREEN_LED_1, TRUE);
    }
    else /*state == OFF*/
    {
      digitalWrite(GREEN_LED_1, FALSE);
    }
    break;

  case SECOND_CYLINDER:
    if (state == ON)
    {
      digitalWrite(GREEN_LED_2, TRUE);
    }
    else /*state == OFF*/
    {
      digitalWrite(GREEN_LED_2, FALSE);
    }    
    break;
  
  case THIRD_CYLINDER:
    if (state == ON)
    {
      digitalWrite(GREEN_LED_3, TRUE);
    }
    else /*state == OFF*/
    {
      digitalWrite(GREEN_LED_3, FALSE);
    }
    break;

  default:
    break;
  }
}