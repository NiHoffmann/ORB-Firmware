//*******************************************************************
/*!
\file   Motor.cpp
\author Thomas Breuer (Bonn-Rhein-Sieg University of Applied Sciences)
\date   03.02.2021
*/

//*******************************************************************
#include "lib.h"
#include <math.h>
#include "Motor.h"
#include "AppTask.h"

//*******************************************************************
static const float a0     = 0.05f;
static const float b0     = (1.0f - a0);
static const float maxPwr = 1000.0f;

//*******************************************************************
//
// Motor
//
//*******************************************************************
//-------------------------------------------------------------------
Motor::Motor( cDevMotor  &motorIn,
              cHwEncoder &encIn )

  : motor( motorIn  ),
    enc  ( encIn    )

   {
     power       = 0;
     measuredPos = 0;
     enc.get();
   }

void printt(BYTE line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    AppTask::setMonitorText(nullptr, line, format, args);

    va_end(args);
}



//-------------------------------------------------------------------
void Motor::update( void )
{
  int tics = enc.get();

  settingBuffer >> setting;
  configBuffer  >> config;

  measuredPos   += tics;
  measuredSpeed  = config.ticsToRot*movingAverage.get( tics );

  switch( setting.mode )
  {
    default:
      posSpeed = 0;
      power    = 0;
      motor.run( power );
      break;

    case FREE:
      posSpeed = 0;
      power    = RANGE( setting.speed, -maxPwr, +maxPwr );
      motor.run( power );
      break;

    case BRAKE:
      posSpeed = 0;
      power    = 0;
      motor.brake();
      break;

    case SPEED:
      posSpeed = 0;
      speedControl( setting.speed );
      motor.run( power );
      break;

    case POSITION:
      positionControl( setting.speed, setting.pos );
      speedControl( posSpeed );
      motor.run( power );
      break;
  }

  report.pwr   = a0*power         + b0*report.pwr;
  report.speed = a0*1000.0*measuredSpeed + b0*report.speed;
  report.pos   = config.ticsToRot*(float)measuredPos;

  reportBuffer << report;
}

//-------------------------------------------------------------------
void Motor::speedControl( float speed )
{
  float delta = speed - measuredSpeed;

  power +=   config.Regler_Ki * delta + config.Regler_Kp * (delta - deltaLast);
  power  = RANGE( power, -maxPwr, +maxPwr );

  deltaLast = delta;
}

//-------------------------------------------------------------------
void Motor::positionControl( float speedLimit, int targetPos )
{
  float targetSpeed = 0;
  float distance    = targetPos - config.ticsToRot*(float)measuredPos;
  float accLocal    = config.acc*speedLimit;

  //Getting speed out of distance to drive
  float v = sqrtf( 0.01*200.0f * fabs(distance) * accLocal );

  targetSpeed = SIGN(distance) * MIN( v, (float)fabs(speedLimit) );

  //Accelerate if targetSpeed not arived
  if( posSpeed + accLocal < targetSpeed )
  {
    posSpeed += accLocal;
  }
  else if( posSpeed - accLocal > targetSpeed )
  {
    posSpeed -= accLocal;
  }
  else
  {
    posSpeed = targetSpeed;
  }
}
