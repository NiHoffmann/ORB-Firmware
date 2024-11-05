//*******************************************************************
/*!
\file   Remote.cpp
\author Thomas Breuer (Bonn-Rhein-Sieg University of Applied Sciences)
\date   03.02.2021
*/

//*******************************************************************
#include "lib.h"

//*******************************************************************
#include "Remote.h"
#include "Bluetooth.h"

//*******************************************************************
extern Memory_Flash  mem0;
extern Memory_Flash  mem1;
//extern Memory_Flash  mem3_1;
//extern Memory_RAM    mem3_2;

extern Bluetooth &btm;

extern SensorTask  sTask    [ NUM_OF_SENSOR_PORTS ];
extern MotorTask   mTask    [ NUM_OF_MOTOR_PORTS  ];
extern ServoTask   servoTask[ NUM_OF_SERVO_PORTS  ];

extern Digital taster1;
extern Digital taster2;

extern PythonTask pythonTask;


//*******************************************************************
//
// Remote
//
//*******************************************************************
//-------------------------------------------------------------------
Remote::Remote( cISC           &ipc,
                UserInterface  &ui,
                Control        &ctrl,
                Settings       &settings,
                Monitor        &monitor )

: ipc     ( ipc      )
 ,ui      ( ui       )
 ,ctrl    ( ctrl     )
 ,settings( settings )
 ,monitor ( monitor  )

 ,configIn   ( ipc )
 ,propIn     ( ipc )
 ,propOut    ( ipc )
 ,monitorIn  ( ipc )
 ,monitorOut ( ipc )
 ,settingsOut( ipc )
 ,settingsIn ( ipc )

 ,download   ( ipc )
 ,interface0 ( download, mem0,   ui )
// ,interface1 ( download, mem3_1, ui )
// ,interface2 ( download, mem3_2, ui  )

{
  configReceivedFlag = false;
  acceptFlag = true;
}

//---------------------------------------------------------------
void Remote::update()
{
  bool send = false;

  ipc.update();

  if(pythonTask.expectionReady()){
    pythonTask.writeException();
  }

  //-------------------------------------------------------------
  if( configIn.isNew() )
  {
    if( !ui.isAppActive())
    {
      configReceivedFlag = true;

      // set sensor configuration
      for( int i=0; i<NUM_OF_SENSOR_PORTS; i++ )
      {
        sTask[i].config( configIn.data.sensorConfig[i].type,
                         configIn.data.sensorConfig[i].mode,
                         configIn.data.sensorConfig[i].option );
      }

      // set motor configuration
      for( int i=0; i<NUM_OF_MOTOR_PORTS; i++ )
      {
        mTask[i].config( configIn.data.motorConfig[i].ticsPerRotation,
                         configIn.data.motorConfig[i].acceleration,
                         configIn.data.motorConfig[i].Regler_Kp,
                         configIn.data.motorConfig[i].Regler_Ki );
      }
    }
  }
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  if( propIn.isNew()  )
  {
    if( !ui.isAppActive() )
    {
      // set motor
      for( int i=0; i<NUM_OF_MOTOR_PORTS; i++ )
      {
        mTask[i].set( propIn.data.motor[i].mmode,
                      propIn.data.motor[i].speed,
                      propIn.data.motor[i].pos );
      }

      // set servo
      for( int i=0; i<NUM_OF_SERVO_PORTS; i++ )
      {
        servoTask[i].set( propIn.data.modelServo[i].sspeed,
                          propIn.data.modelServo[i].spos );
      }

      ctrl.trigger(); //inform ctrl, that propIn was received
    }

    send = true;
  }
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  if( settingsIn.isNew() )
  {
    if( acceptFlag && settingsIn.data.command & 0x01  )
    {
      settings.store( settingsIn.data );
      btm.config( settingsIn.data.name );
    }

    if( acceptFlag && settingsIn.data.command & 0x02  )
    {
      mem1.erase();
    }

    acceptFlag = (settingsIn.data.command == 0 )?true:false;

    settings.get(settingsOut.data);
    settingsOut.write();
  }
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  if( monitorIn.isNew() )
  {
    ui.setMonitorEvent( monitorIn.data.keycode );

    switch( monitorIn.data.bmode )
    {
      case 1: ui.startApp( monitorIn.data.parameter );  break;
      case 2: ui.stopApp();                             break;
    }

    monitor.getNextLine( monitorOut.data );
    monitorOut.write();

    send = true;
  }
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  if( send )
  {
    cPropFromORB::Data  &d = propOut.data;

    // read data from sensor
    for( int i=0; i<NUM_OF_SENSOR_PORTS; i++ )
    {
      BYTE lenExp = (d.sensor[i].descriptor>>5)&0x07;
      BYTE package = (d.sensor[i].descriptor & 0x1F)+1;

      if( lenExp > 2 && package < (1<<(lenExp-2)) ) //mehr als 2^2=4 Byte und nicht letztes
      {
        // TODO: mehr als 8 Byte geht noch nicht, daher letztes Byte mehrfach
        d.sensor[i].value      = report[i].value[1];
        d.sensor[i].descriptor = (lenExp<<5)|package; // package #1 of 2
      }
      else
      {
        sTask[i].get( report[i] );

        d.sensor[i].value      = report[i].value[0];
        d.sensor[i].descriptor = report[i].lenExp<<5;
        d.sensor[i].type       = report[i].type;
        d.sensor[i].option     = report[i].option;
      }
    }

    d.digital  =  (taster1.get() ? 0x01 : 0)
                 |(taster2.get() ? 0x02 : 0);

    // read data from motor
    for( int i=0; i<NUM_OF_MOTOR_PORTS; i++)
    {
      ORB::Motor  report;
      mTask[i].get( report );
      d.motor[i].pwr   = RANGE((int)(0.10005f*report.pwr),-100,+100);
      d.motor[i].speed = 1.0005f*report.speed;
      d.motor[i].pos   =         report.pos;
    }

    // read other data
    d.Vcc = ctrl.getVcc();

    //TODO make sure the 0-Bit for this flag is set otherwise the orb-monitor wont update
    d.status =   (( ui.isAppActive() || pythonTask.expectionReady()  ? 1 : 0 ) << 0 )
               | (( configReceivedFlag ? 1 : 0 ) << 1 );

    configReceivedFlag = false;

    // write sensor and motor data
    propOut.write();
  }
}
//EOF
