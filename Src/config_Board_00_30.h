//*******************************************************************
/*!
\file   ...
\author Thomas Breuer
\date   ...
\brief  Configuration file
*/

//*******************************************************************
#include "BT_RN4678.h"

//*******************************************************************
System sys;

PinConfig::MAP PinConfig::table[] =
{
  // UART 3 (Bluetooth)
  USART3_TX_PD8,
  USART3_RX_PD9,
  USART3_CTS_PD11,
  USART3_RTS_PD12,

  //UART 2 SensorPort 1
  USART2_TX_PD5,
  USART2_RX_PD6,

  //UART 5 SensorPort 2
  UART5_TX_PC12,
  UART5_RX_PD2,

  //UART 4 SensorPort 3
  UART4_TX_PC10,
  UART4_RX_PC11,

  //UART 1 SensorPort 4
  USART1_TX_PA9,
  USART1_RX_PA10,

  // Timer (Encoder-1)
  TIM5_CH1_PA0,
  TIM5_CH2_PA1,

  // Timer (Encoder-2)
  TIM4_CH1_PB6,
  TIM4_CH2_PB7,

  // Timer (Encoder-3)
  TIM3_CH1_PA6,
  TIM3_CH2_PA7,

  // Timer (Encoder-4)
  TIM1_CH1_PE9,
  TIM1_CH2_PE11,

  // Timer (PWM)
  TIM8_CH1_PC6,
  TIM8_CH2_PC7,
  TIM8_CH3_PC8,
  TIM8_CH4_PC9,

  TIM9_CH1_PE5,
  TIM9_CH2_PE6,
  //TIM12_CH1_PB_14,
  //TIM12_CH2_PB_15,

  // ADC
  ADC12_IN14_PC4,   // Vcc-Messung

  ADC123_IN10_PC0,    // Analog-1
  ADC123_IN11_PC1,    // Analog-2
  ADC123_IN12_PC2,    // Analog-3
  ADC123_IN13_PC3,  // Analog-4

  ADC12_IN4_PA4,    // Analog-2-1
  ADC12_IN5_PA5,    // Analog-2-2
  ADC12_IN8_PB0,    // Analog-2-3
  ADC12_IN9_PB1,  // Analog-2-4

  // USB
  //  OTG_FS_ID_PA_10,
  OTG_FS_DM_PA11,
  OTG_FS_DP_PA12,

  END_OF_TABLE
};

///**********************************************************
///
///  The NXT-Ultrasonic sensor does not work after power up.
///  The reason is unknown, but a delay after startup and
///  before global object instantiation is a good workaround
///
///**********************************************************
class TimeDelay4NxtUltrasonic
{
public:
    TimeDelay4NxtUltrasonic()
    {
         System::delayMilliSec(20);
    }
} dummyTimeDelay4NxtUltrasonic;

// UART
//-----
cHwUART_Ext      btx  ( Uart_Mcu::USART_3, 115200 //307200
                                         ,Uart_Mcu::CTS
                                         |Uart_Mcu::RTS, 1024, 1024 );  // Bluetooth module on board

cHwUART_Ext      uart1 ( Uart_Mcu::USART_2, 2400, 0,100, 100 );
cHwUART_Ext      uart2 ( Uart_Mcu::UART_5, 2400, 0,100, 100 );
cHwUART_Ext      uart3 ( Uart_Mcu::UART_4,  2400,0, 100, 100 );
cHwUART_Ext      uart4 ( Uart_Mcu::USART_1, 2400, 0,100, 100 );

// Ports
//------
Port_Mcu         portA ( Port_Mcu::PA );
Port_Mcu        portB ( Port_Mcu::PB );
Port_Mcu        portC ( Port_Mcu::PC );
Port_Mcu        portD ( Port_Mcu::PD );
Port_Mcu        portE ( Port_Mcu::PE );

// UART Bluetooth
Port_Mcu::Pin     BT_Wakeup( portD, 3 );
Port_Mcu::Pin     BT_RST_N ( portD, 4 );
Port_Mcu::Pin     BT_Status( portC, 5 );

// Timer
//------
Timer_Mcu       tim1  ( Timer_Mcu::TIM_1,   100 );
Timer_Mcu       tim8  ( Timer_Mcu::TIM_8,    50 );
Timer_Mcu       tim10 ( Timer_Mcu::TIM_10,   30000 );
//cHwTimer_N       tim11 ( cHwTimer_N::TIM_11,   50 );
Timer_Mcu       tim9  ( Timer_Mcu::TIM_9,   16000 );
Timer_Mcu       timer ( Timer_Mcu::TIM_2,  1000 );

TaskManager taskManager( timer );

// ADC
//----
Adc_Mcu         adc   ( Adc_Mcu::ADC_1, timer );

// Motor-Enable
//-------------
Digital         enable( portA, 3, Digital::Out, 1 );

// Motor-PWM
//----------
cDevAnalogOutPWM    pwm_A( tim8, Timer::NORMAL, 1, 1000.0, 0.0 );
cDevAnalogOutPWM    pwm_B( tim8, Timer::NORMAL, 0, 1000.0, 0.0 );
cDevAnalogOutPWM    pwm_C( tim8, Timer::NORMAL, 2, 1000.0, 0.0 );
cDevAnalogOutPWM    pwm_D( tim8, Timer::NORMAL, 3, 1000.0, 0.0 );

// Modellbau-Servo-PWM
//--------------------
cDevAnalogOutPWM    pwm_ModellServo1( tim9, Timer::NORMAL, 0, 16.0, 0.0 );
cDevAnalogOutPWM    pwm_ModellServo2( tim9, Timer::NORMAL, 1, 16.0, 0.0 );


// Motor-CW/CCW
//-------------
Digital         cw_A ( portE, 14, Digital::Out, 0 );
Digital         ccw_A( portE, 15, Digital::Out, 0 );
Digital         cw_B ( portB, 11, Digital::Out, 0 );
Digital         ccw_B( portB, 10, Digital::Out, 0 );
Digital         cw_C ( portE,  7, Digital::Out, 0 );
Digital         ccw_C( portE,  8, Digital::Out, 0 );
Digital         cw_D ( portD, 14, Digital::Out, 0 );
Digital         ccw_D( portD, 13, Digital::Out, 0 );

// Motor-Encoder
//--------------
cHwEncoder_N     enc_A ( cHwEncoder_N::TIM_5, cHwEncoder_N::NORMAL );
cHwEncoder_N     enc_B ( cHwEncoder_N::TIM_4, cHwEncoder_N::NORMAL );
cHwEncoder_N     enc_C ( cHwEncoder_N::TIM_3, cHwEncoder_N::NORMAL );
cHwEncoder_N     enc_D ( cHwEncoder_N::TIM_1, cHwEncoder_N::NORMAL );

// Motor
//------
cDevMotorDirection mot1( cw_A, ccw_A, pwm_A );
cDevMotorDirection mot2( cw_B, ccw_B, pwm_B );
cDevMotorDirection mot3( cw_C, ccw_C, pwm_C );
cDevMotorDirection mot4( cw_D, ccw_D, pwm_D );


// Digital-Eingang
Port::Pin     digitalA_1   ( portD,  5 );   // S1.5, SCL
Port::Pin     digitalB_1   ( portD,  6 );   // S1.6, SDA, ExtInt
Port::Pin     digitalC_1   ( portA,  4 );   // S1.2
Port::Pin     digitalD_1   ( portC,  0 );   // S1.1
Port::Pin     digitalH_1   ( portD, 15 );   // S1.1-HS

Port::Pin     digitalA_2   ( portC, 12 );   // S2.5, SCL
Port::Pin     digitalB_2   ( portD,  2 );   // S2.6, SDA, ExtInt
Port::Pin     digitalC_2   ( portA,  5 );   // S2.2
Port::Pin     digitalD_2   ( portC,  1 );   // S1.1
Port::Pin     digitalH_2   ( portD,  1 );   // S2.1-HS

Port::Pin     digitalA_3   ( portC, 10 );   // S3.5, SCL
Port::Pin     digitalB_3   ( portC, 11 );   // S3.6, SDA, ExtInt
Port::Pin     digitalC_3   ( portB,  0 );   // S3.2
Port::Pin     digitalD_3   ( portC,  2 );   // S1.1
Port::Pin     digitalH_3   ( portA,  8 );   // S3.1-HS

Port::Pin     digitalA_4   ( portA,  9 );   // S4.5, SCL
Port::Pin     digitalB_4   ( portA, 10 );   // S4.6, SDA, ExtInt
Port::Pin     digitalC_4   ( portB,  1 );   // S4.2
Port::Pin     digitalD_4   ( portC,  3 );   // S1.1
Port::Pin     digitalH_4   ( portD,  0 );   // S4.1-HS

cHwExtInt_N      extInt1( cHwExtInt_N::PD, 6);
cHwExtInt_N      extInt2( cHwExtInt_N::PD, 2);
cHwExtInt_N      extInt3( cHwExtInt_N::PC,11);
cHwExtInt_N      extInt4( cHwExtInt_N::PA,10);

// Digital-Eingang Extra
Digital         taster2(portE,0, Digital::InPU,  1);
Digital         taster1(portE,1, Digital::InPU,  1);

// Versorgungsspannung
cDevAnalogInADC     Vcc ( adc, 14, (133.0f/33*33), 0.0f );   // Uref = 33/10 V; R1=100k, R2=33k

// Analog-Eingang
cDevAnalogInADC     adc1( adc, 10, 3.3f, 0.0f );   // S1.1
cDevAnalogInADC     adc2( adc, 11, 3.3f, 0.0f );   // S2.1
cDevAnalogInADC     adc3( adc, 12, 3.3f, 0.0f );   // S3.1
cDevAnalogInADC     adc4( adc, 13, 3.3f, 0.0f );   // S4.1

// Analog-Eingang
cDevAnalogInADC     adc21( adc, 4, 3.3f, 0.0f );   // S1.2
cDevAnalogInADC     adc22( adc, 5, 3.3f, 0.0f );   // S2.2
cDevAnalogInADC     adc24( adc, 8, 3.3f, 0.0f );   // S3.2
cDevAnalogInADC     adc23( adc, 9, 3.3f, 0.0f );   // S4.2


// User
//-----
Digital      btnUsr1(portC, 14, Digital::InPU,  1 );
Digital      btnUsr2(portC, 15, Digital::InPU,  1 );
Digital      ledUsr1(portE,  3, Digital::OutOD, 1 );
Digital      ledUsr2(portE,  2, Digital::OutOD, 1 );

DigitalButton btn   ( btnUsr1, taskManager, 100, 500 );
DigitalButton btnAlt( btnUsr2, taskManager, 100, 500 );


DigitalIndicator indUsr1( ledUsr1, taskManager, 100 );
DigitalIndicator indUsr2( ledUsr2, taskManager, 100 );

// Power-LED
Digital          ledPwr (portE,  4, Digital::OutOD, 1 );
DigitalIndicator pwrIndicator(ledPwr, taskManager );


// Task-Timer
//-----------
//cTaskHandler     taskHandler( &timer );

//
//-----------
Memory_Flash  mem0( 11 );
Memory_Flash  mem1( 10 );
Memory_Flash  mem2( 9 );
//Memory_Flash  mem3_1( cHwMemory_Flash::SECTOR_7 );
//Memory_RAM    mem3_2;

USBdeviceDescriptor_0     desc;
USBdevice_Mcu usb( desc );

BT_RN4678 btmodul(btx,BT_RST_N,BT_Wakeup,BT_Status);
Bluetooth &btm = btmodul;
Uart &bt = btmodul.uart;

//EOF
