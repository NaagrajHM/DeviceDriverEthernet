#pragma once

class InterfaceClientDeviceDriverEthernet{
   public:
      virtual     ~InterfaceClientDeviceDriverEthernet ()                 = default;
//    virtual void vTransmit                           (Message& message) = 0;
};

extern InterfaceClientDeviceDriverEthernet* InterfaceClientDeviceDriverEthernet_ptr;
