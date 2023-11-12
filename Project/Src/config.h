//*******************************************************************
/*!
\file   config.h
\author Thomas Breuer
\date   02.02.2021
\brief  Configuration file
*/

//*******************************************************************
#include "descriptor.cpp"

//*******************************************************************
#if (BOARD_MAIN == 0 && BOARD_SUB == 22)

  #include "config_Board_00_22.h"

#elif (BOARD_MAIN == 0 && BOARD_SUB == 30)

  #include "config_Board_00_30.h"

#else

  #error "Board hardware version not defined"

#endif

//EOF
