/*! @file Activate.cpp
 *  @version 3.1.8
 *  @date Aug 05 2016
 *
 *  @brief
 *  Activation process for the STM32 example App.
 *
 *  Copyright 2016 DJI. All right reserved.
 *
 * */

#include "Activate.h"

extern CoreAPI defaultAPI;
extern CoreAPI *coreApi;
extern Flight flight;
extern FlightData flightData;

void
User_Activate ()
{
  static char key_buf[65] = "6e356224a9b11f8e5e15a7a847d9b952a119163831d82c6bd2bba202a34c5191"; /*"your app_key"*/

  ActivateData user_act_data;
  user_act_data.ID = 1037794; /*need your key in number like: 111000*/


  //! Change the version string to your platform/version as defined in DJI_Version.h
  //! Starting from OSDK 3.2.1, you do not need to do this. Instead, call droneVersion before activation.
  user_act_data.version = versionM100_31;

  user_act_data.encKey = key_buf;

  coreApi->activate (&user_act_data);

}
