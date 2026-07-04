#pragma once

#include <string>
using namespace std;

typedef struct{
   string strNameFileInput;
   string strNameFileOutput;
   string strAddressIp;
   bool   bIsFoundFlagNameFileInput;
   bool   bIsFoundFlagNameFileOutput;
   bool   bIsFoundFlagAddressIp;
}Type_stDataInit_DeviceDriverEthernet;

class InterfaceClientDeviceDriverEthernet{
   public:
      virtual     ~InterfaceClientDeviceDriverEthernet ()                                                    = default;
      virtual void vWriteDataInit                      (Type_stDataInit_DeviceDriverEthernet&& lrstDataInit) = 0;
};

extern InterfaceClientDeviceDriverEthernet* InterfaceClientDeviceDriverEthernet_ptr;
