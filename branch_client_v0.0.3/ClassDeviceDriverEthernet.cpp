#include "InterfaceClientDeviceDriverEthernet.hpp"
#include "InterfaceDeviceDriverEthernet_ServicesSystemEcuM.hpp"
#include "InterfaceDeviceDriverEthernet_ServicesSystemSchM.hpp"

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>

#include <fstream>

static constexpr size_t SIZE_MAX_BUFFER = 64;

typedef struct{
   char                                     buffer[SIZE_MAX_BUFFER];
   chrono::time_point<chrono::steady_clock> point_time_ns_start_system;
   struct sockaddr_in                       stAddress;
   ifstream                                 StreamFileInput;
   ofstream                                 StreamFileOutput;
   istream*                                 ptrStreamInput;
   ostream*                                 ptrStreamOutput;
   int                                      FdSocketServer;
}Type_stContext_DeviceDriverEthernet;

#include "InterfaceServicesSystemEcuM_DeviceDriverEthernet.hpp"
class ClassDeviceDriverEthernet:
      public InterfaceClientDeviceDriverEthernet
   ,  public InterfaceClientServicesSystemEcuM
   ,  public InterfaceClientServicesSystemSchM
{
   private:
             Type_stContext_DeviceDriverEthernet  stContext;
             Type_stDataInit_DeviceDriverEthernet stDataInit;
      double ldGetDurationTimeElapsed_s(void){
         chrono::time_point<chrono::steady_clock> point_time_ns_now = chrono::steady_clock::now();
         return(
            (
                  (
                        point_time_ns_now
                     -  stContext.point_time_ns_start_system
                  ).count()
               %  3600000000000LL
            ) / 1000000000.0
         );
      }
      void lvWriteLog(ostringstream& lStringStreamOutput){
         string lStringFormatted = lStringStreamOutput.str();
         if(
               (false == stDataInit.bIsFoundFlagNameFileInput)
            && (true  == stDataInit.bIsFoundFlagNameFileOutput)
         ){
            cout << lStringFormatted << endl;
         }
         *stContext.ptrStreamOutput << lStringFormatted << endl;
         stContext.ptrStreamOutput->flush();
      }

   public:
      void vFunctionDeInit(void){
         close(stContext.FdSocketServer);
         if(stDataInit.bIsFoundFlagNameFileInput){
            stContext.StreamFileInput.close();
         }
         if(stDataInit.bIsFoundFlagNameFileOutput){
            stContext.StreamFileOutput.close();
         }
      }
      ~ClassDeviceDriverEthernet(){
         vFunctionDeInit();
      }
      void vFunctionInit(void){
         stContext.point_time_ns_start_system = chrono::steady_clock::now();
         if(stDataInit.bIsFoundFlagNameFileInput){
            stContext.StreamFileInput.open(stDataInit.strNameFileInput);
            if(stContext.StreamFileInput.is_open()){
               stContext.ptrStreamInput = &stContext.StreamFileInput;
            }
            else{
               cerr << "Error: Couldn't open input file " << stDataInit.strNameFileInput << "!\n";
               return;
            }
         }
         else{
            stContext.ptrStreamInput = &cin;
         }
         if(stDataInit.bIsFoundFlagNameFileOutput){
            stContext.StreamFileOutput.open(stDataInit.strNameFileOutput, ios::app);
            if(!stContext.StreamFileOutput.is_open()){
               cerr << "Error: Cannot write to output file '" << stDataInit.strNameFileOutput
                    << "'. It may be read-only or the directory does not exist.\n";
               return;
            }
            else{
               stContext.ptrStreamOutput = &stContext.StreamFileOutput;
            }
         }
         else{
            stContext.ptrStreamOutput = &cout;
         }
         stContext.FdSocketServer = socket(
               AF_INET
            ,  SOCK_STREAM
            ,  0
         );
         stContext.stAddress.sin_family = AF_INET;
         stContext.stAddress.sin_port   = htons(8080);
         inet_pton(
               AF_INET
            ,  (
                  (
                        true
                     == stDataInit.bIsFoundFlagAddressIp
                  )
                  ?  stDataInit.strAddressIp.c_str()
                  :  "127.0.0.1"
               )
            ,  &stContext.stAddress.sin_addr
         );
         if(
               connect(
                     stContext.FdSocketServer
                  ,  (struct sockaddr*) &stContext.stAddress
                  ,  sizeof(struct sockaddr_in)
               )
            <  0
         ){
            perror("Connection failed");
            exit(EXIT_FAILURE);
         }
      }
      void vFunctionMain(void){
         if(false == stDataInit.bIsFoundFlagNameFileInput){
            cout << "xxxx.xxxxxx 1  172.17.0.1      Tx   d 64 ";
         }
         string lStringInput;
         if(stDataInit.bIsFoundFlagNameFileInput){
            bool lbFoundRequest = false;
            bool lbFoundEoF     = false;
            while(
                  false == lbFoundRequest
               && false == lbFoundEoF
            ){
               if(
                  getline(
                        *stContext.ptrStreamInput
                     ,  lStringInput
                  )
               ){
                  istringstream iss(lStringInput);
                  string        lStringToken;
                  string        lStringTokenLast;
                  while(iss >> lStringToken){
                     if("Rx" == lStringToken){
                        break;
                     }
                     else if("Tx" == lStringToken){
                        lbFoundRequest = true;
                     }
                     else{
                        lStringTokenLast = lStringToken;
                     }
                  }
                  if(lbFoundRequest){
                     lStringInput = lStringTokenLast;
                  }
               }
               else{
                  lbFoundEoF = true;
               }
            }
         }
         else{
            getline(
                  *stContext.ptrStreamInput
               ,  lStringInput
            );
         }
         if(false == stDataInit.bIsFoundFlagNameFileInput){
            cout << "\033[1A\r";
            cout << "\033[K";
         }
         send(
               stContext.FdSocketServer
            ,  lStringInput.c_str()
            ,  lStringInput.length()
            ,  0
         );
         ostringstream lStringStreamOutput;
         lStringStreamOutput << fixed << setprecision(6) << setfill('0') << setw(11) << ldGetDurationTimeElapsed_s() << " 1  172.17.0.1      Tx   d 64 " << lStringInput;
         lvWriteLog(lStringStreamOutput);
         memset(stContext.buffer, 0, SIZE_MAX_BUFFER);
         if(
               read(
                     stContext.FdSocketServer
                  ,  stContext.buffer
                  ,  SIZE_MAX_BUFFER
               )
            <= 0
         ){
            InterfaceServicesSystemEcuM_DeviceDriverEthernet_ptr->vSetStatusEcuM(eStatusEcuM_InitShutdown);
         }
         else{
            lStringStreamOutput.str("");
            lStringStreamOutput.clear();
            lStringStreamOutput.seekp(0);
            lStringStreamOutput << fixed << setprecision(6) << setfill('0') << setw(11) << ldGetDurationTimeElapsed_s() << " 1  172.17.0.2      Rx   d 64 " << stContext.buffer;
            lvWriteLog(lStringStreamOutput);
            if(
                  0
               == strcmp(
                        "025104"
                     ,  stContext.buffer
                  )
            ){
               InterfaceServicesSystemEcuM_DeviceDriverEthernet_ptr->vSetStatusEcuM(eStatusEcuM_InitShutdown);
            }
         }
      }
      void vWriteDataInit(
         Type_stDataInit_DeviceDriverEthernet&& lrstDataInit
      ){
         stDataInit = move(lrstDataInit);
      }
};

static ClassDeviceDriverEthernet            InstanceDeviceDriverEthernet;
       InterfaceClientDeviceDriverEthernet* InterfaceClientDeviceDriverEthernet_ptr                   = &InstanceDeviceDriverEthernet;
       InterfaceClientServicesSystemEcuM*   InterfaceClientServicesSystemEcuM_ptrDeviceDriverEthernet = &InstanceDeviceDriverEthernet;
       InterfaceClientServicesSystemSchM*   InterfaceClientServicesSystemSchM_ptrDeviceDriverEthernet = &InstanceDeviceDriverEthernet;
