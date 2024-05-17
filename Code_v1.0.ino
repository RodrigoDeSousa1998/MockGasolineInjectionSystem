//TODO*: Implement timers for angular speed calculations                         | (DONE)
//TODO*: Implement potentiometer for load detection (throttle valve opening)     | (DONE)
//TODO*: Implement lookup tables for ignition timing and injection timing        | (DONE)
//TODO : Implement temperature correction for the timing calculations            | (DONE~)
//TODO : Implement pushbuttons for push to start and gear change                 | (DONE~)
//TODO!: Implement motor control with pwm having load as an input                |
//TODO!: Implement buzzer for rpm limit warning and starting sound               |
//TODO!: Implement output with rpm and gear                                      | (NOT ENOUGH PINS)

//Libraries
#include "DHT.h"

//Macro Definitions
#define FULL_TURN           30U
#define INIT                0U
#define INIT_F              0.00F
#define OFF                 0U
#define ON                  1U
#define UNPRESSED           0U
#define PRESSED             1U
#define FALSE               0U
#define TRUE                1U
#define POT_MIN             12.00F   
#define POT_MAX             1023.00F
#define SCALE_MAX           100.00F
#define SCALE_MAX_8Bit      255U
#define ENCODER_RESOLUTION  12U
#define ADMISSION           0U
#define COMPRESSION         1U
#define EXPANSION           2U
#define EXHAUST             3U
#define FIRST_CYLINDER      0U
#define SECOND_CYLINDER     1U
#define THIRD_CYLINDER      2U
#define NUMBER_OF_CYLINDERS 3U
#define ONE_SECOND          1000U
#define AMBIENT_TEMPERATURE 25U

//Macro Pin Definitions
#define TEMP_SENSOR      0U
#define ENCODER_CRK_CLK  1U
#define ENCODER_CRK_DT   2U
#define BUZZER           3U
#define MOTOR_CONTROLLER 5U
#define BLUE_LED         6U
#define BUTTON           7U
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
void pushToStart(void);

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

static cylinder first_cylinder  = {FIRST_CYLINDER , EXPANSION  , 0  , update}; /*Cycle Positions at beginning determine firing order*/
static cylinder second_cylinder = {SECOND_CYLINDER, EXHAUST    , 240, update}; /*Firing Order: 1-3-2*/
static cylinder third_cylinder  = {THIRD_CYLINDER , ADMISSION  , 480, update};

DHT dht (TEMP_SENSOR, DHT11); /*Pin and sensor id for temperature reading*/

static unsigned int current_state = OFF; /*Activation and deactivation control variable*/

//Main Functions
void setup() 
{
  // delay(ONE_SECOND); /*To ensure temperature sensor has passed the unstable status*/
  // dht.begin();

  Serial.begin (9600);

  pinMode (BUZZER           , OUTPUT);
  pinMode (MOTOR_CONTROLLER , OUTPUT);
  pinMode (BLUE_LED         , OUTPUT);
  pinMode (RED_LED_1        , OUTPUT);
  pinMode (RED_LED_2        , OUTPUT);
  pinMode (RED_LED_3        , OUTPUT);
  pinMode (GREEN_LED_1      , OUTPUT);
  pinMode (GREEN_LED_2      , OUTPUT);
  pinMode (GREEN_LED_3      , OUTPUT);

  pinMode (ENCODER_CRK_DT  , INPUT);
  pinMode (ENCODER_CRK_CLK , INPUT);

  //Interrupt Pin
  pinMode (BUTTON , INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), pushToStart, FALLING);

  tone(BUZZER, 1000, 300);
  delay(700);
  tone(BUZZER, 1000, 300);
  
  // delay(ONE_SECOND); /*To ensure max frequency reading of temperature sensor is abided*/
}

void loop() 
{  
  static int spark_advance      = INIT;
  static int start_of_injection = INIT;
  static int fuel_mass          = INIT;

  static unsigned int torque_request = INIT;

  // static float temperature = INIT_F; 
  // static unsigned long int temp_read_init  = INIT;
  // static unsigned long int temp_read_final = INIT;
  // static int first_run = TRUE;

  // temp_read_final = millis();
  // if (((temp_read_final - temp_read_init) >= ONE_SECOND) || (first_run == TRUE))
  // {
  //   temperature = dht.readTemperature();
  //   temp_read_init = millis();
  //   first_run = FALSE;
  // }

  // Serial.println(temperature);
  
  if (current_state == ON)
  {
    digitalWrite(BLUE_LED, TRUE);
    
    unsigned long int init_time = millis();
    shaft crankshaft = {FALSE, INIT};
    do
    {
      crankshaft = encoderReader(ENCODER_CRK_CLK, ENCODER_CRK_DT);

      for (int i = 0; i < NUMBER_OF_CYLINDERS; i++)
      {
        if (i == FIRST_CYLINDER)
        {
          if (first_cylinder.cycle_point == ADMISSION || first_cylinder.cycle_point == COMPRESSION)
          {
            int cylinder_position = (first_cylinder.position - 360) / ENCODER_RESOLUTION;

            static int spark_start = INIT; 
            int ingnition_point = (360 - spark_advance) / ENCODER_RESOLUTION;
            if (cylinder_position == ingnition_point)
            {
              Serial.println("FIRST IGN ON");
              spark_start = cylinder_position;
              spark(FIRST_CYLINDER, ON);
            }

            static int injection_start = INIT;
            int injection_point = (360 - start_of_injection) / ENCODER_RESOLUTION;
            if (cylinder_position  == injection_point)
            {
              Serial.println("FIRST INJ ON");
              injection_start = cylinder_position;
              inject(FIRST_CYLINDER, ON);
            }

            if (spark_start != INIT)
            {
              spark(FIRST_CYLINDER, OFF);
            }

            if ((injection_start != INIT) && ((cylinder_position - injection_start) > (fuel_mass / ENCODER_RESOLUTION)))
            {
              inject(FIRST_CYLINDER, OFF);
            }
          }
        }
        else if (i == SECOND_CYLINDER)
        {
          if (second_cylinder.cycle_point == ADMISSION || second_cylinder.cycle_point == COMPRESSION)
          {
            int cylinder_position = (second_cylinder.position - 360) / ENCODER_RESOLUTION;

            static int spark_start = INIT; 
            int ingnition_point = (360 - spark_advance) / ENCODER_RESOLUTION;
            if (cylinder_position == ingnition_point)
            {
              Serial.println("SECOND IGN ON");
              spark_start = cylinder_position;
              spark(SECOND_CYLINDER, ON);
            }

            static int injection_start = INIT;
            int injection_point = (360 - start_of_injection) / ENCODER_RESOLUTION;
            if (cylinder_position == injection_point)
            {
              Serial.println("SECOND INJ ON");
              injection_start = cylinder_position;
              inject(SECOND_CYLINDER, ON);
            }

            if (spark_start != INIT)
            {
              spark(SECOND_CYLINDER, OFF);
            }

            if ((injection_start != INIT) && ((cylinder_position - injection_start) > (fuel_mass / ENCODER_RESOLUTION)))
            {
              inject(SECOND_CYLINDER, OFF);
            }
          }
        }
        else if (i == THIRD_CYLINDER)
        {
          if (third_cylinder.cycle_point == ADMISSION  || third_cylinder.cycle_point == COMPRESSION)
          {
            int cylinder_position = (third_cylinder.position - 360) / ENCODER_RESOLUTION;
            
            static int spark_start = INIT; 
            int ingnition_point = (360 - spark_advance) / ENCODER_RESOLUTION;
            if (cylinder_position == ingnition_point)
            {
              Serial.println("THIRD IGN ON");
              spark_start = cylinder_position;
              spark(THIRD_CYLINDER, ON);
            }

            static int injection_start = INIT;
            int injection_point = (360 - start_of_injection) / ENCODER_RESOLUTION;
            if (cylinder_position == injection_point)
            {
              Serial.println("THIRD INJ ON");
              injection_start = cylinder_position;
              inject(THIRD_CYLINDER, ON);
            }

            if (spark_start != INIT)
            {
              spark(THIRD_CYLINDER, OFF);
            }

            if ((injection_start != INIT) && ((cylinder_position - injection_start) > (fuel_mass / ENCODER_RESOLUTION)))
            {
              inject(THIRD_CYLINDER, OFF);
            }
          }
        }
        else
        {
          /*DO NOTHING*/
        }

        if (current_state == OFF)
        {
          break;
        }
      }
    } while (crankshaft.full_turn != TRUE);
    unsigned long int final_time = millis();

                                            /*Conversion Factor from milliseconds to minutes*/
    static float elapsed_time = (float)(final_time - init_time) * (0.001/60.000);
    static unsigned int rpm = 1/elapsed_time;

    int pot_val = analogRead(POTENTIOMETER);
                            /*Conversion Factor Calculation*/
    int load = SCALE_MAX * (float)(pot_val/POT_MAX);

    torque_request = SCALE_MAX_8Bit * (float)(pot_val/POT_MAX);
    analogWrite(MOTOR_CONTROLLER, torque_request);

    int load_idx = load/10;
    int rpm_idx = (rpm/10) - 1;

    // int temp_correction = temperature - AMBIENT_TEMPERATURE;
    // //To avoid not multiples of encoder resolution
    // if (temp_correction > 1)
    // {
    //   temp_correction = 1;
    // } 
    // else if (temp_correction < -1)
    // {
    //   temp_correction = -1;
    // }
    // else
    // {
    //   /*DO NOTHING*/
    // }

    // spark_advance      = ignition_map [load_idx][rpm_idx] - (ENCODER_RESOLUTION * temp_correction);
    spark_advance      = ignition_map [load_idx][rpm_idx]; 
    start_of_injection = injection_map[load_idx][rpm_idx];
    fuel_mass          = fuel_map     [load_idx][rpm_idx];
  }
  else /*(current_state == OFF)*/
  {
    static unsigned int blink = OFF;

    blink = (blink == OFF) ? ON : OFF;

    delay(500);
    
    digitalWrite(BLUE_LED, blink);
    
    spark_advance      = INIT;
    start_of_injection = INIT;
    fuel_mass          = INIT;

    first_cylinder.position = 0;
    first_cylinder.cycle_point = EXPANSION;

    second_cylinder.position = 240;
    second_cylinder.cycle_point = EXHAUST;

    third_cylinder.position = 480;
    third_cylinder.cycle_point = ADMISSION;
  }
}

//Auxiliary Functions Implementations

void pushToStart(void) /*Interrupt Function*/
{
  current_state = (current_state == OFF) ? ON : OFF;

  Serial.println(current_state);

  tone(BUZZER, 700, 300);
  tone(BUZZER, 1000, 300);
}

shaft encoderReader(int pinCLK, int pinDT)
{
  static int last_switch_A = INIT;
  static int last_switch_B = INIT;
  static shaft encoder = {FALSE, INIT};
  static int flip = INIT;
 
  int switch_A = digitalRead(pinCLK);
  int switch_B = digitalRead(pinDT);

  if (switch_A != last_switch_A)
  {    
    if ((switch_A > switch_B) || flip) 
    {
      encoder.position++;
      
      first_cylinder.update_data(FIRST_CYLINDER);
      second_cylinder.update_data(SECOND_CYLINDER);
      third_cylinder.update_data(THIRD_CYLINDER);
      
      flip = ~flip;
    }  
  }

  if (encoder.position == FULL_TURN)
  {
    encoder.full_turn = TRUE;
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
    first_cylinder.position += ENCODER_RESOLUTION;
    if (first_cylinder.position == 720)
    {
      first_cylinder.position = 0;
      first_cylinder.cycle_point = EXPANSION;
    }
    else if (first_cylinder.position == 180)
    {
      first_cylinder.cycle_point = EXHAUST;
    }
    else if (first_cylinder.position == 360)
    {
      first_cylinder.cycle_point = ADMISSION;
    }
    else if (first_cylinder.position == 540)
    {
      first_cylinder.cycle_point = COMPRESSION;
    }
    else
    {
      /*DO NOTHING*/
    }
    break;
  case SECOND_CYLINDER:
    second_cylinder.position += ENCODER_RESOLUTION;
    if (second_cylinder.position == 720)
    {
      second_cylinder.position = 0;
      second_cylinder.cycle_point = EXPANSION;
    }
    else if (second_cylinder.position == 180)
    {
      second_cylinder.cycle_point = EXHAUST;
    }
    else if (second_cylinder.position == 360)
    {
      second_cylinder.cycle_point = ADMISSION;
    }
    else if (second_cylinder.position == 540)
    {
      second_cylinder.cycle_point = COMPRESSION;
    }
    else
    {
      /*DO NOTHING*/
    }
    break;
  case THIRD_CYLINDER:
    third_cylinder.position += ENCODER_RESOLUTION;
    if (third_cylinder.position == 720)
    {
      third_cylinder.position = 0;
      third_cylinder.cycle_point = EXPANSION;
    }
    else if (third_cylinder.position == 180)
    {
      third_cylinder.cycle_point = EXHAUST;
    }
    else if (third_cylinder.position == 360)
    {
      third_cylinder.cycle_point = ADMISSION;
    }
    else if (third_cylinder.position == 540)
    {
      third_cylinder.cycle_point = COMPRESSION;
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