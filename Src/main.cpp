//*******************************************************************
/*!
\file   main.cpp
\author Thomas Breuer (Bonn-Rhein-Sieg University of Applied Sciences)
\date   03.02.2021
*/
#define PYTHON_TASK         (1)
#define APP_TASK            (1)
#define SENSOR_TASKS        (NUM_OF_SENSOR_PORTS)
#define MAIN_TASK           (1)
#define MICRO_SECONDS_PER_TASK_EXECUTION (200)

//*******************************************************************
#include "PythonTask.h"
#include "lib.h"
#include "Module/Rtos/Rtos.h"
#include "Module/ISC.h"
#include "Module/USB/USBdeviceSimpleIO.h"
#include "Module/Download.h"
#include "config.h"

#include "Properties.h"
#include "MotorTask.h"
#include "ServoTask.h"
#include "SensorTask.h"
#include "Settings.h"
#include "Monitor.h"
#include "UserInterface.h"
#include "Remote.h"

//*******************************************************************
//Adding one task (PythonTask)
Rtos rtos( SENSOR_TASKS + PYTHON_TASK + APP_TASK + MAIN_TASK, MICRO_SECONDS_PER_TASK_EXECUTION );

Sensor::Context sContext[ NUM_OF_SENSOR_PORTS ]
  = { Sensor::Context( adc1, adc21, digitalA_1, digitalB_1, digitalC_1, digitalD_1, digitalH_1 ),
      Sensor::Context( adc2, adc22, digitalA_2, digitalB_2, digitalC_2, digitalD_2, digitalH_2 ),
      Sensor::Context( adc3, adc23, digitalA_3, digitalB_3, digitalC_3, digitalD_3, digitalH_3 ),
      Sensor::Context( adc4, adc24, digitalA_4, digitalB_4, digitalC_4, digitalD_4, digitalH_4 ) };

SensorTask sTask[ NUM_OF_SENSOR_PORTS ]
  = { SensorTask( rtos, sContext[0], uart1, tim10, extInt1 ),
      SensorTask( rtos, sContext[1], uart2, tim10, extInt2 ),
      SensorTask( rtos, sContext[2], uart3, tim10, extInt3 ),
      SensorTask( rtos, sContext[3], uart4, tim10, extInt4 ) };

MotorTask mTask[ NUM_OF_MOTOR_PORTS ]
  = { MotorTask( taskManager, mot1, enc_A ),
      MotorTask( taskManager, mot2, enc_B ),
      MotorTask( taskManager, mot3, enc_C ),
      MotorTask( taskManager, mot4, enc_D ) };

ServoTask servoTask[ NUM_OF_SERVO_PORTS ]
  = { ServoTask( taskManager, pwm_ModellServo1 ),
      ServoTask( taskManager, pwm_ModellServo2 ) };

Monitor   monitor;

Settings  settings( mem2 );

//*******************************************************************
cISC_UART        ipcBT ( bt );
cISC_USBdevice   ipcUSB( usb, 0/*interfId*/);

//*******************************************************************
AppTask        app      ( rtos                                     );
PythonTask     pythonTask ( rtos                                   );
UserInterface  ui       ( app, pythonTask,      settings           );
Control        ctrl     ( taskManager, ui                          );
Remote         remoteBT ( ipcBT,       ui, ctrl, settings, monitor );
Remote         remoteUSB( ipcUSB,      ui, ctrl, settings, monitor );

//*******************************************************************

int main( void )
{

usb.start();
System::delayMilliSec(100);

while(1)
  {
    ui.       update();
    ctrl.     update();
    remoteBT. update();
    remoteUSB.update();
  }
}
