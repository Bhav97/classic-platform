/*
* Configuration of module: Port (Port_Cfg.h)
*
* Created by:              
* Copyright:               
*
* Configured for (MCU):    TMS570
*
* Module vendor:           ArcCore
* Generator version:       2.0.2
*
* Generated by Arctic Studio (http://arccore.com) 
*           on Thu Jun 16 18:30:20 CEST 2011
*/


#if !(((PORT_SW_MAJOR_VERSION == 1) && (PORT_SW_MINOR_VERSION == 0)) )
#error Port: Configuration file expected BSW module version to be 1.0.*
#endif


#ifndef PORT_CFG_H_
#define PORT_CFG_H_

#include "Std_Types.h"


/** Build version info API */
#define PORT_VERSION_INFO_API				STD_OFF
/** Enable Development Error Trace */
#define PORT_DEV_ERROR_DETECT				STD_OFF
/** Build change pin direction API */
#define PORT_SET_PIN_DIRECTION_API	        STD_OFF
/** Allow Pin mode changes during runtime */
#define PORT_SET_PIN_MODE_API               STD_OFF

#define PORT_NUMBER_OF_PINS	2

#define PORT_FUNC		(1 << 1)
#define PORT_FUNC_NO	(0 << 1)
#define PORT_PULL_NONE	(1 << 2)
#define PORT_PULL_UP	(1 << 3)
#define PORT_PULL_DOWN	(0 << 3)
#define PORT_ODE_ENABLE	(1 << 4)
#define PORT_DIRECTION_CHANGEABLE (1 << 5)

/** HW specific symbolic names of pins */
/** @req PORT013 */
typedef enum
{
	PORT_PIN_DCAN1_TX = 0x0800,
	PORT_PIN_DCAN1_RX = 0x0801,  
} Port_PinType;

/** Port pad mappings */
#define	PORT_PAD_106	0x0800 // PORT_PIN_DCAN1_TX
#define	PORT_PAD_107	0x0801 // PORT_PIN_DCAN1_RX

typedef struct {
	Port_PinType pin;
	uint8 conf;
} Port_ConfiguredPinType;

/** Top level configuration container */
/** @req PORT073 */
typedef struct
{
  const Port_ConfiguredPinType pins[PORT_NUMBER_OF_PINS];
} Port_ConfigType;

/** Instance of the top level configuration container */
extern const Port_ConfigType PortConfigData;


#endif /*PORT_CFG_H_*/
