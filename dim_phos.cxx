//// dim_phos.cxx : Server which interfaces STU API with PVSS using DIM protocol
//
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <dis.hxx>
#include <unistd.h>
#include "STU_API.h"
#include <string>
#include <fstream> 
#include <math.h>
#include <time.h>
#include <bitset>

//#define _SLOW_DIM
#define _STEP_DURATION 10
#define _FIRST_TRU_SYNC 0
#define _ERROR_LIMIT 10
#define _FINE_DELAY1 0
#define _CTRL_REG 3

#define _L0ProcessingTime 279
#define _ParaSize 15

string FPGAFile="STU_FPGA.mcs";

using namespace std;

class myCmd: public DimServer
{
  DimCommand	*en_L0                  ;
  DimCommand	*L0_mask                ;
  DimCommand	*ExitDIM                ;
  DimCommand	*GetI2CData             ;
  DimCommand	*ErrorCount             ;
  DimCommand	*SetTriggerDelays_TIN   ;
  
  DimCommand	*SOR_number             ;
  DimCommand	*SOR_sync               ;
  DimCommand	*SOR                    ;
  DimCommand	*EN_MEM_PAT_TST         ;
  
  DimCommand	*EOR                    ;
  DimCommand	*sorReset               ;
  
  DimCommand  *ReloadConfig           ;
  
  DimCommand	*Configure              ;
  DimCommand	*ConfFPGA               ;
  DimCommand	*ResetBoard             ;
  DimCommand	*ReadIDCODEs            ;
  DimCommand	*ReadMonitor            ;
  DimCommand	*GetDDLStatus           ;
  DimCommand	*ReadTTC                ;
  DimCommand	*GetSerialStatus        ;
  DimCommand	*WriteTTC               ;
  DimCommand	*ReadReceptionPointer   ;
  DimCommand	*RegionEnable           ;
  DimCommand	*RegisterLoopBack       ;
  DimCommand	*UpdatePulse            ;
  DimCommand	*UpdateMode             ;
  DimCommand	*ChangeSerialAdd        ;
  DimCommand	*ReadISPFLASHIDCODES    ;
  DimCommand	*ISPFLASHRegLoopBack    ;
  DimCommand	*ReadFlashSignature     ;
  DimCommand	*ProgramFlash           ;
  DimCommand	*RetrieveISPFLASHStatus ;
  DimCommand	*ReadV0all              ;
  DimCommand	*SetGammaThreshold      ;
  DimCommand	*SetJetThreshold        ;
  DimCommand    *MonErrorCount          ;//for Run2
  DimCommand    *BusyClear              ;//for Run2
  DimCommand    *EnablePattern          ;//for Run2
  
  DimCommand	*SetRandomVal           ;
  DimCommand	*SetTriggerDelays       ;
  DimCommand	*SetOpcode              ;
  DimCommand	*SetL0PhaseDelay        ;
  DimCommand	*ReadOffsetCount        ;
  DimCommand	*SearchL0BestPhase      ;
  DimCommand	*GetConfiguration       ;
  DimCommand	*GetMonitorValues       ;

  //Data Structure to retreive command values
  typedef struct{
  	int           G_A         ;//Gamma Threshold
  	int           G_B         ;
  	int           G_C         ;
  	int           G_A_mid     ;
  	int           G_B_mid     ;
  	int           G_C_mid     ;
  	int           G_A_low     ;
  	int           G_B_low     ;
  	int           G_C_low     ;
  	unsigned int  L1_G_delay  ;
  	int           GetRawData  ;
  	unsigned int  region      ;
	unsigned int  L0mask      ;
  } Cmd_struct;
 
  //Data Structure to receive Mode bits
  typedef struct{
  	int   set_phase_manu_cmd  ;
  	int   recompute_phase_cmd ;
  	int   do_slip_manu_cmd    ;
  	int   L0_cmd              ;
  	int   L1a_cmd             ;
  	int   L2a_cmd             ;
  	int   L2r_cmd             ;
  	int   ClearL0Cnt_cmd      ;
  	int   setL0Phase_cmd      ;
  	int   MeasJitter_cmd      ;
  	int   SOR_reset_cmd       ;
  } Cmd_Pulse;
  
  //Data Structure to receive Mode bits
  typedef struct{
  	int   PhaseManuValue_box      ;
  	int   Deser_ADD_box           ;
  	int   EnableL0OverTTC_cmd     ;
  	int   GetRAWDataChk_cmd       ;
  	int   UseTestPatternChk_cmd   ;
  	int   UseFakeTriggersChk_cmd  ;
  } Cmd_Mode;
  
  //Quadratic parameters of Gamma and Jet equations
  typedef struct{
  	int   A       ;
  	int   B       ;
  	int   C       ;
  	int   A_mid   ;
  	int   B_mid   ;
  	int   C_mid   ;
  	int   A_low   ;
  	int   B_low   ;
  	int   C_low   ;
  } Cmd_Param;
  














  //Delay parameters 
  typedef struct{
  	int   L0          ;
  	int   L1_G_delay  ;
  } Cmd_Delays;
 

  typedef struct{
    int   TRU[nb_PORT] ;
 	} Cmd_TRU;

  string current_date_FileName();
  // Calculate the absolute value of the error counters 
  unsigned int abs_errors_counts(unsigned int value1, unsigned int value2);

  //Function that displays the Client attributes of the command sent
  string DimClientName();

  ///////////////////////////////////////////////
  // Save config in "STUCONFIG" for the next run
  // Update of the STU_Reload_CONFIG file
  void write_STU_CONFIG(	
      int             GetRawData    , 
      unsigned        region        , 
      unsigned short  L1_G_delay    , 
      int             fw            ,
      short           Trigger_mode  ,
      int             G_A           , 
      int             G_B           , 
      int             G_C           , 	
      int             G_A_mid       , 
      int             G_B_mid       , 
      int             G_C_mid       ,
      int             G_A_low       , 
      int             G_B_low       , 
      int             G_C_low        
      );
  
  
  
  
  
  
  
  
  
  
  // Load config in "STUCONFIG"
  void ReloadConfig_F (	
      int&            GetRawData    , 
      unsigned&       region        ,  
      unsigned short& L1_G_delay    , 
      int&            fw            ,
      short&          Trigger_mode  ,
      int&            G_A           ,
      int&            G_B           , 
      int&            G_C           ,	
      int&            G_A_mid       , 
      int&            G_B_mid       , 
      int&            G_C_mid       ,
      int&            G_A_low       , 
      int&            G_B_low       , 
      int&            G_C_low        
      );
  
 
 
 
 
 
 
 
 
 
  void commandHandler();
  bool readTRUconfig(bitset<28> &bs, int type, int sru);
  
public:

  TSTU_API STU_API;
  //DimService *Service_L1_J_delay;
  DimService *Service_L1_G_delay;
  DimService *Service_Conf_done;
  DimService *Service_I2CData;
  DimService *Service_SyncAlarm;
  DimService *Service_ConfFPGA_Done;
  DimService *Service_ResetFPGA_Done;
  DimService *Service_SOR_Done;
  DimService *Service_Firmware;
  
  DimService *Service_GetTriggers;
  DimService *Service_GetOpcode;
  
  DimService *Service_Connect_Status;
  DimService *Service_Temperature_Val;
  DimService *Service_GetConfig;
  DimService *Service_TRU_sync;
  DimService *Service_TRU_sync_error;
  DimService *Service_temp;
  DimService *Service_vccint;
  DimService *Service_vccaux;
  
  Cmd_struct GetConfig;
  Cmd_TRU TRU_sync;
  Cmd_TRU TRU_sync_error;

  //
  int L0_mask_int;
  int en_L0_int;
  //
  int I2CData_pvss;
  int I2CData_read;
  //TIN variables
  int TIN_delay;
  char opcode_char;
  short opcode;
  
  unsigned short tableau_sync[nb_PORT];
  unsigned short tableau_sync_tmp[nb_PORT];
  // services
  int Conf_done;
  int TRU_SyncAlarm;
  int SyncAlarm;
  int ConfFPGA_Done;
  int ResetFPGA_Done;
  int SOR_Done;
  
  int SOR_nb_int;
  string SOR_nb_str;
  string TRU_table[nb_PORT];
  void *p_GetConfig;
  void *p_TRU_sync;
  void *p_TRU_sync_error;
  void *p_monitor;
  
  void* p_temp;
  void* p_vccint;
  void* p_vccaux;
  
  ofstream fichierLog;
  ofstream fichierRun;
  ostringstream fichierConfig[_ParaSize];
  ofstream fichierConfig_tmp;
  ofstream fichierConfig_written;
  ofstream fichierConfigFile;
  
  string catch_string;
  
  int CurrentDeserAdd;
  int PhaseManuVal;
  int WriteTTC_val;
  int RegionEnable_val;
  // 
  int G_A     ;
  int G_B     ;
  int G_C     ;
  int G_A_mid ;
  int G_B_mid ;
  int G_C_mid ;
  int G_A_low ;
  int G_B_low ;
  int G_C_low ;
  
  
  
  
  
  
  
  
  int GetRawData;
  int Random_value;
  
  unsigned short  L0delay     ;
  unsigned short  L1_G_delay  ;
  
  unsigned        region      ;
  int             fw          ; //firmware version

  unsigned global_region;
  unsigned long long total_trial[nb_PORT];
  unsigned long long total_error[nb_PORT];
  double total_err_rate[nb_PORT];
  int      isFirstTrial[nb_PORT];

  int en_mem_pat;//test pattern
  int ID_FIRM;
  
  short Trigger_mode;
  
  int RegisterLoopBack_val;
  int ISPFLASHLoopBackReg_val;
  int sorReset_int;
  
  // SetL0Delay from an *.ini file
  string line;
  string delay_line;
  string current_line;
  string delay_value;
  size_t found;
  size_t pos_eq;
  
  int int_delay[nb_PORT];
  
  ifstream fichier;
  
  char status_char[10000];
  
  string status_string;             
  string date_string_for_sync[nb_PORT]; // for Run2
  
  unsigned short L0Count;
  unsigned short L0Latency;
  unsigned short L0_L0_delay;
  
  ///GETIDCODES
  unsigned short IDCODE0;
  unsigned short IDCODE1;
  
  double temperature;
  int align;		

  // The constructor creates the Commands
  myCmd( );

  //Calculate L0/region mask from TRU configs.
  bool CalculateL0Mask(unsigned int &mask);
  
	int cnt_packet_failure[nb_PORT];//HIROKI

};//end of class myCmd




//####################################################################################################
string myCmd::current_date_FileName()
{
  struct tm *log_time;
  time_t timer;
  string temp_date;
  ostringstream oss;
  //get time of the day
  timer=time(NULL);
  log_time=localtime(&timer);
  oss<<"\n**Date: ";
  oss<< dec << log_time->tm_mday;  oss<<"_"   ;
  oss<< log_time->tm_mon+1      ;  oss<<"_"   ;
  oss<< log_time->tm_year+1900  ;  oss<<" "   ;
  oss<< log_time->tm_hour       ;  oss<<"h"   ;
  oss<< log_time->tm_min        ;  oss<<"min" ;
  oss<< log_time->tm_sec        ;  oss<<"s "  ;
  return oss.str();
}
//####################################################################################################
// Calculate the absolute value of the error counters 
unsigned int myCmd::abs_errors_counts(unsigned int value1, unsigned int value2)
{
  if (value2 >= value1) { return (value2 - value1); }
  return (65535 - value1) + value2;
}
//####################################################################################################
//Function that displays the Client attributes of the command sent
string myCmd::DimClientName()
{
  string status_string;
  ostringstream oss_id,oss_name, oss_time, oss_quality;

  int Id= this ->getClientId( );
  oss_id<<Id;
  status_string= "Client Name: " + (string) this ->getClientName( )+ " -> Id: " + oss_id.str()+"\n";
  cout<<status_string<<endl;
  return status_string;
}
//####################################################################################################
// Save config in "STUCONFIG" for the next run
void myCmd::write_STU_CONFIG(	
    int             GetRawData    , 
    unsigned        region        , 
    unsigned short  L1_G_delay    , 
    int             fw            ,
    short           Trigger_mode  ,
    int             G_A           , 
    int             G_B           , 
    int             G_C           , 	
    int             G_A_mid       , 
    int             G_B_mid       , 
    int             G_C_mid       ,
    int             G_A_low       , 
    int             G_B_low       , 
    int             G_C_low        
    )
{










  ofstream fichierConfigFile;
  //Update of the STU_Last_CONFIG file
  fichierConfigFile.open("STUCONFIG", ios::out);
  if(!fichierConfigFile){cout<<" ERROR: Cannot open STUCONFIG file"<< endl;}
  
  fichierConfig[1] << "G_A:"           << "\t\t" << G_A          << endl;
  fichierConfig[2] << "G_B:"           << "\t\t" << G_B          << endl;
  fichierConfig[3] << "G_C:"           << "\t\t" << G_C          << endl;
  fichierConfig[4] << "G_A_mid:"       << "\t\t" << G_A_mid      << endl;
  fichierConfig[5] << "G_B_mid:"       << "\t\t" << G_B_mid      << endl;
  fichierConfig[6] << "G_C_mid:"       << "\t\t" << G_C_mid      << endl;
  fichierConfig[7] << "G_A_low:"       << "\t\t" << G_A_low      << endl;
  fichierConfig[8] << "G_B_low:"       << "\t\t" << G_B_low      << endl;
  fichierConfig[9] << "G_C_low:"       << "\t\t" << G_C_low      << endl;
  fichierConfig[10]<< "L1_G_delay:"    << "\t\t" << L1_G_delay   << endl;
  fichierConfig[11]<< "GetRawData:"    << "\t\t" << GetRawData   << endl;
  fichierConfig[12]<< "region:"        << "\t\t" << hex<< region << endl;
  fichierConfig[13]<< "fw:"            << "\t\t" << fw           << endl;
  fichierConfig[14]<< "Trigger_mode:"  << "\t\t" << Trigger_mode << endl;
  
 
 
 
 
 
 
 
 
 
  fichierConfig[12]<<fichierConfig[13].str()<<fichierConfig[14].str()                                                   ;
  fichierConfig[8] <<fichierConfig[9].str() <<fichierConfig[10].str()<<fichierConfig[11].str()<<fichierConfig[12].str() ;
  fichierConfig[4] <<fichierConfig[5].str() <<fichierConfig[6].str() <<fichierConfig[7].str() <<fichierConfig[8].str()  ;
  fichierConfigFile<<fichierConfig[1].str() <<fichierConfig[2].str() <<fichierConfig[3].str() <<fichierConfig[4].str()  ;



  // clear the streams
  for (int i=1; i<_ParaSize; i++)
  {
    fichierConfig[i].str("");
  }
  fichierConfigFile.close();

  //Update of the STU_Reload_CONFIG file
  fichierConfigFile.open("STU_Reload_CONFIG", ios::out);
  if(!fichierConfigFile){cout<<" ERROR: Cannot open STU_Reload_CONFIG file"<< endl;}

  fichierConfig[1]  << G_A          << endl;
  fichierConfig[2]  << G_B          << endl;
  fichierConfig[3]  << G_C          << endl;
  fichierConfig[4]  << G_A_mid      << endl;
  fichierConfig[5]  << G_B_mid      << endl;
  fichierConfig[6]  << G_C_mid      << endl;
  fichierConfig[7]  << G_A_low      << endl;
  fichierConfig[8]  << G_B_low      << endl;
  fichierConfig[9]  << G_C_low      << endl;
  fichierConfig[10] << L1_G_delay   << endl;
  fichierConfig[11] << GetRawData   << endl;
  fichierConfig[12] << region       << endl;
  fichierConfig[13] << fw           << endl;
  fichierConfig[14] << Trigger_mode << endl;
  
  
  
  
  
  
  
  
  
  
  fichierConfig[12]<<fichierConfig[13].str()<<fichierConfig[14].str()                                                   ;
  fichierConfig[8] <<fichierConfig[9].str() <<fichierConfig[10].str()<<fichierConfig[11].str()<<fichierConfig[12].str() ;
  fichierConfig[4] <<fichierConfig[5].str() <<fichierConfig[6].str() <<fichierConfig[7].str() <<fichierConfig[8].str()  ;
  fichierConfigFile<<fichierConfig[1].str() <<fichierConfig[2].str() <<fichierConfig[3].str() <<fichierConfig[4].str()  ;

  
  
  // clear the streams
  for (int i=1;i<_ParaSize; i++)
  {
    fichierConfig[i].str("");
  }
  fichierConfigFile.close();
}
//####################################################################################################
// Load config in "STUCONFIG"
void myCmd::ReloadConfig_F (	
    int&            GetRawData    , 
    unsigned&       region        ,  
    unsigned short& L1_G_delay    , 
    int&            fw            ,
    short&          Trigger_mode  ,
    int&            G_A           ,
    int&            G_B           , 
    int&            G_C           ,	
    int&            G_A_mid       , 
    int&            G_B_mid       , 
    int&            G_C_mid       ,
    int&            G_A_low       , 
    int&            G_B_low       , 
    int&            G_C_low        
    )

{		     			     
  
  
  
  
  
  
  
  
  
  cout<<"entering in Reloading mode..."<< endl;

  string current_line;
  stringstream ss;
  string temp;
  // Chargement des parametres memebres de la classe
  fichier.open("STUCONFIG", ios::in);
  fichier >> 
    temp  >> G_A          >>  
    temp  >> G_B          >> 
    temp  >> G_C          >> 
    temp  >> G_A_mid      >> 
    temp  >> G_B_mid      >> 
    temp  >> G_C_mid      >> 
    temp  >> G_A_low      >> 
    temp  >> G_B_low      >> 
    temp  >> G_C_low      >> 
    temp  >> L1_G_delay   >> 
    temp  >> GetRawData   >> 
    temp  >> region       >> 
    temp  >> fw           >> 
    temp  >> Trigger_mode ; 

  
  
  
  
  
  
  
  
  
  fichier.close();
}
//####################################################################################################
void myCmd::commandHandler()
{
  status_char[0]='\0';
  fichierLog.open("log.txt", ios::app);

  DimCommand *currCmnd = getCommand();		

  try{
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////////ExitDIM/////////////
    if ( currCmnd == ExitDIM )
    {
      exit(1);
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////////Region test/////////////
    if ( currCmnd == en_L0 )
    {
      en_L0_int=en_L0->getInt();
      if (en_L0_int==1) {
        STU_API.SetRecordOnL0(true);
	
        status_string+="Record_on_L0 activated\n";
        strcpy(status_char, status_string.c_str());
        (*Service_Connect_Status).updateService(status_char);

        cout << status_string <<endl;
        fichierLog<<status_string << endl;

        status_string="";
      }	
      else{
        STU_API.SetRecordOnL0(false);

        status_string+="Record_on_L0 deactivated\n";
        strcpy(status_char, status_string.c_str());
        (*Service_Connect_Status).updateService(status_char);
        cout << status_string <<endl;
        fichierLog<<status_string << endl;

        status_string="";
      }
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////////Region test/////////////
    if ( currCmnd == L0_mask )
    {
      L0_mask_int=L0_mask->getInt();
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////////ReloadConfig/////////////
    if ( currCmnd == ReloadConfig )
    {
      ReloadConfig_F(
          GetRawData,
          region,
          L1_G_delay,
          fw, 
          Trigger_mode,
          G_A ,G_B ,G_C , 
          G_A_mid ,G_B_mid ,G_C_mid , 
          G_A_low ,G_B_low ,G_C_low 
          );
    
    
    
      cout << "G_A: "         << G_A        	<< endl;
      cout << "G_B: "         << G_B        	<< endl;
      cout << "G_C: "         << G_C        	<< endl;
      cout << "G_A_mid: "     << G_A_mid    	<< endl;
      cout << "G_B_mid: "     << G_B_mid    	<< endl;
      cout << "G_C_mid: "     << G_C_mid    	<< endl;
      cout << "G_A_low: "     << G_A_low    	<< endl;
      cout << "G_B_low: "     << G_B_low    	<< endl;
      cout << "G_C_low: "     << G_C_low    	<< endl;
      cout << "L1_G_delay: "  << L1_G_delay 	<< endl;
      cout << "GetRawData: "  << GetRawData 	<< endl;
      cout << "region: "      << region     	<< endl;
      cout << "fw: "          << fw         	<< endl;
      cout << "Trigger_mode: "<< Trigger_mode << endl;
    }
    
    
    
    
    
    
    
    
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////////Configure/////////////
    else if ( currCmnd == Configure )
    {  
      //time_t start = clock(); 
      time_t start,end;
      start = time(NULL);
      Conf_done=0;  
      (*Service_Conf_done).updateService(Conf_done);

      status_string= current_date_FileName()+"\n" + "Configure Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      ostringstream oss, oss1, oss_ID;

      //configure parameters
      // Cmd_struct * Configure_struc;
      // Configure_struc = (Cmd_struct * ) (currCmnd->getData());

      
      ifstream     CONFIGFILE;
      CONFIGFILE.open("STUCONFIG",ifstream::in);
      CONFIGFILE.clear();
      int i=0;
      unsigned int array[13];
      string line;
      if(CONFIGFILE.is_open()){
	while(getline(CONFIGFILE,line)) {
	  if(i < 11)
	    array[i] = atoi(line.c_str());
	  else {
	    stringstream ss;
	    ss << std::hex << line;
	    ss >> array[i];
	  }
	  printf("array[%d]=%u\n", i, array[i]);
	  i++;	  
	}
	CONFIGFILE.close();
      }
      Cmd_struct  * Configure_struc = (Cmd_struct *)malloc(sizeof( Cmd_struct ));
      //Configure_struc = (Cmd_struct * ) (currCmnd->getData());
      Configure_struc->G_A        = array[0 ];
      Configure_struc->G_B        = array[1 ];
      Configure_struc->G_C        = array[2 ];
      Configure_struc->G_A_mid    = array[3 ];
      Configure_struc->G_B_mid    = array[4 ];
      Configure_struc->G_C_mid    = array[5 ];
      Configure_struc->G_A_low    = array[6 ];
      Configure_struc->G_B_low    = array[7 ];
      Configure_struc->G_C_low    = array[8 ];
      Configure_struc->GetRawData = array[9 ];
      Configure_struc->L1_G_delay = (unsigned int)array[10];
      Configure_struc->region     = (unsigned int)array[11];
      Configure_struc->L0mask     = (unsigned int)array[12];

      L0_mask_int = (unsigned int)array[12];
      global_region = array[11];
     
      unsigned int maskL0;

      if(CalculateL0Mask(maskL0)) {
	L0_mask_int = maskL0;
	Configure_struc->L0mask = L0_mask_int;
	Configure_struc->region = L0_mask_int;
	global_region = L0_mask_int;
      }

      status_string="Configuration received values: ";	
      oss<<dec<<Configure_struc->G_A        ; status_string+="\nGamma_A: "        + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->G_B        ; status_string+="\nGamma_B: "        + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->G_C        ; status_string+="\nGamma_C: "        + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->G_A_mid    ; status_string+="\nGamma_A_mid: "    + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->G_B_mid    ; status_string+="\nGamma_B_mid: "    + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->G_C_mid    ; status_string+="\nGamma_C_mid: "    + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->G_A_low    ; status_string+="\nGamma_A_low: "    + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->G_B_low    ; status_string+="\nGamma_B_low: "    + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->G_C_low    ; status_string+="\nGamma_C_low: "    + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->L1_G_delay ; status_string+="\nL1_G: "           + oss.str(); oss.str("");
      oss<<dec<<Configure_struc->GetRawData ; status_string+="\nGet Raw Data: "   + oss.str(); oss.str("");
      oss<<hex<<Configure_struc->region     ; status_string+="\nRegion Enable: "  + oss.str(); oss.str("");
      oss<<hex<<Configure_struc->L0mask     ; status_string+="\nL0 mask: "        + oss.str(); oss.str("");
      
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<< current_date_FileName()<< "\n Command -> Configure\n"<< status_string << endl;

      //Reset Board
      STU_API.DCSResetBoard();
      STU_API.ACTELResetBoard();
      usleep(500000);
      STU_API.ResetTTConly();
      sleep(1); //can be test and optimized to be 1 sec long

      //Check IDCODES
	    STU_API.GetIDCODES(IDCODE0,IDCODE1);
      cout<<"IDCODE0 : "<< hex          << IDCODE0 << endl;
      cout<<"IDCODE1 : "<< oss_ID <<hex << IDCODE1 << endl;
      ID_FIRM=IDCODE1;
      (*Service_Firmware).updateService();	

      unsigned short  I2CData     ;
      bool            PLL_locked  ;
      bool            QPLL_error  ;
      bool            QPLL_locked ;
      bool            TTC_ready   ;
      bool            I2C_avail   ;

      //Read TTC
      STU_API.ReadTTC(_FINE_DELAY1,I2CData,PLL_locked,QPLL_error,QPLL_locked,TTC_ready,I2C_avail);
      I2CData_read=(int)I2CData;
      fichierLog<<"I2CData: " << hex << I2CData_read << endl;
      (*Service_I2CData).updateService();
      //Init of TTC mode
      STU_API.WriteI2CTTC(_CTRL_REG,0xF9);
      usleep(100000);
      STU_API.WriteI2CTTC(_FINE_DELAY1,I2CData);
      usleep(100000);
      status_string="STU_API.WriteI2CTTC\n";
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<<status_string << endl;	
      //Read TTC
      STU_API.ReadTTC(_FINE_DELAY1,I2CData,PLL_locked,QPLL_error,QPLL_locked,TTC_ready,I2C_avail);
      //usleep(500000);
      I2CData_read=(int)I2CData;
      fichierLog<<"I2CData: " << hex << I2CData_read << endl;
      (*Service_I2CData).updateService();

      //Set L0 Processing time
      STU_API.setL0ProcessingTime(_L0ProcessingTime);
      //usleep(500000);
      status_string=" STU_API.setL0ProcessingTime) OK\n";
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<<status_string << endl;
      
      //Region enable
      STU_API.RegionEnable(Configure_struc->region,L0_mask_int);
      //usleep(500000);
      region= Configure_struc->region;
      status_string=" STU_API.RegionEnable OK\n";	
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<<status_string << endl;
      
      STU_API.SetRecordOnL0(false);
      status_string="Record_on_L0 deactivated\n";
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<<status_string << endl;

      STU_API.SelTestPatternMode(false);

      //Opcode
      STU_API.setTriggerMode(Trigger_mode);
      //usleep(500000);

      // int     mask = 1      ;
      // int     PhaseOptValue ;
      // bool    PhasingDone   ; 
      // bool    CharDone      ;
      // bool    CharFailure   ; 
      // bool    RDY_DELAY     ;
      // time_t  seconds_bef   ;
      // time_t  seconds_aft   ;
      // ostringstream ossDeser,TRUsync,ossN, ossCTL;

      unsigned short  ctl_word=0  ;
      // unsigned int    memdelay  ;
      GetRawData = Configure_struc->GetRawData;
      if(Configure_struc->GetRawData==1) {
	  ctl_word=Enable_L0_over_TTC|GetRaw; //ici get raw si checkbox true
      } else { 
	ctl_word=Enable_L0_over_TTC;
      }
      STU_API.SetCTLWORD_static(ctl_word);
      /*
      unsigned short  ctl_word  ;
      unsigned int    memdelay  ;

      for (int add=0; add<nb_PORT; add++)
      {
		    PhasingDone =false;
		    CharDone    =false;

        //adress extraction
        mask=1<<add;
		
	      if(region & mask)
	      {
		      // ***UPDATE MODE***
		      if(Configure_struc->GetRawData==1)
		      {
		        ctl_word=(add<<6)|Enable_L0_over_TTC|GetRaw; //ici get raw si checkbox true
		        GetRawData= Configure_struc->GetRawData;
		      }
		      else 
          { 
			      ctl_word=(add<<6)|Enable_L0_over_TTC;
			      GetRawData= Configure_struc->GetRawData;
			    }
		      STU_API.SetCTLWORD_static(ctl_word);
		      //sleep(1);
		      
		      ossCTL<< "ctl_word:" << hex << ctl_word <<endl;
		      status_string=ossCTL.str();
		      ossCTL.str("");
		
		      // ***UPDATE PULSE***
		      STU_API.SendPulse(recompute_phase_bit);
		      //sleep(2);
		
		      seconds_bef = time(NULL);
		      seconds_aft = 0;
		
          while (!(PhasingDone && CharDone) && seconds_aft<2)
          {
            STU_API.GetDeserStatus(PhaseOptValue,PhasingDone,CharDone,CharFailure,RDY_DELAY,align);
            sleep(0.5);
            seconds_aft = time(NULL) - seconds_bef;
          }
          
					if(CharDone == 0 || CharFailure != 0 )cnt_packet_failure[add]++;
		      memdelay=STU_API.GetMemDelayImage();
          sleep(0.5);
	
          cout 
            << " memdelay 0x"     << hex  <<  setw(10)<< memdelay 
            << " PhaseOptValue: " << dec  <<  setw(3) << PhaseOptValue 
            << " PhasingDone: "   << PhasingDone 
            << " CharDone: "      << CharDone 
            << " CharFailure: "   << CharFailure 
            << " RDY_DELAY: "     << RDY_DELAY 
            << endl;

		      (TRU_sync.TRU)[add]=memdelay;
		      TRUsync	<<"sync TRU# "					<< dec << add 
									<<" done, memdelay 0x" 	<< hex << memdelay 
									<<" CharDone: "					<< CharDone 
									<<endl;
		      status_string+=TRUsync.str();
		      TRUsync.str("");
		      strcpy(status_char, status_string.c_str());
		      (*Service_Connect_Status).updateService(status_char);
		      cout << status_string <<endl;
		      fichierLog<<status_string << endl;
        }//if(region&mask)
      }//for (int add=0; add<nb_PORT; add++)
      sleep(1);

      (*Service_TRU_sync).updateService(p_TRU_sync, sizeof(Cmd_TRU));

      //Buffer for next evaluation of the error tab
      for(int i=0; i<nb_PORT; i++)
      {
        tableau_sync[i]    =0;
        tableau_sync_tmp[i]=0;
				cout<<"TRU#"<<dec<<i<<" sync failure : "<<cnt_packet_failure[i]<<endl;
      }
      */

      STU_API.SetGammaThresParameters(
          Configure_struc->G_A_low,
          Configure_struc->G_B_low,
          Configure_struc->G_C_low,
          Configure_struc->G_A_mid,
          Configure_struc->G_B_mid,
          Configure_struc->G_C_mid,
          Configure_struc->G_A,
          Configure_struc->G_B,
          Configure_struc->G_C
          );
      //usleep(500000);

      //	Gamma threshold on the board because didn't fail 
      // G_A     = Configure_struc->G_A;
      // G_B     = Configure_struc->G_B;
      // G_C     = Configure_struc->G_C;
      // G_A_mid = Configure_struc->G_A_mid;
      // G_B_mid = Configure_struc->G_B_mid;
      // G_C_mid = Configure_struc->G_C_mid;
      // G_A_low = Configure_struc->G_A_low;
      // G_B_low = Configure_struc->G_B_low;
      // G_C_low = Configure_struc->G_C_low;
     
      //Set L1 gamma and jets
      STU_API.setL1GammaDelay(
          Configure_struc->L1_G_delay
          );
      //usleep(500000);
      // L1_G_delay= Configure_struc->L1_G_delay;

      //TIN update
      // TIN_delay=L1_G_delay;
      
      // SetL0Delay from an *.ini file

      // fichier.open("STU_control.ini");
      // if( !fichier )// ce test �houe si le fichier n'est pas ouvert
      // {
      //   cout << "file STU_control.ini unfound\n no delay set!!!" << endl;
      // } 
      // else
      //   cout << "file STU_control.ini found" << endl;

      // //Get the Delay values from the STU_control.ini file
      // for(int i=0; i<nb_PORT; i++)
      // {
      //   do{
      //     getline( fichier, current_line );
      //     found=current_line.find("Delay=");
      //   } while(found==string::npos);
        
      //   delay_line= current_line;
      //   pos_eq=delay_line.find("=");
      //   delay_line.erase(0,pos_eq+1);

      //   istringstream iss(delay_line);
      //   iss>> int_delay[i];

      //   oss1<< "Delay " << i << " => " << int_delay[i];
      //   status_string=oss1.str();
      //   cout << status_string <<endl;
      //   oss1.str("");

      //   STU_API.SetL0Delay(i,int_delay[i]);

      //   strcpy(status_char, status_string.c_str());
      //   (*Service_Connect_Status).updateService(status_char);
      // }
      // fichier.close();
	

      status_string="SUCCESS -> STU Connection: Whole Configuration went through\n";	
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);

      cout << status_string <<endl;
      fichierLog<<status_string << endl;

      Conf_done=1;	
      (*Service_Conf_done).updateService(Conf_done);	

      free(Configure_struc);
      //time_t end = clock();
      end = time(NULL);
      //printf("Configure takes %f sec\n", (double)(end - start)/CLOCKS_PER_SEC);
      printf("Configure takes %d sec\n", (int)(end - start));

    }//else if ( currCmnd == Configure )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////GetConfiguration////////////////
	  else if ( currCmnd == GetConfiguration )
    {
      ostringstream oss;
      status_string= current_date_FileName()+"\n" + "GetConfiguration Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      oss << "current config:" << endl;
      oss << "G_A: "         << G_A        << endl;
      oss << "G_B: "         << G_B        << endl;
      oss << "G_C: "         << G_C        << endl;
      oss << "G_A_mid: "     << G_A_mid    << endl;
      oss << "G_B_mid: "     << G_B_mid    << endl;
      oss << "G_C_mid: "     << G_C_mid    << endl;
      oss << "G_A_low: "     << G_A_low    << endl;
      oss << "G_B_low: "     << G_B_low    << endl;
      oss << "G_C_low: "     << G_C_low    << endl;
      oss << "L1_G_delay: "  << L1_G_delay << endl;
      oss << "GetRawData: "  << GetRawData << endl;
      oss << "region: "      << region     << endl;
      oss << "fw: "          << fw         << endl;

    
    
    
    
    
    
    
    
    
      status_string=oss.str();

      GetConfig.G_A        = G_A        ;
      GetConfig.G_B        = G_B        ;
      GetConfig.G_C        = G_C        ;
      GetConfig.G_A_mid    = G_A_mid    ;
      GetConfig.G_B_mid    = G_B_mid    ;
      GetConfig.G_C_mid    = G_C_mid    ;
      GetConfig.G_A_low    = G_A_low    ;
      GetConfig.G_B_low    = G_B_low    ;
      GetConfig.G_C_low    = G_C_low    ;
      GetConfig.L1_G_delay = L1_G_delay ;
      GetConfig.GetRawData = GetRawData ;
      GetConfig.region     = region     ;

      
      
      
      
      
      
      
      
      
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<< current_date_FileName()<< status_string << endl;

      (*Service_GetConfig).updateService(&GetConfig, sizeof(Cmd_struct));
    }//else if ( currCmnd == GetConfiguration )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////////////SOR_number/////////
    else if ( currCmnd == SOR_number )
    {
      ostringstream oss;
      SOR_nb_int= SOR_number ->getInt();
      oss << dec << SOR_nb_int;
      SOR_nb_str= oss.str();
    }//else if ( currCmnd == SOR_number )
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////////////SOR///////////////
    else if ( currCmnd == SOR )
    {
#ifdef _SLOW_DIM
      cout <<"Delay SOR started" << endl;
      sleep(_STEP_DURATION);
#endif
      ostringstream ossSOR, oss1,oss;
      
      time_t sor_time1;
      sor_time1=time(NULL);

      string fileName="log_"+ SOR_nb_str;
      fichierRun.open(fileName.c_str(), ios::out);

      TRU_SyncAlarm=0;
      //Resest Error Count tab

      for(int i=0; i<nb_PORT; i++){
        tableau_sync[i]=0;
      }

      string result;
      ifstream fichierdiff;
      string diff_line;
      ostringstream bash_command, bash_command2, bash_command3, TRUsync,ossCTL;

      //get the region enable computed in PVSS from TRU mask and RCU active list
      region= SOR->getInt();

      STU_API.DCSResetBoard();
      STU_API.ACTELResetBoard();
      sleep(1); //can be test and optimized to be 1 sec long

      STU_API.setL0ProcessingTime(_L0ProcessingTime);
      status_string+=" setL0ProcessingTime done\nNow configuring registers";

      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<<status_string << endl;
      ossSOR<<"Run about to start with configuration:"<<endl;

      ossSOR<<"G_A: "       << dec << G_A       <<endl;
      ossSOR<<"G_B: "       << dec << G_B       <<endl;
      ossSOR<<"G_C: "       << dec << G_C       <<endl;
      ossSOR<<"G_A_mid: "   << dec << G_A_mid   <<endl;
      ossSOR<<"G_B_mid: "   << dec << G_B_mid   <<endl;
      ossSOR<<"G_C_mid: "   << dec << G_C_mid   <<endl;
      ossSOR<<"G_A_low: "   << dec << G_A_low   <<endl;
      ossSOR<<"G_B_low: "   << dec << G_B_low   <<endl;
      ossSOR<<"G_C_low: "   << dec << G_C_low   <<endl;
      ossSOR<<"GetRawData: "<< dec << GetRawData<<endl;
      
      
      
      
      
      
      
      
      status_string=ossSOR.str();

      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<< current_date_FileName()<< status_string << endl;

      ////////////////////////////////////////////////////////
      /// HIGH en premier apparamment!!!!!!!!!!!!!!!!
      STU_API.SetGammaThresParameters(G_A_low ,G_B_low ,G_C_low,G_A_mid ,G_B_mid ,G_C_mid,G_A ,G_B ,G_C );
      STU_API.setL1GammaDelay(        L1_G_delay                              );
      
      
      
      
      ////////////////////////////////////////////////////////

      STU_API.RegionEnable(region, L0_mask_int);
      ossSOR<<"Region Enable set to:"<< hex << region<<"L0_mask: " << hex << L0_mask_int << endl;

      status_string=ossSOR.str();	
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<< current_date_FileName()<< status_string << endl;

      // write_STU_CONFIG(
      //     GetRawData,
      //     region,L1_G_delay,
      //     fw, 
      //     Trigger_mode,
      //     G_A ,G_B ,G_C ,
      //     G_A_mid ,G_B_mid ,G_C_mid ,
      //     G_A_low ,G_B_low ,G_C_low 
      //     );



      ossSOR<<"SOR: 2nd Part...now starting synchro for run "<< SOR_nb_str <<endl;
      status_string=ossSOR.str();

      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<< current_date_FileName()<< status_string << endl;

      //Check the difference between STUCONFIG and STUCONFIG_written
      system("diff STUCONFIG STUCONFIG_written > diff.log");

      fichierdiff.open("diff.log", ios::in);
      getline(fichierdiff, diff_line);

      if( diff_line!="" )
      { 
        status_string="New_Configuration_received: ";	
        oss<<dec<<G_A       ; status_string+="\nGamma_A: "        + oss.str(); oss.str("");
        oss<<dec<<G_B       ; status_string+="\nGamma_B: "        + oss.str(); oss.str("");
        oss<<dec<<G_C       ; status_string+="\nGamma_C: "        + oss.str(); oss.str("");
        oss<<hex<<G_A_mid   ; status_string+="\nGamma_A_mid: "    + oss.str(); oss.str("");
        oss<<hex<<G_B_mid   ; status_string+="\nGamma_B_mid: "    + oss.str(); oss.str("");
        oss<<hex<<G_C_mid   ; status_string+="\nGamma_C_mid: "    + oss.str(); oss.str("");
        oss<<hex<<G_A_low   ; status_string+="\nGamma_A_low: "    + oss.str(); oss.str("");
        oss<<hex<<G_B_low   ; status_string+="\nGamma_B_low: "    + oss.str(); oss.str("");
        oss<<hex<<G_C_low   ; status_string+="\nGamma_C_low: "    + oss.str(); oss.str("");
        oss<<dec<<L1_G_delay; status_string+="\nL1_G: "           + oss.str(); oss.str("");
        oss<<dec<<GetRawData; status_string+="\nGet Raw Data: "   + oss.str(); oss.str("");
        oss<<hex<<region    ; status_string+="\nRegion Enable: "  + oss.str();
        fichierLog<< current_date_FileName()<< status_string << endl;
      }//if( diff_line!="" ) 
      
      
      
      
      
      
      
      
      
      status_string="";

      int     mask = 1      ;
      int     PhaseOptValue ;
      bool    PhasingDone   ; 
      bool    CharDone      ;
      bool    CharFailure   ; 
      bool    RDY_DELAY     ;
      time_t  seconds_bef   ;
      time_t  seconds_aft   ;
      
      unsigned short  ctl_word  ;
      unsigned short  readval   ;
      unsigned int    memdelay  ;

      //get time 
      time_t sec1;
      sec1=time(NULL);
      int add;
      
      // clear error counts
      for(int i=0; i<nb_PORT; i++){
        tableau_sync[i]=0;
      }

      for (int dec_add=_FIRST_TRU_SYNC; dec_add<(nb_PORT+_FIRST_TRU_SYNC); dec_add++)
      {	
        if (dec_add>=nb_PORT)
          add=dec_add-nb_PORT;
        else
          add=dec_add;

        PhasingDone =false;
        CharDone    =false;

        //adress extraction
        mask=1<<add;
		
	      if(region & mask)
        {
          // ***UPDATE MODE***
          if(GetRawData==1)
          {
            ctl_word=(add<<6)|Enable_L0_over_TTC|GetRaw; //ici get raw si checkbox true
          }
          else {
            ctl_word=(add<<6)|Enable_L0_over_TTC;
          }
		
		      STU_API.SetCTLWORD_static(ctl_word);
		      STU_API.CheckCTLWORD_static(readval);
		      
		      // ***UPDATE PULSE***
		      STU_API.SendPulse(recompute_phase_bit);
		
		      seconds_bef = time(NULL);
		      seconds_aft = 0;
		
          while (!(PhasingDone && CharDone) && seconds_aft<3) 
          {	
            STU_API.GetDeserStatus(PhaseOptValue,PhasingDone,CharDone,CharFailure,RDY_DELAY,align);
            sleep(0.5);
            seconds_aft = time(NULL) - seconds_bef;
          }
          //sleep(3);
          STU_API.GetDeserStatus(PhaseOptValue,PhasingDone,CharDone,CharFailure,RDY_DELAY,align);
          memdelay=STU_API.GetMemDelayImage();
          
          if (!PhasingDone)
            (TRU_sync.TRU)[add]=memdelay;
         
          TRUsync<< "sync TRU# "<< dec << add <<" done, memdelay 0x" << hex << memdelay ;
          TRUsync << dec 
                  << " TapVal: "      << PhaseOptValue 
                  << " PhasingDone: " << PhasingDone 
                  << " CharDone: "    << CharDone 
                  << " CharFailure: " << CharFailure 
                  << " Align: "       << align 
                  << endl;

          //test
          if (!PhasingDone){
            usleep(500000);
            STU_API.GetDeserStatus(PhaseOptValue,PhasingDone,CharDone,CharFailure,RDY_DELAY,align);
            TRUsync<< "2nd Try: sync TRU# "<< dec << add <<" done, memdelay 0x" << hex << memdelay ;
            TRUsync << dec 
                    << " TapVal: "      << PhaseOptValue 
                    << " PhasingDone: " << PhasingDone 
                    << " CharDone: "    << CharDone 
                    << " CharFailure: " << CharFailure 
                    << " Align:"        << align 
                    << endl;
          }

          status_string+=TRUsync.str();
          TRUsync.str("");
        }//if(region & mask)

        //Buffer for next evaluation of the error tab
        for(int i=0; i<nb_PORT; i++)
        {
          tableau_sync[i]     =0;
          tableau_sync_tmp[i] =0;
        }

      }//for (int dec_add=_FIRST_TRU_SYNC; dec_add<(32+_FIRST_TRU_SYNC); dec_add++)

#ifdef _SLOW_DIM
      cout <<"Sync part done, sleep now" << endl;
      sleep(_STEP_DURATION);
#endif

      STU_API.CheckCTLWORD_static(readval);
      fichierLog<< "ctl_word, bef PAT_MEM_TEST:" << hex << " readval=" <<readval << endl;

      if (en_mem_pat==1) 
      {
        STU_API.load_test_memory("Pattern.txt");
        STU_API.SelTestPatternMode(true);

        STU_API.CheckCTLWORD_static(readval);
        fichierLog<< "ctl_word, aft PAT_MEM_TEST:" << hex << " readval=" <<readval << endl;

        status_string+="PAT_MEM_TEST \n";
        strcpy(status_char, status_string.c_str());
        (*Service_Connect_Status).updateService(status_char);
        cout << status_string <<endl;
        fichierLog<<status_string << endl;

        status_string="";

#ifdef _SLOW_DIM
        sleep(_STEP_DURATION);
#endif
      }//if (en_mem_pat==1)
	
      if (en_L0_int==1) {
        STU_API.SetRecordOnL0(true);

        status_string+="Record_on_L0 activated\n";
        strcpy(status_char, status_string.c_str());
        (*Service_Connect_Status).updateService(status_char);
        cout << status_string <<endl;
        fichierLog<<status_string << endl;

        status_string="";
      }	
      else{
        STU_API.SetRecordOnL0(false);

        status_string+="Record_on_L0 deactivated\n";
        strcpy(status_char, status_string.c_str());
        (*Service_Connect_Status).updateService(status_char);
        cout << status_string <<endl;
        fichierLog<<status_string << endl;

        status_string="";
      }	
	
#ifdef _SLOW_DIM
      sleep(_STEP_DURATION);
#endif

      //get time
      time_t sec2;
      sec2=time (NULL);

      status_string+="STU synchro done\n";
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;

      fichierRun<< status_string << current_date_FileName()<<  endl;
      fichierRun.close();

      fichierLog<<  status_string << current_date_FileName()<<  endl;
      fichierLog<< "Synchro_time: " << sec2-sec1 << "s" <<endl;
      fichierLog<< "Total SOR time: " << sec2-sor_time1 << "s" <<endl;

      //TRU Service update for archiving
      (*Service_TRU_sync).updateService(p_TRU_sync, sizeof(Cmd_TRU));

      ////FOR ERROR COUNT tests
      
      //unsigned short ret_value;
      //unsigned short current_error;
      ////STU_API.GetErrorCount(tableau_sync_tmp);
      ////cout<<"error count passed"<<endl;
      ////fichierLog<<"error count passed"<<endl;
      
      //for(int i=0; i<32; i++){
      //	if(GetRawData==1)
      //	{
      //		ctl_word=(i<<6)|Enable_L0_over_TTC|GetRaw | ((en_mem_pat)?1:0); //ici get raw si checkbox true
      // 	}
      //	else {
      //	  ctl_word=(i<<6)|Enable_L0_over_TTC | ((en_mem_pat)?1:0);
      //	}
      //	STU_API.SetCTLWORD_static(ctl_word);
      //	STU_API.CheckCTLWORD_static(ret_value);
      //	
      //	current_error=STU_API.ReadErrorCount();
      //	tableau_sync_tmp[i]=current_error;
      //	
      //	(TRU_sync_error.TRU)[i]=current_error;
      //
      //  //STU_API.CheckCTLWORD_static(ret_value);
      //  cout << dec << "TRU sync error #i"<< i << " error count "<< current_error  <<endl;
      //  fichierLog<< dec<< "TRU sync error #i="<< i << " error count="<< current_error<<endl;
      //  if(abs_errors_counts(tableau_sync[i],tableau_sync_tmp[i]))
      //  {
      //    //We tell PVSS which TRU sync has an error
      //    TRU_SyncAlarm=i;
      //    (*Service_SyncAlarm).updateService();
      //    cout << "TRU #"<< TRU_SyncAlarm << " has a sync problem because --error count bef= "<<tableau_sync[i]<< " --error count  after: " << tableau_sync_tmp[i]    << endl;
      //    fichierLog<< "TRU #"<< TRU_SyncAlarm << " has a sync problem because --error count bef= "<<tableau_sync[i]<< " --error count  after: " << tableau_sync_tmp[i]    << endl;
      //  }
      //}

      SOR_Done=1;

      //acknowledge
      (*Service_SOR_Done).updateService();
    }//else if ( currCmnd == SOR )
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////////////SOR_sync (only)///////////////
    else if ( currCmnd == SOR_sync )
    {
      //time_t start = clock();
      time_t start, end;
      start = time(NULL);
      SOR_Done=0;
      //acknowledge
      (*Service_SOR_Done).updateService();

      ostringstream ossSOR, oss1,oss;
      ostringstream bash_command, bash_command2, bash_command3, TRUsync,ossCTL;

      //get the region enable computed in PVSS from TRU mask and RCU active list
      int region_sync= global_region;//SOR_sync->getInt();

      int     mask = 1      ;
      int     PhaseOptValue ;
      bool    PhasingDone   ; 
      bool    CharDone      ;
      bool    CharFailure   ; 
      bool    RDY_DELAY     ;
      time_t  seconds_bef   ;
      time_t  seconds_aft   ;

      unsigned short  ctl_word  ;
      unsigned short  readval   ;
      unsigned int    memdelay  ;
      status_string="";

      int add;

      for (int dec_add=_FIRST_TRU_SYNC; dec_add<(nb_PORT+_FIRST_TRU_SYNC); dec_add++)
      {	
        if (dec_add>=nb_PORT)
          add=dec_add-nb_PORT;
        else
          add=dec_add;

        //adress extraction
        mask=1<<add;

        if(region_sync & mask)
        {
          // ***UPDATE MODE***
          if(GetRawData==1)
          {
            ctl_word=(add<<6)|Enable_L0_over_TTC|GetRaw | ((en_mem_pat)?1:0); //ici get raw si checkbox true
          }
          else {
            ctl_word=(add<<6)|Enable_L0_over_TTC |((en_mem_pat)?1:0);
          }
          STU_API.SetCTLWORD_static(ctl_word);
	  usleep(10000);
          //STU_API.CheckCTLWORD_static(readval);
          // ***UPDATE PULSE***
          STU_API.SendPulse(recompute_phase_bit);
	}
      }
      usleep(1000000);

      for (int dec_add=_FIRST_TRU_SYNC; dec_add<(nb_PORT+_FIRST_TRU_SYNC); dec_add++)
      {	
        if (dec_add>=nb_PORT)
          add=dec_add-nb_PORT;
        else
          add=dec_add;

        PhasingDone=false;
        CharDone=false;

        //adress extraction
        mask=1<<add;

        if(region_sync & mask)
        {
          // ***UPDATE MODE***
          if(GetRawData==1)
          {
            ctl_word=(add<<6)|Enable_L0_over_TTC|GetRaw | ((en_mem_pat)?1:0); //ici get raw si checkbox true
          }
          else {
            ctl_word=(add<<6)|Enable_L0_over_TTC |((en_mem_pat)?1:0);
          }

          STU_API.SetCTLWORD_static(ctl_word);
          STU_API.CheckCTLWORD_static(readval);

          // ***UPDATE PULSE***
          //STU_API.SendPulse(recompute_phase_bit);
	  total_trial[dec_add]=0;
	  total_error[dec_add]=0;
	  isFirstTrial[dec_add]=0;

          seconds_bef = time(NULL);
          seconds_aft = 0;
	  int synccnt = 0;
          while (!(PhasingDone && CharDone) && seconds_aft<3) 
          {	
            STU_API.GetDeserStatus(PhaseOptValue,PhasingDone,CharDone,CharFailure,RDY_DELAY,align);
            //sleep(0.5);
	    //usleep(100000);
	    synccnt++; 
            seconds_aft = time(NULL) - seconds_bef;
          }
	  if(!(synccnt==1)) printf("WARNING: Sleep time is not optimized\n");
          memdelay=STU_API.GetMemDelayImage();
          (TRU_sync.TRU)[add]=memdelay;

          TRUsync<< "sync TRU# "<< dec << add <<" done, memdelay 0x" << hex << memdelay ;
          TRUsync << dec 
                  << " TapVal: "      << PhaseOptValue 
                  << " PhasingDone: " << PhasingDone 
                  << " CharDone: "    << CharDone 
                  << " CharFailure: " << CharFailure 
                  << " Align: "       << align 
                  <<endl;
          status_string+=TRUsync.str();
          TRUsync.str("");
        }//if(region_sync & mask)
      }//for (int dec_add=_FIRST_TRU_SYNC; dec_add<(nb_PORT+_FIRST_TRU_SYNC); dec_add++)
      
      status_string+="STU synchro done\n";
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;

      fichierLog<<  status_string << current_date_FileName()<<  endl;


      SOR_Done=1;

      //acknowledge
      (*Service_SOR_Done).updateService();
      //time_t end = clock();
      end = time(NULL);
      //printf("SOR_sync takes %f sec\n", (double)(end - start)/CLOCKS_PER_SEC);
      printf("SOR_sync takes %d sec\n", (int)(end - start));
    }//else if ( currCmnd == SOR_sync )
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////////////EN_MEM_PAT_TST///////////////
	  else if ( currCmnd == EN_MEM_PAT_TST )
    {
      en_mem_pat=EN_MEM_PAT_TST->getInt();

      status_string= current_date_FileName() + "EN_MEM_PAT_TST Command Received:";

      fichierLog<< DimClientName() << status_string <<en_mem_pat <<  endl;
    }//else if ( currCmnd == EN_MEM_PAT_TST )
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////////////EOR///////////////
    else if ( currCmnd == EOR )
    {
      TRU_SyncAlarm=0;
      //Reset Error Count tab
      for(int i=0; i<nb_PORT; i++){
        tableau_sync[i]=0;
      }
      status_string= current_date_FileName()+"\n" + "EOR Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;
    }//else if ( currCmnd == EOR )
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////////////ErrorCount///////////////
    else if ( currCmnd == ErrorCount )
    {
      status_string= current_date_FileName()+"\n" + "ErrorCount Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;
      int error_val= ErrorCount->getInt();

      cout << "TRU ErrorCount request" << endl;
      fichierLog<< "TRU ErrorCount request"<<endl;

      unsigned short ret_value;
      unsigned short ctl_word;
      unsigned short current_error;
      //STU_API.GetErrorCount(tableau_sync_tmp);
      //cout<<"error count passed"<<endl;
      //fichierLog<<"error count passed"<<endl;

      for(int i=0; i<nb_PORT; i++){
        if(GetRawData==1)
        {
          // ctl_word=(i<<6)|Enable_L0_over_TTC|GetRaw | ((en_mem_pat)?1:0); //ici get raw si checkbox true
	  ctl_word=(PhaseManuVal<<1)|(i<<6)|Enable_L0_over_TTC|GetRaw | ((en_mem_pat)?1:0); //ici get raw si checkbox true
        }
        else {
          ctl_word=(PhaseManuVal<<1)|(i<<6)|Enable_L0_over_TTC | ((en_mem_pat)?1:0);
        }

        STU_API.SetCTLWORD_static(ctl_word);
        STU_API.CheckCTLWORD_static(ret_value);

        current_error=STU_API.ReadErrorCount();
        tableau_sync_tmp[i]=current_error;

        (TRU_sync_error.TRU)[i]=current_error;

        if (error_val==2)
        {
          //STU_API.CheckCTLWORD_static(ret_value);
          cout << dec << "TRU sync error #i"<< i << " error count "<< current_error  <<endl;
          fichierLog<< dec<< "TRU sync error #i="<< i << " error count="<< current_error<<endl;
        } 
  
        if(abs_errors_counts(tableau_sync[i],tableau_sync_tmp[i]) && (tableau_sync[i]% 4097) )
        {
          //We tell PVSS which TRU sync has an error
          TRU_SyncAlarm=i;
          (*Service_SyncAlarm).updateService();

          cout << "TRU #"<< TRU_SyncAlarm << " has a sync problem because --error count bef= "<<tableau_sync[i]<< " --error count  after: " << tableau_sync_tmp[i]    << endl;
          fichierLog<< "TRU #"<< TRU_SyncAlarm << " has a sync problem because --error count bef= "<<tableau_sync[i]<< " --error count  after: " << tableau_sync_tmp[i]    << endl;
        }
      }

      (*Service_TRU_sync_error).updateService(p_TRU_sync_error, sizeof(Cmd_TRU));

      //Buffer for next evaluation of the error tab
      for(int i=0; i<nb_PORT; i++)
      {
        tableau_sync[i]=tableau_sync_tmp[i];
      }
    }//else if ( currCmnd == ErrorCount )
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////////////ConfFPGA///////////////
    else if ( currCmnd == MonErrorCount )
    {
      // Cmd_MonError * MonError;
      // MonError = (Cmd_MonError * ) (currCmnd->getData());
      status_string= current_date_FileName()+"\n" + "MonErrorCount Command Received\n" + DimClientName();
      string tmp_date_string=current_date_FileName();
      fichierLog<< status_string << endl;

      cout << "TRU/STU MonErrorCount request" << endl;
      fichierLog<< "TRU/STU MonErrorCount request"<<endl;
      cout << "Current date " << current_date_FileName() << endl;

      int RangeMin = 0;
      int RangeMax = 27;

      int     mask = 1      ;
      int     add;
      unsigned short ret_value;
      unsigned short ctl_word;
      unsigned int   current_trial[nb_PORT]={};
      unsigned int   current_error[nb_PORT]={};
      double         current_err_rate=0.;
      int            read_err[nb_PORT]={};

      int     PhaseOptValue ;
      bool    PhasingDone   ; 
      bool    CharDone      ;
      bool    CharFailure   ; 
      bool    RDY_DELAY     ;

      for(int i=RangeMin; i<=RangeMax; i++){
	add=i;

        //adress extraction
        mask=1<<add;

	if((region & mask)){
	  if(GetRawData==1)
	    {
	      ctl_word=(PhaseManuVal<<1)|(i<<6)|Enable_L0_over_TTC|GetRaw | ((en_mem_pat)?1:0); //ici get raw si checkbox true
	    }
	  else {
	    ctl_word=(PhaseManuVal<<1)|(i<<6)|Enable_L0_over_TTC | ((en_mem_pat)?1:0);
	  }

	  STU_API.SetCTLWORD_static(ctl_word);
	  STU_API.CheckCTLWORD_static(ret_value);

	  if(!STU_API.ReadMonErrorCount(current_trial[i], current_error[i])){
	    cout << "Error: please reset the error counter for TRU/STU #" << i << endl;
	    cout << "current_trial[" << i << "]: " << current_trial[i] << "; current_error[" << i << "]: " << current_error[i] << endl;           
            read_err[i]=1;
	    continue;
	  }
	  if(!isFirstTrial[i]){
            tmp_date_string=current_date_FileName();
	    date_string_for_sync[i] = tmp_date_string;
	  }
	  read_err[i]=0;
	}
      }

      for(int i=RangeMin; i<=RangeMax; i++){

          add=i;

        //adress extraction
        mask=1<<add;

	if((region & mask)){
	  if(GetRawData==1)
	    ctl_word=(i<<6)|Enable_L0_over_TTC|GetRaw | ((en_mem_pat)?1:0); //ici get raw si checkbox true
	  else 
	    ctl_word=(i<<6)|Enable_L0_over_TTC | ((en_mem_pat)?1:0);
	  
	  STU_API.SetCTLWORD_static(ctl_word);
	  STU_API.CheckCTLWORD_static(ret_value);
          STU_API.GetDeserStatus(PhaseOptValue,PhasingDone,CharDone,CharFailure,RDY_DELAY,align);

	  current_err_rate = (double)current_error[i]/(double)current_trial[i];
	  printf("%2d> Current Trial: %u Current Error: %u Current err rate: %e\n\n", 
		 i, current_trial[i], current_error[i], current_err_rate);
	  fichierLog << "TRU/STU sync error #i="<< i << " current trial count=" << current_trial[i] << " current error count="<< current_error[i] << " current error rate=" << current_err_rate << endl;

	  total_trial[i] += current_trial[i];
	  total_error[i] += current_error[i];
	  if(isFirstTrial[i] && !read_err[i]){
	    total_err_rate[i] = (double)total_error[i]/(double)total_trial[i]; 
            cout << "Total error rate from " << date_string_for_sync[i] << endl;
	    printf("%2d> Total Trial: %llu Total Error: %llu Total err rate: %e\n\n",
		   i, total_trial[i], total_error[i], total_err_rate[i]);
	    fichierLog << "Total error rate from " << date_string_for_sync[i] << endl;
	    fichierLog << "TRU/STU total err rate #i="<< i << " total trial count=" << total_trial[i] << " total error count="<< total_error[i] << " total error rate=" << total_err_rate[i] << endl << endl;

	  }
	  if(!isFirstTrial[i] && CharDone)
	    isFirstTrial[i]=1;
	}
      }
    }//else if ( currCmnd == MonErrorCount )
    else if ( currCmnd == ConfFPGA )
    {
      ostringstream oss1, oss2;

      status_string= current_date_FileName()+"\n" + "ConfFPGA Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      //Config FPGA
      string FPGAFile="STU_FPGA.mcs";
      STU_API.SelectmapConfFPGA(FPGAFile);
      sleep(5);  

      ConfFPGA_Done=1;
      (*Service_ConfFPGA_Done).updateService(ConfFPGA_Done);
      ConfFPGA_Done=0;

      sleep(10);	

      status_string+="STU ConfFPGA done!\nChecking ID CODES...\n";

      //Check IDCODES

      STU_API.GetIDCODES(IDCODE0,IDCODE1);

      oss1<<"IDCODE0 : "<< hex << IDCODE0;
      oss2<<"IDCODE1 : "<< hex << IDCODE1;
      status_string+="\nIDCODE0 : " + oss1.str();
      status_string+="\nIDCODE1 : " + oss2.str();

      (*Service_Firmware).updateService();	

      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog<<status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ConfFPGA )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////GetI2CData////////////////
    else if ( currCmnd == GetI2CData )
    {
      I2CData_pvss=GetI2CData->getInt();

      status_string= current_date_FileName()+"\n" + "Finedelay1 changed" + DimClientName();
      unsigned short I2CData;
      bool PLL_locked;
      bool QPLL_error;
      bool QPLL_locked;
      bool TTC_ready;
      bool I2C_avail;

      //Init of TTC mode
      STU_API.WriteI2CTTC(_CTRL_REG,0xF3);
      STU_API.WriteI2CTTC(_FINE_DELAY1,I2CData_pvss);
      status_string="STU_API.WriteI2CTTC\n";
      strcpy(status_char, status_string.c_str());
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
      fichierLog<<status_string << endl;	

      STU_API.ReadTTC(_FINE_DELAY1,I2CData,PLL_locked,QPLL_error,QPLL_locked,TTC_ready,I2C_avail);

      I2CData_read=(int)I2CData;
      fichierLog<<"I2CData: " << hex << I2CData_read << endl;
      (*Service_I2CData).updateService();

      fichierLog<< status_string << endl;
    }//else if ( currCmnd == GetI2CData )
      
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////////ResetBoard/////////////
    else if ( currCmnd == ResetBoard )
    {
      int reset_val= ResetBoard->getInt();
      if (reset_val==2){
        status_string= current_date_FileName()+"\n" + "SOR: 1st Part" + DimClientName();
        fichierLog<< status_string << endl;
      } 
      else{
        status_string= current_date_FileName()+"\n" + "ResetBoard Command Received\n" + DimClientName();
        fichierLog<< status_string << endl;
      }

      STU_API.ResetTTConly();
      sleep(1);
      status_string="TTC Reset done\n";
      strcpy(status_char, status_string.c_str());
      cout << status_string <<endl;
      fichierLog<<status_string << endl;	

#ifdef _SLOW_DIM
      sleep(_STEP_DURATION);
#endif
      STU_API.WriteI2CTTC(_CTRL_REG,0xF3);
      STU_API.WriteI2CTTC(_FINE_DELAY1,I2CData_pvss);

      status_string="WriteI2C/TTC done - SOR 1st part finished";
      strcpy(status_char, status_string.c_str());
      //(*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;

      unsigned short I2CData;
      bool PLL_locked;
      bool QPLL_error;
      bool QPLL_locked;
      bool TTC_ready;
      bool I2C_avail;

      STU_API.ReadTTC(_FINE_DELAY1,I2CData,PLL_locked,QPLL_error,QPLL_locked,TTC_ready,I2C_avail);

      I2CData_read=(int)I2CData;
      fichierLog<<"I2CData: " << hex << I2CData_read << endl;
      (*Service_I2CData).updateService();

      //acknowledge
      ResetFPGA_Done=1;
      (*Service_ResetFPGA_Done).updateService();

      status_string="\nSOR 1st part done: ";	
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      strcpy(status_char, (status_string.c_str()));
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ResetBoard )
	
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////ReadIDCODEs/////////////////
    else if ( currCmnd == ReadIDCODEs )
    {
      status_string= current_date_FileName()+"\n" + "ReadIDCODEs Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      ostringstream oss, oss1;

      //Check IDCODES
      STU_API.GetIDCODES(IDCODE0,IDCODE1);

      oss<<"IDCODE0 : "<< hex << IDCODE0;
      oss1<<"IDCODE1 : "<< hex << IDCODE1;

      status_string="\nIDCODE0 : " + oss.str();
      status_string+="\nIDCODE1 : " + oss1.str();

      (*Service_Firmware).updateService();

      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog<< status_string << endl;
      (*Service_Connect_Status).updateService(status_char);

    }//else if ( currCmnd == ReadIDCODEs )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////ReadMonitor/////////////////
    if ( currCmnd == ReadMonitor )
    {
      status_string= current_date_FileName()+"\n" + "ReadMonitor Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      catch_string="ReadMonitor: ";
      fichierLog<< current_date_FileName()<< "\n Command -> ReadMonitor\n"<< status_string << endl;
      double temp[3];
      double vccint[3];
      double vccaux[3];
      ostringstream oss;

      STU_API.GetMonitorStatus(temp,vccint,vccaux);

      status_string="Read Monitor: ";	
      status_string+="temp min =";
      oss<<temp[2];
      status_string+= oss.str();
      oss.str("");
      status_string+= " C;\ttemp nom=";
      oss<<temp[0];
      status_string+= oss.str();
      oss.str("");
      status_string+= " C;\ttemp max=";
      oss<<temp[1];
      status_string+= oss.str();
      oss.str("");
      status_string+= " C";
      status_string+= " \n";

      status_string+= "vccint min =";
      oss<<vccint[2];
      status_string+= oss.str();
      oss.str("");
      status_string+=" V;\tvccint nom=";
      oss<<vccint[0];
      status_string+= oss.str();
      oss.str("");
      status_string+=" V;\tvccint max=";
      oss<<vccint[1];
      status_string+= oss.str();
      oss.str("");
      status_string+=" V";
      status_string+= " \n";

      status_string+="vccaux min =";
      oss<<vccaux[2];
      status_string+= oss.str();
      oss.str("");
      status_string+=" V;\tvccaux nom=";
      oss<<vccaux[0];
      status_string+= oss.str();
      oss.str("");
      status_string+=" V;\tvccaux max=";
      oss<<vccaux[1];
      status_string+= oss.str();
      oss.str("");
      status_string+=" V";
      status_string+= " \n";

      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog<< status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//if ( currCmnd == ReadMonitor )

    //else
    ////////////////GetMonitorValues/////////////////
    //if ( currCmnd == GetMonitorValues )
    //{
    //// to be finished if used
    //  status_string= current_date_FileName()+"\n" + "ReadMonitor Command Received\n" + DimClientName();
    //  fichierLog<< status_string << endl;
    //
    //  double temp[3];
    //  double vccint[3];
    //  double vccaux[3];
    //  fichierLog<< current_date_FileName()<< "\n Command -> GetMonitorValues\n"<< status_string << endl;
    //  
    //  STU_API.GetMonitorStatus(temp,vccint,vccaux);
    //  (*Service_Temperature_Val).updateService(temperature);
    //}

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ///////GetDDLStatus////////////////////////
    else if ( currCmnd == GetDDLStatus )
    {
      status_string= current_date_FileName()+"\n" + "GetDDLStatus Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      bool RDYRX;
      bool EOBTR;
      short transactionID;
      unsigned int DDL_WORD;
      ostringstream oss;
      fichierLog<< current_date_FileName()<< "\n Command -> GetDDLStatus\n"<< status_string << endl;

      STU_API.GetDDLStatus(RDYRX,EOBTR,transactionID);

      status_string="Check DDL status -> ";
      if(RDYRX){ status_string+="RDYRX=true ";} else status_string+="RDYRX=false ";
      if(EOBTR){ status_string+="EOBTR=true ";} else status_string+="EOBTR=false ";

      oss<<hex<<transactionID;
      status_string+="TransID=0x"+oss.str();
      oss.str("");

      STU_API.GetDDLWORD(DDL_WORD);
      oss<<hex<<DDL_WORD;
      status_string+="\tDDL_WORD=0x"+oss.str();
      oss.str("");

      strcpy(status_char, (status_string.c_str()));
      fichierLog<< status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
      cout << status_string <<endl;
    }//else if ( currCmnd == GetDDLStatus )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////////ReadTTC///////////////
    else if ( currCmnd == ReadTTC )
    {
      status_string= current_date_FileName()+"\n" + "ReadTTC Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      unsigned short I2CData;
      bool PLL_locked;
      bool QPLL_error;
      bool QPLL_locked;
      bool TTC_ready;
      bool I2C_avail;
      ostringstream oss;

      STU_API.ReadTTC(_CTRL_REG,I2CData,PLL_locked,QPLL_error,QPLL_locked,TTC_ready,I2C_avail);

      status_string="Read TTC add3";
      //status_string+="I2CData=0x";
      if (I2C_avail){
        oss<<hex<<I2CData;
        status_string+="I2CData=0x"+(oss.str());
        oss.str("");
      } else
        status_string+="I2CData=0xUU";

      status_string+=" VIRTEX_PLL_locked=";
      if (PLL_locked){status_string+="1";}else status_string+="0";
      status_string+="\tQPLL_error=";
      if (QPLL_error){status_string+="1";}else status_string+="0";
      status_string+="\tQPLL_locked=";
      if (QPLL_locked){status_string+="1";}else status_string+="0";
      status_string+="\tTTC_ready=";
      if (TTC_ready){status_string+="1";}else status_string+="0";
      status_string+="\tI2C_avail=";
      if (I2C_avail){status_string+="1";}else status_string+="0";

      strcpy(status_char, (status_string.c_str()));
      (*Service_Connect_Status).updateService(status_char);
      fichierLog<< current_date_FileName()<< status_string << endl;
      cout << status_string <<endl;
    }//else if ( currCmnd == ReadTTC )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////////GetSerialStatus//////////////
    else if ( currCmnd == GetSerialStatus )
    {
      status_string= current_date_FileName()+"\n" + "GetSerialStatus Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      ostringstream oss;

      oss << "CurrentDeserAdd: " << CurrentDeserAdd << endl;
      status_string= oss.str();
      cout<<status_string;
      oss.str("");

      unsigned int mem_delay;
      int PhaseOptValue;
      bool PhasingDone;
      bool CharDone;
      bool CharFailure;
      bool RDY_DELAY;

      mem_delay=STU_API.GetMemDelayImage();

      cout << "mem_delay: " << hex << mem_delay << endl;
      STU_API.GetDeserStatus(PhaseOptValue,PhasingDone,CharDone,CharFailure,RDY_DELAY,align);

      cout << "return values: " << PhaseOptValue << " " << PhasingDone << " " << CharDone << " " << CharFailure << " " << RDY_DELAY << endl;

      status_string+="GetSerialStatus ->";
      oss<<hex<<mem_delay;
      status_string+="mem_delay=0x" + oss.str();
      oss.str("");

      oss<<dec<<PhaseOptValue;
      status_string+="\tPhaseOptValue=" + oss.str();
      oss.str("");

      status_string+="\tPhasingDone=" ;
      if (PhasingDone){status_string+="1";}else status_string+="0";
      status_string+="\tCharDone=";
      if (CharDone){status_string+="1";}else status_string+="0";
      status_string+="\tCharFailure=";
      if (CharFailure){status_string+="1";}else status_string+="0";
      status_string+="\tRDY_DELAY=";
      if (RDY_DELAY){status_string+="1";}else status_string+="0";
      cout << status_string <<endl;

      strcpy(status_char, (status_string.c_str()));
      cout << status_char <<endl;
      fichierLog << status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == GetSerialStatus )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////WriteTTC/////////////
    else if ( currCmnd == WriteTTC )
    {
      status_string= current_date_FileName()+"\n" + "WriteTTC Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;
      
      ostringstream oss;

      //The Value has to be sent by the panel in hex 
      WriteTTC_val = currCmnd->getInt();

      oss<< hex << "WriteTTC_val: " << WriteTTC_val << endl;
      cout<<oss;
      oss.str("");
      STU_API.WriteI2CTTC(_CTRL_REG,WriteTTC_val);
      status_string+="Write TTC done";

      strcpy(status_char, (status_string.c_str()));
      cout<< status_string << endl;
      fichierLog<< status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == WriteTTC )	

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ///////////ReadReceptionPointer/////////////
    else if ( currCmnd == ReadReceptionPointer )
    {
      status_string= current_date_FileName()+"\n" + "ReadReceptionPointer Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      CurrentDeserAdd = currCmnd->getInt();

      unsigned short PointerLevel=0;
      ostringstream oss1, oss2;

      PointerLevel=STU_API.ReadReceptionPointer(CurrentDeserAdd);

      oss1<<dec<<CurrentDeserAdd;
      oss2<<dec<<PointerLevel;
      status_string="input#" +  oss1.str() + " Level=" + oss2.str();
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      strcpy(status_char, (status_string.c_str()));
      fichierLog << status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }/////////////ReadReceptionPointer/////////////

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ///////////RegionEnable/////////////
    if ( currCmnd == RegionEnable )
    {
      status_string= current_date_FileName()+"\n" + "RegionEnable Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      ostringstream oss;

      int region_value= RegionEnable->getInt();

      oss << "region:"<< hex << region << endl;
      status_string= oss.str();

      STU_API.RegionEnable(region_value, L0_mask_int);

      strcpy(status_char, (status_string.c_str()));
      fichierLog << status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//if ( currCmnd == RegionEnable )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////RegisterLoopBack////////////////
    else if ( currCmnd == RegisterLoopBack )
    { 
      status_string= current_date_FileName()+"\n" + "RegisterLoopBack Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      int RegisterLoopBack_val= RegisterLoopBack->getInt();
      cout << "RegisterLoopBack_val:"<< hex << RegisterLoopBack_val<<endl;

      unsigned short ret_value;
      ostringstream oss1, oss2;

      ret_value=RegisterLoopBack_val;
      oss1<< hex<< ret_value;
      status_string="Loopback mode, Write 0x" + oss1.str();
      strcpy(status_char, (status_string.c_str()));
      cout << status_char <<endl;
      (*Service_Connect_Status).updateService(status_char);
      fichierLog<< status_string << endl;
      STU_API.SetCTLWORD_static(ret_value);
      STU_API.CheckCTLWORD_static(ret_value);

      oss2<< hex<< ret_value;
      status_string="\nLoopback returned value: 0x" + oss2.str();
      fichierLog<< status_string << endl;
      strcpy(status_char, (status_string.c_str()));
      cout << status_char <<endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == RegisterLoopBack )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////UpdatePulse////////////////
    else if ( currCmnd == UpdatePulse )
    { 
      status_string= current_date_FileName()+"\n" + "UpdatePulse Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      Cmd_Pulse * UpdatePulse;
      UpdatePulse = (Cmd_Pulse * ) (currCmnd->getData());

      unsigned short Value=0;
      status_string="Pulser:\n";
      ostringstream oss;

      if(UpdatePulse->set_phase_manu_cmd){ 
        Value|=set_phase_manu; 
        status_string+="->set_phase_manu\n";
      }
      if(UpdatePulse->recompute_phase_cmd){ 	
        Value|=recompute_phase_bit;
        status_string+="->recompute_phase_bit\n";
      }
      if(UpdatePulse->do_slip_manu_cmd){ 
        Value|=do_slip_manu;
        status_string+="->do_slip_manu\n";
      }
      if(UpdatePulse->L0_cmd){ 
        Value|=fake_L0;
        status_string+="->fake_L0\n";
      }
      if(UpdatePulse->L1a_cmd){ 	
        Value|=L1a;
        status_string+="->L1a\n";
      }
      if(UpdatePulse->L2a_cmd){ 	
        Value|=L2a;
        status_string+="->L2a\n";
      }
      if(UpdatePulse->L2r_cmd){
        Value|=L2r;
        status_string+="->L2r\n";
      }
      if(UpdatePulse->ClearL0Cnt_cmd){ 
        Value|=ClearL0Cnt;
        status_string+="->ClearL0Cnt\n";
      }
      if(UpdatePulse->setL0Phase_cmd){	
        Value|=setL0Phase;
        status_string+="->setL0Phase\n";
      }
      if(UpdatePulse->MeasJitter_cmd){ 
        Value|=measJitter;
        status_string+="->measJitter\n";
      }
      if(UpdatePulse->SOR_reset_cmd){ 
        Value|=SOR_reset;
        status_string+="->SOR_reset\n";
      }

      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
      fichierLog<< status_string << endl;
      cout << "Pulser Value: "<< hex << Value <<endl;

      //   STU_API.SetBufferAddress(CurrentDeserAdd);
      //   oss<<hex<<CurrentDeserAdd << endl;
      //   status_string= "CurrentDeserAdd sent:" + oss.str();
      //   oss.str("");
      STU_API.SendPulse(Value);
      sleep(1);
      oss<<hex<<Value;

      status_string+="Pulse word written 0x" + oss.str();
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog<< status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == UpdatePulse )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////////////UpdateMode//////////////////
    if ( currCmnd == UpdateMode )
    { 
      status_string= current_date_FileName()+"\n" + "UpdateMode Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      Cmd_Mode * UpdateMode;
      UpdateMode = (Cmd_Mode * ) (currCmnd->getData());

      unsigned short Value=0;
      status_string="Mode Control:\n";
      ostringstream oss;

      Value|=((UpdateMode->PhaseManuValue_box) <<1);
      PhaseManuVal=UpdateMode->PhaseManuValue_box;
      Value|=CurrentDeserAdd<<6;

      if(UpdateMode->EnableL0OverTTC_cmd){ 	
        Value|=Enable_L0_over_TTC; 
        status_string+="-> Enable_L0_over_TTC\n";
      }

      if(UpdateMode->GetRAWDataChk_cmd){ 
        Value|=GetRaw;
        GetRawData=1;
        status_string+="->GetRaw\n";
      }
      else 
        GetRawData=0;					

      if(UpdateMode->UseTestPatternChk_cmd){ 
        Value|=UseTestPattern;
        status_string+="->UseTestPattern\n";
      }

      if(UpdateMode->UseFakeTriggersChk_cmd){
        Value|=UseFakeTrigger;
        status_string+="->UseFakeTrigger\n";
      }

      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog << status_string << endl;
      (*Service_Connect_Status).updateService(status_char);

      oss<< "Value word: " <<hex << Value << endl;

      STU_API.SetCTLWORD_static(Value);
      status_string+= oss.str();

      cout << "ctl word: " << hex << Value << endl;

      status_string="Control word written 0x" + oss.str();

      STU_API.setL0ProcessingTime(_L0ProcessingTime);
      status_string+="\nL0-L1 timing set";

      STU_API.SetBufferAddress(CurrentDeserAdd);
      sleep(1);

      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog << status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//if ( currCmnd == UpdateMode )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////////ChangeSerialAdd///////////
    else if ( currCmnd == ChangeSerialAdd )
    { 
      status_string= current_date_FileName()+"\n" + "ChangeSerialAdd Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;
      
      ostringstream oss;
      fichierLog<< current_date_FileName()<< "\n Command -> ChangeSerialAdd\n"<< status_string << endl;
      //The address has to be sent by the panel
      CurrentDeserAdd = currCmnd->getInt();

      oss<<"CurrentDeserAdd: "<< CurrentDeserAdd << endl;
      status_string= oss.str();
      oss.str("");

      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog << status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ChangeSerialAdd )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////////ReadISPFLASHIDCODEs///////////
    else if ( currCmnd == ReadISPFLASHIDCODES)
    {
      status_string= current_date_FileName()+"\n" + "ReadISPFLASHIDCODES Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      ostringstream oss0, oss1;

      status_string=("Read ISPFLASH IDCODE");
      STU_API.GetIspFlashIDCODES(IDCODE0,IDCODE1);
      oss0<<hex<<IDCODE0;
      status_string+="\tReturned values : IDCODE0=0x" + oss0.str();

      oss1<<hex<<IDCODE1;
      status_string+="\tIDCODE1=0x" + oss1.str()+"\n";

      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog<< status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ReadISPFLASHIDCODES)

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////ISPFLASHRegLoopBack/////////
    else if ( currCmnd == ISPFLASHRegLoopBack )
    { 
      status_string= current_date_FileName()+"\n" + "ISPFLASHRegLoopBack Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      ISPFLASHLoopBackReg_val = currCmnd->getInt();

      ostringstream oss0, oss1;
      unsigned short value=ISPFLASHLoopBackReg_val;

      oss0<<hex<<value;
      cout<<"ISPFLASHLoopBackReg_val: " << ISPFLASHLoopBackReg_val << endl;
      status_string="ISPFLASH Loopback mode, Write 0x" + oss0.str()+"\n";
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      fichierLog << status_string << endl;
      (*Service_Connect_Status).updateService(status_char);

      STU_API.WriteIspflashCtrl(value);
      STU_API.ReadIspflashCtrl(value);

      oss1<<hex<<value;
      status_string="ISPFLASH Loopback returned value: 0x" + oss1.str()+"\n";
      fichierLog << status_string << endl;
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ISPFLASHRegLoopBack )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////ReadFlashSignature/////////
    else if ( currCmnd == ReadFlashSignature )
    { 
      status_string= current_date_FileName()+"\n" + "ReadFlashSignature Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      unsigned short MANUFACTURER;
      unsigned short DEVICE_CODE;
      ostringstream oss0, oss1;
      fichierLog<< current_date_FileName()<< "\n Command -> ReadFlashSignature\n"<< status_string << endl;

      status_string="Read flash signature";

      MANUFACTURER=STU_API.FlashAutoSelect(FLASH_READ_MANUFACTURER);
      DEVICE_CODE=STU_API.FlashAutoSelect(FLASH_READ_DEVICE_CODE);

      oss0<<hex<<MANUFACTURER;
      status_string+="Returned values : Manufacturer=0x" + oss0.str();
      oss1<<hex<<DEVICE_CODE;
      status_string+=" \tDEVICE_CODE=0x" + oss1.str();
      fichierLog<< status_string << endl;
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ReadFlashSignature )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /////////////ProgramFlash///////////////
    else if ( currCmnd == ProgramFlash )
    { 
      status_string= current_date_FileName()+"\n" + "ProgramFlash Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      STU_API.EmbeddedFlashErase();
      status_string= "ProgramFlash ->EmbeddedFlashErase completed";
      fichierLog<< current_date_FileName() << status_string << endl;
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);

      STU_API.EmbeddedConfigureFlash(FPGAFile);
      status_string= "ProgramFlash ->EmbeddedConfigureFlash(FPGAFile) completed";
      fichierLog<< current_date_FileName() << status_string << endl;
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ProgramFlash )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////////////RetrieveISPFLASHStatus////////////////////////////
    else if ( currCmnd == RetrieveISPFLASHStatus )
    { 
      status_string= current_date_FileName()+"\n" + "RetrieveISPFLASHStatus Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      bool FlashBusy;
      bool DONE_ACTEL;
      bool INIT_ACTEL;
      bool EN_SELECTMAP;
      fichierLog<< current_date_FileName()<< "\n Command -> RetrieveISPFLASHStatus\n"<< status_string << endl;

      STU_API.GetISPFLASHStatus(FlashBusy,DONE_ACTEL,INIT_ACTEL,EN_SELECTMAP);
      status_string="Check ISPFLASH status ->";
      status_string+="FlashBusy=";
      if(FlashBusy){status_string+="1";} else status_string+="0";
      status_string+="\tDONE_ACTEL=" ;
      if(DONE_ACTEL){status_string+="1";} else status_string+="0";
      status_string+="\tINIT_ACTEL=";
      if(INIT_ACTEL){status_string+="1";} else status_string+="0";
      status_string+="\tEN_SELECTMAP=";
      if(EN_SELECTMAP){status_string+="1";} else status_string+="0";
      fichierLog<< status_string << endl;
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == RetrieveISPFLASHStatus )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ///////////ReadV0all///////////////////////
    else if ( currCmnd == ReadV0all )
    { 
      status_string= current_date_FileName()+"\n" + "ReadV0all Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      unsigned int ret_value=STU_API.ReadV0Data();
      bool V0_RXREADY;
      bool V0_RXERROR;
      bool MOD_PRESENT;
      bool V0_RX_LOSS;
      short FrameCount;
      short ErrorCount;
      ostringstream oss,oss1,oss2;
      fichierLog<< current_date_FileName()<< "\n Command -> ReadV0all\n"<< status_string << endl;
      cout<< hex << "Read V0 data=" << ret_value << endl;
      oss << hex << ret_value;
      status_string="Read V0 data="+oss.str();
      strcpy(status_char, (status_string.c_str()));
      (*Service_Connect_Status).updateService(status_char);
      fichierLog<<  status_string << endl;

      STU_API.ReadV0LinkStatus(V0_RXREADY,V0_RXERROR, MOD_PRESENT,V0_RX_LOSS);
      status_string="V0 link status ";
      status_string+="V0_RXREADY=";
      if(V0_RXREADY){status_string+="1";} else status_string+="0";
      status_string+="V0_RXERROR=";
      if(V0_RXERROR){status_string+="1";} else status_string+="0";
      status_string+="MOD_PRESENT=";
      if(MOD_PRESENT){status_string+="1";} else status_string+="0";
      status_string+="V0_RX_LOSS=";
      if(V0_RX_LOSS){status_string+="1";} else status_string+="0";
      fichierLog<< status_string << endl;
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);

      STU_API.ReadV0FrameAndError(FrameCount,ErrorCount);
      oss1<< dec << FrameCount;
      oss2<< dec << ErrorCount;
      status_string="V0 frame count= "+ oss1.str();
      status_string+="\tV0 error count= "+ oss2.str() ; 
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ReadV0all )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////SetGammaThreshold///////////////	
    else if ( currCmnd == SetGammaThreshold )
    { 
      status_string= current_date_FileName()+"\n" + "SetGammaThreshold Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      Cmd_Param * Gamma_Param;
      Gamma_Param = (Cmd_Param * ) (currCmnd->getData());
      
      ostringstream oss, oss1, oss2, oss3, oss4, oss5, oss6, oss7, oss8;

      oss  << dec << Gamma_Param->A;
      oss1 << dec << Gamma_Param->B;
      oss2 << dec << Gamma_Param->C;
      oss3 << dec << Gamma_Param->A_mid;
      oss4 << dec << Gamma_Param->B_mid;
      oss5 << dec << Gamma_Param->C_mid;
      oss6 << dec << Gamma_Param->A_low;
      oss7 << dec << Gamma_Param->B_low;
      oss8 << dec << Gamma_Param->C_low;

      STU_API.SetGammaThresParameters(
          Gamma_Param->A_low,
          Gamma_Param->B_low,
          Gamma_Param->C_low,
          Gamma_Param->A_mid,
          Gamma_Param->B_mid,
          Gamma_Param->C_mid,
          Gamma_Param->A,
          Gamma_Param->B,
          Gamma_Param->C
          );
      G_A_low =Gamma_Param->A_low ;
      G_B_low =Gamma_Param->B_low ;
      G_C_low =Gamma_Param->C_low ;
      G_A_mid =Gamma_Param->A_mid ;
      G_B_mid =Gamma_Param->B_mid ;
      G_C_mid =Gamma_Param->C_mid ;
      G_A     =Gamma_Param->A     ;
      G_B     =Gamma_Param->B     ;
      G_C     =Gamma_Param->C     ;

      status_string="Gamma thres A=" + oss.str() + " B="      + oss1.str()  + " C="     + oss2.str() 
                         + " A_mid=" + oss3.str()+ " B_mid="  + oss4.str()  + " C_mid=" + oss5.str()
                         + " A_low=" + oss6.str()+ " B_low="  + oss7.str()  + " C_low=" + oss8.str()+"\n";
      strcpy(status_char, (status_string.c_str()));
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
      fichierLog<< status_string << endl;
    }//else if ( currCmnd == SetGammaThreshold )

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////////SetRandomVal////////////
    else if ( currCmnd == SetRandomVal )
    { 
      status_string= current_date_FileName()+"\n" + "SetRandomVal Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      Random_value = currCmnd->getInt();
      ostringstream oss;

      oss << hex << Random_value;      
      status_string="Do set Random value=" + oss.str();

      strcpy(status_char, status_string.c_str());
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
      fichierLog<<  status_string << endl;
      STU_API.setRandomGenerator(Random_value);
    }//else if ( currCmnd == SetRandomVal )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////SetTriggerDelays///////////	
    else if ( currCmnd == SetTriggerDelays )
    { 
      status_string= current_date_FileName()+"\n" + "SetTriggerDelays Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      Cmd_Delays * Delay_struct;
      Delay_struct = (Cmd_Delays * ) (currCmnd->getData());

      ostringstream oss, oss1, oss2;

      L0delay=Delay_struct->L0;
      L1_G_delay=Delay_struct->L1_G_delay;

      
      //TIN update
      TIN_delay=L1_G_delay;

      oss << dec << L0delay;
      oss2 << dec << L1_G_delay;

      
      status_string="Do set trigger delays L0=" + oss.str();
      status_string+="Do set trigger delays L1 Gamma=" + oss2.str();
      
      fichierLog << status_string << endl;
      strcpy(status_char, status_string.c_str());
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);

      STU_API.setL1GammaDelay(L1_G_delay);
 
    }//else if ( currCmnd == SetTriggerDelays )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////////SetTriggerDelays_TIN///////////	
    if ( currCmnd == SetTriggerDelays_TIN )
    { 
      status_string= current_date_FileName()+"\n" + "CTP SetTriggerDelays Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      TIN_delay = (currCmnd->getInt());

      L1_G_delay = TIN_delay;


      ostringstream oss;
      oss << dec << TIN_delay;

      status_string="CTP L1 triggers delay set to ->" + oss.str();

      STU_API.setL1GammaDelay(TIN_delay);


      (*Service_L1_G_delay).updateService(TIN_delay);


      fichierLog << status_string << endl;
      strcpy(status_char, status_string.c_str());
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
    }//if ( currCmnd == SetTriggerDelays_TIN )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ///////////SetOpcode/////////////
    else if ( currCmnd == SetOpcode )
    { 	
      status_string= current_date_FileName()+"\n" + "SetOpcode Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;
      opcode = currCmnd->getShort();
      Trigger_mode=opcode-1;
      cout<<"Opcode: " << Trigger_mode <<endl;
      // for CTP TIN proxy
      switch (opcode) {
        case 1: opcode_char='N'; 
                status_string="Trigger mode set to: Normal\n";
                strcpy(status_char, status_string.c_str());
                fichierLog << status_string <<endl;
                (*Service_Connect_Status).updateService(status_char);
                break;   // Normal
        case 2: opcode_char='T'; 
                status_string="Trigger mode set to: Toggling\n";
                strcpy(status_char, status_string.c_str());
                fichierLog << status_string <<endl;
                (*Service_Connect_Status).updateService(status_char);
                break;   // Toggle
        case 3: opcode_char='S'; 
                status_string="Trigger mode set to: Sending signature\n";
                strcpy(status_char, status_string.c_str());
                fichierLog << status_string <<endl;
                (*Service_Connect_Status).updateService(status_char);
                break;   // Signature
        case 4: opcode_char='R'; 
                status_string="Trigger mode set to: Random\n";
                strcpy(status_char, status_string.c_str());
                fichierLog << status_string <<endl;
                (*Service_Connect_Status).updateService(status_char);
                break;   // Random
      }

      STU_API.setTriggerMode(Trigger_mode);

      strcpy(status_char, "Set trigger mode");
      cout << status_string <<endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == SetOpcode )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////SetL0PhaseDelay//////////
    else if ( currCmnd == SetL0PhaseDelay )
    { 	
      status_string= current_date_FileName()+"\n" + "SetL0PhaseDelay Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      ostringstream oss, oss1;
      fichierLog<< current_date_FileName()<< "\n Command -> SetL0PhaseDelay\n"<< status_string << endl;

      // SetL0Delay from an *.ini file
      fichier.open("STU_control.ini");
      if( !fichier )// ce test �houe si le fichier n'est pas ouvert
      {
        cout << "file STU_control.ini unfound\n no delay set!!!" << endl;
      } 
      else
        cout << "file STU_control.ini found" << endl;

      //Get the Delay values from the STU_control.ini file
      for(int i=0; i<nb_PORT; i++)
      {
        do{
          getline( fichier, current_line );

          found=current_line.find("Delay=");
        } while(found==string::npos);

        delay_line= current_line;
        pos_eq=delay_line.find("=");
        delay_line.erase(0,pos_eq+1);

        int_delay[i]= atoi(delay_line.c_str());

        STU_API.SetL0Delay(i,int_delay[i]);
        oss<< dec << i;
        oss1<< dec << int_delay[i];
        status_string="InputNumber "+ oss.str()+"=>"+ oss1.str();
        strcpy(status_char, status_string.c_str());
        cout << status_string <<endl;
        fichierLog<< status_string << endl;
        (*Service_Connect_Status).updateService(status_char);
      }
      fichier.close();
    }//else if ( currCmnd == SetL0PhaseDelay )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //////////ReadOffsetCount////////////////	
    else if ( currCmnd == ReadOffsetCount )
    { 	
      status_string= current_date_FileName()+"\n" + "ReadOffsetCount Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      ostringstream oss, oss1, oss2;

      L0Count=STU_API.ReadL0Count();
      //L0Latency=STU_API.ReadL0Latency();
      STU_API.ReadL0LatencyDelay(L0Latency,L0_L0_delay);

      oss << dec << L0Count;
      oss1 << dec << L0Latency;
      oss2 << dec << L0_L0_delay;

      status_string=" L0Count=" + oss.str();
      status_string+="\n L0 latency=" + oss1.str();
      status_string+="\t L0 to L0 delay=" + oss2.str();

      strcpy(status_char, status_string.c_str());
      cout << status_string <<endl;
      fichierLog << status_string << endl;
      (*Service_Connect_Status).updateService(status_char);
    }//else if ( currCmnd == ReadOffsetCount )

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ///////SearchL0BestPhase////////////
    else if ( currCmnd == SearchL0BestPhase )
    { 
      status_string= current_date_FileName()+"\n" + "SearchL0BestPhase Command Received\n" + DimClientName();
      fichierLog<< status_string << endl;

      int delays[nb_PORT];
      ostringstream oss, buffer;
      ofstream fichier2("STU_control2.ini", ios::out);
      // *.ini file

      fichier.open("STU_control.ini");
      if( !fichier )// ce test �houe si le fichier n'est pas ouvert
      {
        cout << "file STU_control.ini unfound\n no delay set!!!" << endl;
      } 
      else
        cout << "file STU_control.ini found" << endl;

      //Get the Delay values from the STU_control.ini file
      for(int i=0; i<nb_PORT; i++)
      {
        STU_API.RegionEnable((1<<i),0);
        delays[i]=STU_API.FindBestPhase(i);
        oss<< dec << delays[i];

        do{
          getline( fichier, current_line );
          found=current_line.find("Delay=");
          if(found==string::npos){
            fichier2.write(current_line.c_str(), strlen(current_line.c_str()));
            fichier2.write("\n",1);}
        } while(found==string::npos);

        delay_line= current_line;
        pos_eq=delay_line.find("=");
        delay_line.replace(pos_eq+1,fstream::end,oss.str());

        fichier2.write(delay_line.c_str(), strlen(delay_line.c_str()));
        fichier2.write("\n",1);
        oss.str("");
        STU_API.SetL0Delay(i,int_delay[i]);
      }
      fichier.close();
      fichier2.close();

      if(remove("STU_control.ini"))
      {
        cout << "Error removing file" <<endl;
      }
      else if(rename("STU_control2.ini","STU_control.ini"))
      {
        cout << "Error renaming file" <<endl;
      }
    }//else if ( currCmnd == SearchL0BestPhase )
    ///////BusyClear////////////
    else if ( currCmnd == BusyClear )
    {
      status_string= current_date_FileName()+"\n" + "Clear Busy\n" + DimClientName();
      fichierLog<< status_string << endl;
      cout << "busy is cleared" << endl;
      status_string="";
      // ***UPDATE PULSE***
      STU_API.SendPulse(busy_clear_bit);
    }//else if ( currCmnd == BusyClear )
    else if ( currCmnd == EnablePattern )
    {
        unsigned short  rval   ;
        STU_API.load_test_memory("Pattern.txt");
        STU_API.SelTestPatternMode(true);

        STU_API.CheckCTLWORD_static(rval);
        fichierLog<< "ctl_word, aft PAT_MEM_TEST:" << hex << " readval=" <<rval << endl;

        status_string+="PAT_MEM_TEST \n";
        strcpy(status_char, status_string.c_str());
        (*Service_Connect_Status).updateService(status_char);

        cout << status_string <<endl;
        fichierLog<<status_string << endl;

        status_string="";
    }
  }//end of "try"	
  catch (THardwareAccess STU_except)
  { 
    status_string=catch_string;	
    status_string+=STU_except.Message.c_str();
    strcpy(status_char, (status_string.c_str()));
    cout << status_string <<endl;
    (*Service_Connect_Status).updateService(status_char);
    fichierLog<< current_date_FileName()<< status_string << endl;	
    return;
  }//catch
	
  fichierLog.close();		
}//void commandHandler()


//####################################################################################################
// The constructor creates the Commands
myCmd::myCmd( )
{
  //set DNS location
  // setDnsNode("localhost");
  //setDnsNode("alidcscom702");
  
  SOR_nb_int=0;
  SOR_nb_str="";

  status_char[0]='\0';

  p_GetConfig= &GetConfig;
  p_TRU_sync= &TRU_sync;
  p_TRU_sync_error= &TRU_sync_error;

  en_L0                   = new DimCommand((const char*)("/STU_PHOS/en_L0"                  ) , (char*)("I"   ), this  );
  L0_mask                 = new DimCommand((const char*)("/STU_PHOS/L0_mask"                ) , (char*)("I"   ), this  );
  ExitDIM                 = new DimCommand((const char*)("/STU_PHOS/ExitDIM"                ) , (char*)("I"   ), this  );
  GetI2CData              = new DimCommand((const char*)("/STU_PHOS/GetI2CData"             ) , (char*)("I"   ), this  );
  ErrorCount              = new DimCommand((const char*)("/STU_PHOS/ErrorCount"             ) , (char*)("I"   ), this  );
  SetTriggerDelays_TIN    = new DimCommand((const char*)("/STU_PHOS/SetTriggerDelays_TIN"   ) , (char*)("I"   ), this  );
  EN_MEM_PAT_TST          = new DimCommand((const char*)("/STU_PHOS/EN_MEM_PAT_TST"         ) , (char*)("I"   ), this  );
  SOR                     = new DimCommand((const char*)("/STU_PHOS/SOR"                    ) , (char*)("I"   ), this  );
  SOR_sync                = new DimCommand((const char*)("/STU_PHOS/SOR_sync"               ) , (char*)("I"   ), this  );
  SOR_number              = new DimCommand((const char*)("/STU_PHOS/SOR_number"             ) , (char*)("I"   ), this  );
  EOR                     = new DimCommand((const char*)("/STU_PHOS/EOR"                    ) , (char*)("I"   ), this  );
  sorReset                = new DimCommand((const char*)("/STU_PHOS/sorReset"               ) , (char*)("I"   ), this  );  
  ReloadConfig            = new DimCommand((const char*)("/STU_PHOS/ReloadConfig"           ) , (char*)("I"   ), this  );
  Configure               = new DimCommand((const char*)("/STU_PHOS/Configure"              ) , (char*)("I:12"), this  );//XXX
  GetConfiguration        = new DimCommand((const char*)("/STU_PHOS/GetConfiguration"       ) , (char*)("I"   ), this  );                   
  ConfFPGA                = new DimCommand((const char*)("/STU_PHOS/ConfFPGA"               ) , (char*)("I"   ), this  );
  ResetBoard              = new DimCommand((const char*)("/STU_PHOS/ResetBoard"             ) , (char*)("I"   ), this  );
  ReadIDCODEs             = new DimCommand((const char*)("/STU_PHOS/ReadIDCODEs"            ) , (char*)("I"   ), this  );
  ReadMonitor             = new DimCommand((const char*)("/STU_PHOS/ReadMonitor"            ) , (char*)("I"   ), this  );
  GetMonitorValues        = new DimCommand((const char*)("/STU_PHOS/GetMonitorValues"       ) , (char*)("I"   ), this  );
  GetDDLStatus            = new DimCommand((const char*)("/STU_PHOS/GetDDLStatus"           ) , (char*)("I"   ), this  );
  ReadTTC                 = new DimCommand((const char*)("/STU_PHOS/ReadTTC"                ) , (char*)("I"   ), this  );
  GetSerialStatus         = new DimCommand((const char*)("/STU_PHOS/GetSerialStatus"        ) , (char*)("I"   ), this  );
  WriteTTC                = new DimCommand((const char*)("/STU_PHOS/WriteTTC"               ) , (char*)("I"   ), this  );
  ReadReceptionPointer    = new DimCommand((const char*)("/STU_PHOS/ReadReceptionPointer"   ) , (char*)("I"   ), this  );
  RegionEnable            = new DimCommand((const char*)("/STU_PHOS/RegionEnable"           ) , (char*)("I"   ), this  );
  RegisterLoopBack        = new DimCommand((const char*)("/STU_PHOS/RegisterLoopBack"       ) , (char*)("I"   ), this  );
  UpdatePulse             = new DimCommand((const char*)("/STU_PHOS/UpdatePulse"            ) , (char*)("I:11"), this  );
  UpdateMode              = new DimCommand((const char*)("/STU_PHOS/UpdateMode"             ) , (char*)("I:6" ), this  );
  ChangeSerialAdd         = new DimCommand((const char*)("/STU_PHOS/ChangeSerialAdd"        ) , (char*)("I"   ), this  );
  ReadISPFLASHIDCODES     = new DimCommand((const char*)("/STU_PHOS/ReadISPFLASHIDCODES"    ) , (char*)("I"   ), this  );
  ISPFLASHRegLoopBack     = new DimCommand((const char*)("/STU_PHOS/ISPFLASHRegLoopBack"    ) , (char*)("I"   ), this  );
  ReadFlashSignature      = new DimCommand((const char*)("/STU_PHOS/ReadFlashSignature"     ) , (char*)("I"   ), this  );
  ProgramFlash            = new DimCommand((const char*)("/STU_PHOS/ProgramFlash"           ) , (char*)("I"   ), this  );
  RetrieveISPFLASHStatus  = new DimCommand((const char*)("/STU_PHOS/RetrieveISPFLASHStatus" ) , (char*)("I"   ), this  );
  ReadV0all               = new DimCommand((const char*)("/STU_PHOS/ReadV0all"              ) , (char*)("I"   ), this  );
  SetGammaThreshold       = new DimCommand((const char*)("/STU_PHOS/SetGammaThreshold"      ) , (char*)("I:9" ), this  );
  
  MonErrorCount           = new DimCommand((const char*)("/STU_PHOS/MonErrorCount"         )  , "I"   , this  ); 
  BusyClear               = new DimCommand((const char*)("/STU_PHOS/BusyClear"             )  , "I"   , this  );
  EnablePattern           = new DimCommand((const char*)("/STU_PHOS/EnablePattern"         )  , "I"   , this  );
  
  SetRandomVal            = new DimCommand((const char*)("/STU_PHOS/SetRandomVal"           ) , (char*)("I"   ), this  );
  SetTriggerDelays        = new DimCommand((const char*)("/STU_PHOS/SetTriggerDelays"       ) , (char*)("I:2" ), this  );//XXX
  SetOpcode               = new DimCommand((const char*)("/STU_PHOS/SetOpcode"              ) , (char*)("S"   ), this  );
  SetL0PhaseDelay         = new DimCommand((const char*)("/STU_PHOS/SetL0PhaseDelay"        ) , (char*)("I"   ), this  );
  ReadOffsetCount         = new DimCommand((const char*)("/STU_PHOS/ReadOffsetCount"        ) , (char*)("I"   ), this  );
  SearchL0BestPhase       = new DimCommand((const char*)("/STU_PHOS/SearchL0BestPhase"      ) , (char*)("I"   ), this  );

  // Services pointers Initialization
 
  //Service_L1_G_delay    = new DimService("/STU_PHOS/L1_G_delay_monitor"      , L1_G_delay_monitor  );
  Service_Conf_done       = new DimService("/STU_PHOS/Conf_done"               , Conf_done           );
  Service_SyncAlarm       = new DimService("/STU_PHOS/SynAlarm"                , TRU_SyncAlarm       );
  //Service_SyncAlarm     = new DimService("/STU_PHOS/SynAlarm"                , "I:32", p_SyncAlarm,sizeof(Cmd_TRU));

  Service_I2CData         = new DimService("/STU_PHOS/I2CData"                 , I2CData_read        );

  Service_ConfFPGA_Done   = new DimService("/STU_PHOS/ConfFPGA_Done"           , ConfFPGA_Done       );
  Service_ResetFPGA_Done  = new DimService("/STU_PHOS/ResetFPGA_Done"          , ResetFPGA_Done      );
  Service_SOR_Done        = new DimService("/STU_PHOS/SOR_Done"                , SOR_Done            );
  Service_Firmware        = new DimService("/STU_PHOS/Firmware"                , ID_FIRM             );

  Service_Connect_Status  = new DimService("/STU_PHOS/Connect_Status"          ,status_char);
  Service_GetConfig       = new DimService((const char*)("/STU_PHOS/GetConfig"     ) ,(char*)("I:12"), p_GetConfig      ,sizeof(Cmd_struct) );
  Service_TRU_sync        = new DimService((const char*)("/STU_PHOS/TRU_SYNC"      ) ,(char*)("I:32"), p_TRU_sync       ,sizeof(Cmd_TRU)    );
  Service_TRU_sync_error  = new DimService((const char*)("/STU_PHOS/TRU_SYNC_ERROR") ,(char*)("I:32"), p_TRU_sync_error ,sizeof(Cmd_TRU)    );

  global_region = 0xFFFFFFF;
  PhaseManuVal = 0;
	for(int i=0;i<nb_PORT;i++){
		cnt_packet_failure[i]	= 0	; 
		total_trial[i]=0;
		total_error[i]=0;
		total_err_rate[i]=0.;
		isFirstTrial[i]=0;
	}
}//constructor

bool myCmd::CalculateL0Mask(unsigned int &L0mask)
{
  
  bitset<28> bs;
  
  for(int iSRU=1; iSRU<=14; iSRU++) {
    if(!readTRUconfig(bs, 0, iSRU)) return false; // TRU00
    if(!readTRUconfig(bs, 2, iSRU)) return false; // TRU20   
  }
  
  L0mask = bs.to_ulong();
  return true;
}

bool myCmd::readTRUconfig(bitset<28> &bs, int type, int iSRU)
{  
  int mod,part,dtc,tru;
  std::string tmp,bad;
  char filename[80];
  
  if(type==0) tru = 2*iSRU - 1;
  if(type==2) tru = 2*iSRU;
  
  if(iSRU==1) { mod=1; part=2; } // M1-2
  if(iSRU==2) { mod=1; part=3; } // M1-3
  if(iSRU==3) { mod=2; part=0; } // M2-0
  if(iSRU==4) { mod=2; part=1; } // M2-1
  if(iSRU==5) { mod=2; part=2; } // M2-2
  if(iSRU==6) { mod=2; part=3; } // M2-3
  if(iSRU==7) { mod=3; part=0; } // M3-0
  if(iSRU==8) { mod=3; part=1; } // M3-1
  if(iSRU==9) { mod=3; part=2; } // M3-2
  if(iSRU==10){ mod=3; part=3; } // M3-3
  if(iSRU==11){ mod=4; part=0; } // M4-0
  if(iSRU==12){ mod=4; part=1; } // M4-1
  if(iSRU==13){ mod=4; part=2; } // M4-2
  if(iSRU==14){ mod=4; part=3; } // M4-3
  
  sprintf(filename,"M%d-%d/TRUparam_TRU%d0.dat",mod,part,type);
  printf("Reading TRU%.2d ports configuration from file %s\n",tru,filename);
  
  fstream f;

  f.open(filename);
  if(!f.is_open()) return false;
  
  f>>tmp>>dtc>>tmp>>bad;
  printf("   TRUreadout %s\n",bad.c_str());
  
  if(!strcmp(bad.c_str(),"good")) { bs[tru-1] = true; }
  else { bs[tru-1] = false; }
  
  return true;
}






//####################################################################################################
//####################################################################################################
//####################################################################################################
//####################################################################################################
//####################################################################################################
//####################################################################################################
//####################################################################################################
//####################################################################################################
//####################################################################################################
//####################################################################################################
int main(){

  typedef struct {
    float val[3];
  } monitor_struct;

  monitor_struct temp_struct, vccaux_struct, vccint_struct;

  double temp[3];
  double vccint[3];
  double vccaux[3];

  //Monitoring services
  DimService Service_temp(  (const char*)("/STU_PHOS/Service_temp"  ),(char*)("F:3"),&temp_struct  ,sizeof(monitor_struct));
  DimService Service_vccint((const char*)("/STU_PHOS/Service_vccint"),(char*)("F:3"),&vccint_struct,sizeof(monitor_struct));
  DimService Service_vccaux((const char*)("/STU_PHOS/Service_vccaux"),(char*)("F:3"),&vccaux_struct,sizeof(monitor_struct));

  ostringstream oss;

  myCmd cmd;

  //TIN services
  //Services for CTP (Tin proxy functionality)

  DimService Service_STATUS_DELAY("PHOS/STATUS_DELAY",cmd.TIN_delay);
  DimService Service_STATUS_OPCODE("PHOS/STATUS_OPTIONCODE",&(cmd.opcode_char));

  //
  // DIM Server START
  ///////////////////////////////////////
  DimServer::start("STU_PHOS");

  //
  // STU Board Connection
  ///////////////////////

  (cmd.STU_API).Connect();

  sleep(5);
  //FPGA configuration
  //string FPGAFile="STU_FPGA.mcs";
  //(cmd.STU_API).SelectmapConfFPGA(FPGAFile);

  while(1)
  {
    (cmd.STU_API).GetMonitorStatus(temp,vccint,vccaux);//FPGA status(temperature, Vcc_int, Vcc_aux)

    for( int i=0; i<3; i++)
    {
      temp_struct.val[i]=temp[i];
      vccint_struct.val[i]=vccint[i];
      vccaux_struct.val[i]=vccaux[i];
    }
    //(cmd.STU_API).GetMonitorStatus(temp,vccint,vccaux);

    //CTP TIN services
    Service_STATUS_OPCODE.updateService();
    Service_STATUS_DELAY.updateService();	

    Service_temp.updateService(&temp_struct, sizeof(monitor_struct));
    Service_vccint.updateService(&vccint_struct,sizeof(monitor_struct));
    Service_vccaux.updateService(&vccaux_struct,sizeof(monitor_struct));

    sleep(1);
  }
  return 0;

}


