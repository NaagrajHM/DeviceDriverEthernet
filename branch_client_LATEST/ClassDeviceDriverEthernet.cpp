#include "InterfaceClientDeviceDriverEthernet.hpp"
#include "InterfaceDeviceDriverEthernet_ServicesSystemEcuM.hpp"
#include "InterfaceDeviceDriverEthernet_ServicesSystemSchM.hpp"

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

static constexpr size_t SIZE_MAX_BUFFER = 64;
string stringAddressIP;
#include "InterfaceServicesSystemEcuM_DeviceDriverEthernet.hpp"
class ClassDeviceDriverEthernet:
      public InterfaceClientDeviceDriverEthernet
   ,  public InterfaceClientServicesSystemEcuM
   ,  public InterfaceClientServicesSystemSchM
{
   private:
             char        buffer[SIZE_MAX_BUFFER];
      struct sockaddr_in stAddress;
             int         FdSocketServer;

   public:
      void vFunctionDeInit(void){
         close(FdSocketServer);
      }
      ~ClassDeviceDriverEthernet(){
         vFunctionDeInit();
      }
      void vFunctionInit(void){
         FdSocketServer = socket(
               AF_INET
            ,  SOCK_STREAM
            ,  0
         );
         stAddress.sin_family = AF_INET;
         stAddress.sin_port   = htons(8080);
         inet_pton(
               AF_INET
            ,  stringAddressIP.c_str()
            ,  &stAddress.sin_addr
         );
         if(
               connect(
                     FdSocketServer
                  ,  (struct sockaddr*) &stAddress
                  ,  sizeof(stAddress)
               )
            <  0
         ){
            perror("Connection failed");
            exit(EXIT_FAILURE);
         }
      }
      void vFunctionMain(void){
         string input;
         cout << "x.xxxxxx 1  7E0             Tx   d 8 ";
         getline(cin, input);
         send(
               FdSocketServer
            ,  input.c_str()
            ,  input.length()
            ,  0
         );
         memset(buffer, 0, SIZE_MAX_BUFFER);
         if(
               read(
                     FdSocketServer
                  ,  buffer
                  ,  SIZE_MAX_BUFFER
               )
            <= 0
         ){
            InterfaceServicesSystemEcuM_DeviceDriverEthernet_ptr->vSetStatusEcuM(eStatusEcuM_InitShutdown);
         }
         else{
            cout << "x.xxxxxx 1  7E8             Rx   d 8 " << buffer << endl;
            if(
               strcmp(
                     "025104"
                  ,  buffer
               )
            ){
            }
            else{
               InterfaceServicesSystemEcuM_DeviceDriverEthernet_ptr->vSetStatusEcuM(eStatusEcuM_InitShutdown);
            }
         }
      }
};

static ClassDeviceDriverEthernet            InstanceDeviceDriverEthernet;
       InterfaceClientDeviceDriverEthernet* InterfaceClientDeviceDriverEthernet_ptr                   = &InstanceDeviceDriverEthernet;
       InterfaceClientServicesSystemEcuM*   InterfaceClientServicesSystemEcuM_ptrDeviceDriverEthernet = &InstanceDeviceDriverEthernet;
       InterfaceClientServicesSystemSchM*   InterfaceClientServicesSystemSchM_ptrDeviceDriverEthernet = &InstanceDeviceDriverEthernet;
