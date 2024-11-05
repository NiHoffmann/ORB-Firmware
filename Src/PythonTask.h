//*******************************************************************
/*!
\author Nils Hoffmann
\date   12.10.2024
*/

#ifndef _PYTHON_TASK_H
#define _PYTHON_TASK_H

#include "lib.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

extern Memory_Flash  programMem;

class PythonTask : public Rtos::Task
{
  public:
    //---------------------------------------------------------------

    BYTE Parameter;
    // This is example 2 script, which will be compiled and executed.
    BYTE *programData;

  public:
    //---------------------------------------------------------------
    PythonTask(Rtos &rtosIn);

    //---------------------------------------------------------------
    virtual ~PythonTask();

    //---------------------------------------------------------------
    virtual void update( );

//    //---------------------------------------------------------------
//    virtual void OnStop(void);
//
//    //---------------------------------------------------------------
//    virtual void OnStart(void);

    //---------------------------------------------------------------
    void Start( BYTE para );

    void Stop();

    void userInterrupt();

    bool isRunning();

    bool expectionReady();

    void writeException();

    void clearException();

    bool isStarting();

};

uint8_t* loadProgram(int length);

uint32_t loadProgramLength();
#endif



