#include "InterfaceClientDeviceDriverEthernet.hpp"
#include "InterfaceDeviceDriverEthernet_ServicesSystemEcuM.hpp"
#include "InterfaceDeviceDriverEthernet_ServicesSystemSchM.hpp"
#include "InterfaceServicesCommunicationDcm_DeviceDriverEthernet.hpp"

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iomanip>

class ClassDeviceDriverEthernet:
      public InterfaceClientDeviceDriverEthernet
   ,  public InterfaceClientServicesSystemEcuM
   ,  public InterfaceClientServicesSystemSchM
{
   private:
      struct sockaddr_in stAddress;
             int         FdSocketServer;
             int         FdSocketClient;
             int         LengthAddress;

   public:
      ~ClassDeviceDriverEthernet(){
         close(FdSocketClient);
         close(FdSocketServer);
      }
      void vFunctionInit(void){
         FdSocketServer = socket(
               AF_INET
            ,  SOCK_STREAM
            ,  0
         );
         stAddress.sin_family      = AF_INET;
         stAddress.sin_port        = htons(8080);
         stAddress.sin_addr.s_addr = INADDR_ANY;
         if(
               bind(
                     FdSocketServer
                  ,  (struct sockaddr*) &stAddress
                  ,  sizeof(stAddress)
               )
            <  0
         ){
            perror("Bind failed");
            exit(EXIT_FAILURE);
         }
         else{
            listen(FdSocketServer, 3);
            LengthAddress = sizeof(stAddress);
            FdSocketClient = accept(
                                      FdSocketServer
               ,  (struct sockaddr*) &stAddress
               ,  (socklen_t*)       &LengthAddress
            );
         }
      }
      void vFunctionMain(void){
         Message message;
         memset(message.data, 0, SIZE_MAX_BUFFER);
         if(
               read(
                     FdSocketClient
                  ,  message.data
                  ,  SIZE_MAX_BUFFER
               )
            <= 0
         ){
            exit(EXIT_FAILURE);
         }
         else{
            InterfaceServicesCommunicationDcm_DeviceDriverEthernet_ptr->vRxIndication(message);
         }
      }
      void vTransmit(
         Message& message
      ){
         send(
               FdSocketClient
            ,  message.data
            ,  strlen(message.data)
            ,  0
         );
      }
};

static ClassDeviceDriverEthernet            InstanceDeviceDriverEthernet;
       InterfaceClientDeviceDriverEthernet* InterfaceClientDeviceDriverEthernet_ptr                   = &InstanceDeviceDriverEthernet;
       InterfaceClientServicesSystemEcuM*   InterfaceClientServicesSystemEcuM_ptrDeviceDriverEthernet = &InstanceDeviceDriverEthernet;
       InterfaceClientServicesSystemSchM*   InterfaceClientServicesSystemSchM_ptrDeviceDriverEthernet = &InstanceDeviceDriverEthernet;
