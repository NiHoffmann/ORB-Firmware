//*******************************************************************
/*!
\file   Bluetooth.h
\author Thomas Breuer (Bonn-Rhein-Sieg University of Applied Sciences)
\date   03.02.2021
*/

//*******************************************************************
#include "UART_Ext.h"

//*******************************************************************
/*!
\class
\brief
*/
class Bluetooth
{
  public:
    //---------------------------------------------------------------
    Bluetooth( cHwUART_Ext  &_uart )
    : uart(_uart)
    {
    }

    //---------------------------------------------------------------
    virtual bool send(const char *str)
    {
      BYTE c;

      // clear buffer
      while(uart.get(c));

      uart.set(str);
      return( false );
    }

    //---------------------------------------------------------------
    virtual bool waitForAckn(const char *str) = 0;

  public:
    //---------------------------------------------------------------
    virtual void config(const char *name) = 0;

  public:
    //---------------------------------------------------------------
    cHwUART_Ext      &uart;

};
