//*******************************************************************
/*!
\file   ServoTask.cpp
\author Thomas Breuer (Bonn-Rhein-Sieg University of Applied Sciences)
\date   03.02.2021
*/

//*******************************************************************
#include "lib.h"
#include "ServoTask.h"

//*******************************************************************
//
// ServoTask
//
//*******************************************************************
//-------------------------------------------------------------------
ServoTask::ServoTask( TaskManager  &taskHandler,
                      cDevAnalogOutPWM   &pwmIn )

: //TaskManager::Task( taskHandler ),
  pwm               ( pwmIn       ),
  timer             ( taskHandler, 25/*ms*/ )

{
  valid    = false;
  running  = false;
  posDelta = 0;
  posSoll  = 0;
  pos      = 0;
  pwm.set( 0 );
  valid = true;

  taskHandler.add( this );
}

//-------------------------------------------------------------------
void ServoTask::set( BYTE speedIn, BYTE posIn )
{
  // 1 step = 0.01;  T=25ms; 10steps/s => Faktor = 0.01*0.025s*10/s = 0.0025
  valid    = false;
  posDelta = (speedIn < 100)?(float)speedIn*0.0025:1.0;
  posSoll  = (float)posIn*0.01 + 1.0;
  valid    = true;
}

//-------------------------------------------------------------------
void ServoTask::ServoTask::update( void )
{
  if( valid && timer.timeout() )
  {
    if( posDelta != 0 ) // Servo on
    {
      if( !running )    // Servo war aus und wird jetzt eingeschaltet
      {
        pos = posSoll;  // Aktuelle Pos. unbekannt, daher Soll-Pos. �bernehmen
        pwm.set( pos );
        running = true;
      }
      else if( pos < posSoll ) // Servo auf neue Position
      {
        pos = MIN( pos + posDelta, posSoll );
        pwm.set( pos );
      }
      else if( pos > posSoll ) // Servo auf neue Position
      {
        pos = MAX( pos - posDelta, posSoll );
        pwm.set( pos );
      }
    }
    else            // Servo off
    {
      if( running ) // Servo war an und wird jetzt ausgeschaltet
      {
        pwm.set( 0 );
        running = false;
      }
    }
  }
}

//EOF
