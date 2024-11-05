//*******************************************************************
/*!
\file   AppTask.cpp
\author Thomas Breuer (Bonn-Rhein-Sieg University of Applied Sciences)
\date   03.02.2021
*/

//*******************************************************************
#include "lib.h"

//*******************************************************************
#include "AppTask.h"
#include "mpconfigport.h"

//*******************************************************************
extern SensorTask   sTask[ NUM_OF_SENSOR_PORTS ];
extern MotorTask    mTask[ NUM_OF_MOTOR_PORTS ];
extern ServoTask    servoTask[ NUM_OF_SERVO_PORTS ];
extern Digital  taster1;
extern Digital  taster2;
extern Monitor      monitor;

//*******************************************************************
//
// AppTask
//
//*******************************************************************
//-------------------------------------------------------------------
cORBlocal  AppTask::orb;
BYTE       AppTask::MonitorEvent = 0;

//-------------------------------------------------------------------
AppTask::AppTask( Rtos &rtosIn)

: Rtos::Task( rtosIn, 10000/* stack size*/ )

{
  monitor.clear();
}

//-------------------------------------------------------------------
AppTask::~AppTask()
{
}

//-------------------------------------------------------------------
void AppTask::update( )
{
  orb.funcPtr_configMotor       = configMotor;
  orb.funcPtr_setMotor          = setMotor;
  orb.funcPtr_getMotor          = getMotor;
  orb.funcPtr_setModelServo     = setModelServo;
  orb.funcPtr_configSensor      = configSensor;
  orb.funcPtr_getSensor         = getSensor;
  orb.funcPtr_getSensorValueExt = getSensorValueExt;
  orb.funcPtr_getSensorDigital  = getSensorDigital;
  orb.funcPtr_setMonitorText    = setMonitorText;
  orb.funcPtr_getMonitorKey     = getMonitorKey;
  orb.funcPtr_getTime           = getTime;
  orb.funcPtr_wait              = wait;
  orb.funcPtr_clearMemory       = clearMemory;
  orb.funcPtr_setMemory         = setMemory;
  orb.funcPtr_getMemory         = getMemory;

  orb.ptr  = this;

//*******************************************************************
#if (BOARD_MAIN == 00 && BOARD_SUB == 22)

 unsigned *addr = (unsigned*)0x80E0004;

#elif (BOARD_MAIN == 00 && BOARD_SUB == 30)

 unsigned *addr = (unsigned*)0x80E0004;

#elif (BOARD_MAIN == 01 && BOARD_SUB == 00)

 unsigned *addr = (unsigned*)0x80E0004;

#else

  #error "Board hardware version not defined"

#endif

  int (*func)(BYTE para, cORBlocal &ptr) =  (int (*)(BYTE, cORBlocal &))(*addr);

  // TODO:
  // - Memory-Size übergeben
  //oder
  // Interpreter ermittelt/kennt selbst die Adressen/Size
  //
//  if( Parameter == 0 )
//    orb.prog = (unsigned *)mem3_1.getAddr();
//  else
//    orb.prog = (unsigned *)mem3_2.getAddr();

  func( Parameter, orb );
 /// todo  Finish();
 stop();
}

////-------------------------------------------------------------------
//void AppTask::OnStop(void)
//{
//  for( int i = 0; i < NUM_OF_MOTOR_PORTS; i++ )
//  {
//    mTask[i].set( 0, 0, 0 );
//  }
//  monitor.clear();
//}
//
////-------------------------------------------------------------------
//void AppTask::OnStart(void)
//{
//}

//-------------------------------------------------------------------
void AppTask::Start( BYTE para )
{
  Parameter = para;
  start();
}


//-------------------------------------------------------------------
//  static methods
//-------------------------------------------------------------------
void AppTask::configMotor(void * ptr, BYTE id, WORD t, BYTE a, BYTE Kp, BYTE Ki)
{
  if( id < NUM_OF_MOTOR_PORTS)
    mTask[id].config( t,a,Kp,Ki );
}

//-------------------------------------------------------------------
void AppTask::setMotor(void * ptr, BYTE id, BYTE mode, short speed, int pos)
{
  if( id < NUM_OF_MOTOR_PORTS)
    mTask[id].set( mode, speed, pos );
}

//-------------------------------------------------------------------
ORB::Motor AppTask::getMotor(void *ptr, BYTE id)
{
  ORB::Motor motorReport;
  if( id < NUM_OF_MOTOR_PORTS )
  {
    mTask[id].get( motorReport );
  }
  return(motorReport);
}

//-------------------------------------------------------------------
void AppTask::setModelServo(void *ptr, BYTE id,BYTE speed, BYTE winkel)
{
  if( id < NUM_OF_SERVO_PORTS )
  {
    servoTask[id].set( speed, winkel );
  }
}

//-------------------------------------------------------------------
void AppTask::configSensor(void * ptr, BYTE id, BYTE type, BYTE mode, WORD option )
{
  if( id < NUM_OF_SENSOR_PORTS)
    sTask[id].config( type, mode, option );
}

//-------------------------------------------------------------------
ORB::Sensor AppTask::getSensor(void *ptr, BYTE id)
{
  ORB::Sensor  sensorReport;
  if( id < NUM_OF_SENSOR_PORTS )
  {
    sTask[id].get( sensorReport );
  }
  return(sensorReport);
}

//-------------------------------------------------------------------
WORD AppTask::getSensorValueExt(void *ptr, BYTE id, BYTE ch)
{
  if( id < NUM_OF_SENSOR_PORTS )
  {
    switch( ch )
    {
      case 0: return( sTask[id].sensor->context.adc1.getRaw()>>4 ); break;
      case 1: return( sTask[id].sensor->context.adc2.getRaw()>>4  ); break;
      case 2: return( sTask[id].sensor->context.digitalA.get() ); break;
      case 3: return( sTask[id].sensor->context.digitalB.get() ); break;
    }
  }
  return(0);
}

//-------------------------------------------------------------------
BYTE AppTask::getSensorDigital(void *ptr, BYTE id)
{
  switch(id )
  {
    case 0: return( taster1.get() ); break;
    case 1: return( taster2.get() ); break;
    default: return(0); break;
  }
}

//-------------------------------------------------------------------
void AppTask::setMonitorText(void *ptr, BYTE line,const char *format, va_list va)
{
  monitor.printf(line, format,va);
}

//-------------------------------------------------------------------
int AppTask::getMonitorKey(void *ptr )
{
  return( MonitorEvent );
}

//-------------------------------------------------------------------
DWORD AppTask::getTime(void *ptr)
{
  return( Rtos_Mcu::getSysTime() );
}

//-------------------------------------------------------------------
void AppTask::wait(void *ptr, DWORD time)
{
  AppTask *t = (AppTask *)ptr;
  DWORD timeToGo = Rtos_Mcu::getSysTime()+time;
  do
  {
    t->pause();
  }  while( Rtos_Mcu::getSysTime() < timeToGo);
}

//-------------------------------------------------------------------
void AppTask::clearMemory(void *ptr)
{
  mem1.erase();
}

//-------------------------------------------------------------------
void AppTask::setMemory(void *ptr, DWORD addr, BYTE *data, DWORD size )
{
  mem1.unlock();
  for( DWORD i=0;i<size;i++)
    mem1.write(addr++,data[i]);
  mem1.lock();
}

//-------------------------------------------------------------------
void AppTask::getMemory(void *ptr, DWORD addr, BYTE *data, DWORD size )
{
  for( DWORD i=0;i<size;i++)
    data[i] = mem1.read(addr++);
}
//---------------------------------------------------------------
//---------------------------------------------------------------

//EOF
