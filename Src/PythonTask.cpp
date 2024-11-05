//*******************************************************************
/*!
\author Nils Hoffmann
\date   12.10.2024
*/

//we have to include everything lib related first because of naming conflicts
#include "lib.h"
#include "Remote.h"

#include "PythonTask.h"
#include "mpconfigport.h"
extern "C"{
    #include "python-vm.h"
}
#ifdef ORB_ENABLE_MONITOR_STD_OUT
    #include "Monitor_C_Interface.h"
#endif

extern DigitalIndicator      indUsr2;
PythonVM vm;
//TODO move this over to mem0 object
Memory_Flash  programMem(11);
bool exceptionReady = false;
bool starting = false;
int start_parameter = 0;

PythonTask::PythonTask( Rtos &rtosIn)

: Rtos::Task( rtosIn, 1024 * STACK_SIZE_KB )
{}

PythonTask::~PythonTask(){

}

void PythonTask::update( ){
    exceptionReady = false;
    vm.run(loadProgramLength, loadProgram, start_parameter);
    starting = false;

    if(vm.getExitStatus() !=  vm.Status::NORMAL){
        exceptionReady = true;
    }

    stop();
}

void PythonTask::Start( BYTE para ){
    starting = true;
    start_parameter = para;
    start();
}

void PythonTask::Stop(){
    stop();
}

//This should only be called from outside the python task
void PythonTask::userInterrupt(){
    vm.stopProgram();
    //wait for vm to exit and deinitialize
    while(vm.isRunning()){}
}

bool PythonTask::expectionReady(){
    return exceptionReady;
}

void PythonTask::writeException(){
    const char *exit_message = vm.getExitInfo();
    size_t pointer = 0;
    size_t len = 31;
    uint8_t line = 0;

    do {
        const char* message_left = exit_message + pointer;

        len = 31;
        if( strlen(message_left) < len){
            len = strlen(message_left);
        }

        setMonitorText(line, message_left, len);
        pointer += 31;
        line += 1;
    }while(len == 31 && line < 3);
}

void PythonTask::clearException(){
    exceptionReady = false;
    //clear the exception message
    setMonitorText(0," ",1);
}

bool PythonTask::isRunning() {
    return vm.isRunning();
}

bool PythonTask::isStarting(){
    return starting;
}

uint8_t* loadProgram(int length) {
    uint8_t* programData = (uint8_t*)malloc(length * sizeof(uint8_t));
    if (programData == nullptr) {
        return nullptr;
    }

    for( DWORD i=0;i<length;i++) {
        programData[i] = programMem.read(PROGRAM_LENGTH_BYTE + LANGUAGE_FLAG_BYTE + i);
    }
    return programData;
}

uint32_t loadProgramLength() {
    return (programMem.read(LANGUAGE_FLAG_BYTE + 0) << 24) |
           (programMem.read(LANGUAGE_FLAG_BYTE + 1) << 16) |
           (programMem.read(LANGUAGE_FLAG_BYTE + 2) << 8)  |
           (programMem.read(LANGUAGE_FLAG_BYTE + 3));
}
