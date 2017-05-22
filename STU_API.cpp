#include "STU_API.h"

#define _USE_MUTEX

#ifdef _USE_MUTEX
#include <pthread.h>
static pthread_mutex_t acces=PTHREAD_MUTEX_INITIALIZER;
#endif

///////////////////////////////////////////////////////////////////////////////
/**Constructor.
*/
THardwareAccess::THardwareAccess(string Mess)
{
  Message=Mess;
}
//---------------------------------------------------------------------------
TSTU_API::TSTU_API()
{
  //FPGAConfigFile="proto_STU.mcs";
  //FPGAConfigFile="FPGA_test.mcs";
  CTLWORD_static=0;
  CTRLWORD_static2=1;
}
//---------------------------------------------------------------------------
bool TSTU_API::Connect()
{
  // if (!STU_mezzanine.ConfigureConnection("10.160.132.123",7777))//RUN2 DCAL
  //if (!STU_mezzanine.ConfigureConnection("10.160.132.122",7777))//RUN2 EMCAL
  if (!STU_mezzanine.ConfigureConnection("10.160.35.102",7777))//RUN2 PHOS
  {
    THardwareAccess HardwareAccess("Connection configuration: " + STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }
  if (!STU_mezzanine.ConnectToServer())
  {
    THardwareAccess HardwareAccess(STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  cout << "STU connected :for PHOS " << endl;
  return true;
}
//---------------------------------------------------------------------------
TSTU_API::~TSTU_API()
{

  STU_mezzanine.Close();
}
//---------------------------------------------------------------------------
bool TSTU_API::FPGADMARead(unsigned short *dest_buffer, int count, char add)
{
#ifdef _USE_MUTEX
  pthread_mutex_lock(&acces);
#endif
  int byte_count=2*count;
  instruction[0]=INST_DMA_READ;

  instruction[1]=add;

  instruction[2]=(char)(byte_count>>24);
  instruction[3]=(char)(byte_count>>16);
  instruction[4]=(char)(byte_count>>8);
  instruction[5]=(char)(byte_count>>0);

  if(!STU_mezzanine.SendData(instruction,6))
  {
    THardwareAccess HardwareAccess("DMA Read request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.ReceiveData((char *)dest_buffer,byte_count))
  {
    THardwareAccess HardwareAccess("DMA Read return failed: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }
#ifdef _USE_MUTEX
  pthread_mutex_unlock(&acces);
#endif
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::ISPFLASHDMARead(unsigned short *dest_buffer, int count, char add)
{
#ifdef _USE_MUTEX
  pthread_mutex_lock(&acces);
#endif
  int byte_count=2*count;
  instruction[0]=INST_ISPFLASH_DMA_READ;

  instruction[1]=add;

  instruction[2]=(char)(byte_count>>24);
  instruction[3]=(char)(byte_count>>16);
  instruction[4]=(char)(byte_count>>8);
  instruction[5]=(char)(byte_count>>0);

  if(!STU_mezzanine.SendData(instruction,6))
  {
    THardwareAccess HardwareAccess("DMA Read request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.ReceiveData((char *)dest_buffer,byte_count))
  {
    THardwareAccess HardwareAccess("DMA Read return failed: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }
#ifdef _USE_MUTEX
  pthread_mutex_unlock(&acces);
#endif
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::FPGADMAWrite(unsigned short *src_buffer, int count, char add)
{
#ifdef _USE_MUTEX
  pthread_mutex_lock(&acces);
#endif
  int byte_count=2*count;

  instruction[0]=INST_DMA_WRITE;

  instruction[1]=add;

  instruction[2]=(char)(byte_count>>24);
  instruction[3]=(char)(byte_count>>16);
  instruction[4]=(char)(byte_count>>8);
  instruction[5]=(char)(byte_count>>0);

  if(!STU_mezzanine.SendData(instruction,6))
  {
    THardwareAccess HardwareAccess("DMA Write request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.SendData((char *)src_buffer,byte_count))
  {
    THardwareAccess HardwareAccess("DMA Write request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }
#ifdef _USE_MUTEX
  pthread_mutex_unlock(&acces);
#endif
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::ISPFLASHDMAWrite(unsigned short *src_buffer, int count, char add)
{
  int byte_count=2*count;

  instruction[0]=INST_ISPFLASH_DMA_WRITE;

  instruction[1]=add;

  instruction[2]=(char)(byte_count>>24);
  instruction[3]=(char)(byte_count>>16);
  instruction[4]=(char)(byte_count>>8);
  instruction[5]=(char)(byte_count>>0);

  if(!STU_mezzanine.SendData(instruction,6))
  {
    THardwareAccess HardwareAccess("DMA Write request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.SendData((char *)src_buffer,byte_count))
  {
    THardwareAccess HardwareAccess("DMA Write request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::GetIDCODES(unsigned short &IDCODE0,unsigned short &IDCODE1)
{
  FPGADMARead(&IDCODE0,1,add_ETH_IDCODE0);
  FPGADMARead(&IDCODE1,1,add_ETH_IDCODE1);


  unsigned short value;

  for (int add=0;add<0x46;add=add+2)
  {
    FPGADMARead(&value,1,(unsigned char)add);
    cout <<hex << "add=0x" << add << " val=0x" << value << endl;
  }

  return true;
}
//---------------------------------------------------------------------------
void TSTU_API::GetIspFlashIDCODES(unsigned short &IDCODE0,unsigned short &IDCODE1)
{
  ISPFLASHDMARead(&IDCODE0,1,add_ETH_IDCODE0);
  ISPFLASHDMARead(&IDCODE1,1,add_ETH_IDCODE1);
}
//---------------------------------------------------------------------------
bool TSTU_API::PutConfigToMem(string src_file)
{
  ifstream ConfigFileHandle;
  int length;
  char buffer[9];

  ConfigFileHandle.open(src_file.c_str(),ios::in);

  if (!ConfigFileHandle)
  {
    THardwareAccess HardwareAccess("FPGA configuration file not found: "+ src_file);
    throw (HardwareAccess);
  }

  // get length of file:
  ConfigFileHandle.seekg (0, ios::end);
  length = ConfigFileHandle.tellg(); //taille en octet du fichier
  ConfigFileHandle.seekg (0, ios::beg);

  ConfigFileHandle.clear();
  ///////////////////////////////////////////////////////////////////////////////
  //try to allocate new memory block
  ///////////////////////////////////////////////////////////////////////////////
  ConfigData=new char[length /2];
  if (ConfigData==NULL)
  {
    THardwareAccess HardwareAccess("bad memory allocation");
    throw (HardwareAccess);
  }
  ///////////////////////////////////////////////////////////////////////////////
  //perform data extraction
  ///////////////////////////////////////////////////////////////////////////////
  bool QUIT = false;
  int RECLEN;
  int LOADOFFSET;
  int RECTYP;
  int temp_buf;
  ConfigDataIndice=0;
  while (!QUIT)
  {
    ConfigFileHandle.read (buffer,9); //gets RECORD FORMAT
    if(sscanf(buffer,":%2x%4x%2x",&RECLEN,&LOADOFFSET,&RECTYP))
    {
      for (int i=0;i<RECLEN;i++)
      {
        ConfigFileHandle.read (buffer,2);  //gets next byte
        switch(RECTYP)
        {
          case 0x00 : //Data record
            if (sscanf(buffer,"%2x",&temp_buf)) {
              ConfigData[ConfigDataIndice]=(unsigned char) temp_buf;
              ConfigDataIndice=ConfigDataIndice+ 1;
            }
            else
              QUIT=true;
            break;
          case 0x01 : //End of file record
            QUIT=true; //this case should not be reached because RECLEN =0 in that case
            break;
          case 0x02 : //Extended Segment address record
            break;
          case 0x03 : //Start Segment address record
            break;
          case 0x04 : //Extended Linear address record
            break;
          case 0x05 : //Start Linear address record
            break;
        }
      }
      //ConfigFileHandle.read (buffer,3);  //gets line checksum and end of line //avant C++ 4 char
      ConfigFileHandle.getline (buffer,10);  //gets line checksum and end of line -> indifferent type de terminaison
    }
    else
      QUIT=true;
  }
  ConfigFileHandle.close();

  //debug routine
  /*
     ofstream sortie;
     sortie.open("image_Config.txt",ios::out);
     if (!sortie)
     {
     string Message="fichier pas ouvert";
     }
     sortie << hex <<setiosflags(ios_base::uppercase) ;

     unsigned char val_to_print;
     for (int i=0;i<ConfigDataIndice;i++)
     {
     val_to_print=ConfigData[i];
     sortie << setw(2)<< setfill('0')<<(int) (val_to_print) << endl;
     }

     sortie.close();
     */
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::ConfFPGA(string ConfigFile)
{
  FPGAConfigFile=ConfigFile;
  return ConfFPGA();
}
//---------------------------------------------------------------------------
bool TSTU_API::ConfFPGA()
{

  if (!PutConfigToMem(FPGAConfigFile))
    return false;

  cout<< "data in mem" << endl;

  instruction[0]=INST_CONFIG_FPGA;
  instruction[1]=(char)((ConfigDataIndice>>16) & 0xFF);
  instruction[2]=(char)((ConfigDataIndice>>8) & 0xFF);
  instruction[3]=(char)((ConfigDataIndice) & 0xFF);

  if(!STU_mezzanine.SendData(instruction,4))
  {
    THardwareAccess HardwareAccess("FPGA configuration request" + STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.ReceiveData(instruction,1))
  {
    THardwareAccess HardwareAccess("Check end of FPGA clear request" + STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }
  if (instruction[0]!='O')
  {
    THardwareAccess HardwareAccess("FPGA clear failure" + STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.SendData(ConfigData,ConfigDataIndice))
  {
    THardwareAccess HardwareAccess("XFER FPGA configuration data" + STU_mezzanine.GetStatus());
    delete[] ConfigData;
    throw (HardwareAccess);
  }
  delete[] ConfigData;
  cout << "serial data Xfered" << endl;

  if(!STU_mezzanine.ReceiveData(instruction,1))
  {
    THardwareAccess HardwareAccess("Request check end of configuration " + STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if (instruction[0]!='O')
  {
    THardwareAccess HardwareAccess("FPGA configuration failure " + STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::DCSResetBoard()
{
  CTRLWORD_static2=1;

  instruction[0]=INST_RESET_BOARD;

  if(!STU_mezzanine.SendData(instruction,1))
  {
    THardwareAccess HardwareAccess("Reset board failed " + STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }
  return true;
}
//---------------------------------------------------------------------------
void TSTU_API::ResetTTConly()
{
  CTRLWORD_static2=0;
  FPGADMAWrite(&CTRLWORD_static2,1,add_ETH_CTRL_static2);
  sleep(1);
  CTRLWORD_static2=1;
  FPGADMAWrite(&CTRLWORD_static2,1,add_ETH_CTRL_static2);
}
//---------------------------------------------------------------------------
void TSTU_API::ACTELResetBoard()
{
  CTRLWORD_static2=1;
  SetISPFLASHMODE(/*mux_to_ETH*/true,/*trigger_configuration*/false,/*do_reset*/true);
  SetISPFLASHMODE(/*mux_to_ETH*/false,/*trigger_configuration*/false,/*do_reset*/false);
}
//---------------------------------------------------------------------------
bool TSTU_API::SetCTLWORD_static(unsigned short &value)
{
  CTLWORD_static=value;
  FPGADMAWrite(&value,1,add_ETH_CTRL_static);
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::SetCTLWORD_static()
{
  FPGADMAWrite(&CTLWORD_static,1,add_ETH_CTRL_static);
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::CheckCTLWORD_static(unsigned short &value)
{

  FPGADMARead(&value,1,add_ETH_CTRL_static);

  return true;

}
//---------------------------------------------------------------------------
unsigned short TSTU_API::ReadErrorCount()
{
  unsigned short toto;
  FPGADMARead(&toto,1,add_ETH_error_count);
  return toto;	
}
//---------------------------------------------------------------------------
unsigned short TSTU_API::GetErrorCount(int add)
{
  unsigned short toto;
  SetBufferAddress(add);
  SetCTLWORD_static();
  FPGADMARead(&toto,1,add_ETH_error_count);
  return toto;	
}

//---------------------------------------------------------------------------
void TSTU_API::GetErrorCount(unsigned short *count_table)
{
  unsigned short toto;
  //unsigned short ctl_word;

  //for (int i=0;i<32;i++)
  for (int i=0;i<nb_PORT;i++)//HIROKI
  {
    SetBufferAddress(i);
    //cout<< "error count ctl_word bef:" << hex << CTLWORD_static << endl;
    SetCTLWORD_static();
    //cout<< "ctl_word:" << hex << CTLWORD_static << " " << val<< endl;
    //FPGADMAWrite(&toto,1,add_ETH_error_count);
    //cout << hex << "add="<< add_ETH_error_count << endl;
    FPGADMARead(&toto,1,add_ETH_error_count);
    //cout <<dec << "i=" << i << " count=" << toto << endl;
    count_table[i]=toto;
  }	
}
//---------------------------------------------------------------------------
/**Build an example test pattern memory
  @see load_test_memory(const char*pfile)
  */
void TSTU_API::build_test_pattern()
{
  //const int LocalDataCount=8*(96+6);
  //const int TotalDataCount=32*LocalDataCount;
  //unsigned short test_mem[2*TotalDataCount];

  ofstream ExamplePatternFile;
  ExamplePatternFile.open("PatternExample.txt",ios::out);

  //for(int num_TRU=0;num_TRU<32;num_TRU++)
  for(int num_TRU=0;num_TRU<nb_TRU;num_TRU++)//HIROKI
  {
    for (int PatNum=0;PatNum<8;PatNum++)
    {
      //for (int i=0;i<96; i++) //ADC val
      for (int i=0;i<nb_MODULE; i++) //ADC val HIROKI
      {
        ExamplePatternFile << hex << ((PatNum<<8) | num_TRU) << endl;
      }

      //HIROKI for (int i=0;i<6; i++) //L0 masks
      //HIROKI {
      //HIROKI   ExamplePatternFile << hex << (i+ num_TRU) << endl;
      //HIROKI }
    }
  }
  ExamplePatternFile.close();

}
//---------------------------------------------------------------------------
/**Load the test memory with the test data.
  Test data should be in a single file nammed test_data.txt and be organized as follows:
  - file in hex format
  - for each TRU, 8 patterns in  serie 96 ADC values + 6 L0 indexes
  - No mirrorring in the data, they should be as the TRU would send them
  */
bool TSTU_API::load_test_memory(const char*pfile)
{
  //const int LocalDataCount=8*(96+6);
  //const int TotalDataCount=32*LocalDataCount;
  const int LocalDataCount=8*nb_MODULE;//HIROKI
  const int TotalDataCount=nb_TRU*LocalDataCount;//HIROKI
  unsigned short test_mem[2*TotalDataCount];
  int i=0;

  //extract data from filei
  ifstream Patternfile;
  string ligne;

  Patternfile.open(pfile,ifstream::in);
  Patternfile.clear();

  while (getline(Patternfile,ligne))
  {
    if (i>=TotalDataCount)
    {
      cout << "file too large : " <<i<<"/"<<TotalDataCount<< endl;
      return false;
    }

    istringstream stream1(ligne);
    stream1 >> hex >> test_mem[i];
    i++;
  }

  if (i!=TotalDataCount)
  {
    cout << "file too small : " <<i<<"/"<<TotalDataCount<< endl;
    return false;
  }

  //send data in STU
  SendPulse(clear_pointer_test);

  //for(int num_TRU=0;num_TRU<32;num_TRU++)
  for(int num_TRU=0;num_TRU<nb_TRU;num_TRU++)//HIROKI
  {
    //cout << dec << "TRU #" << num_TRU << " data[0]=0x" << hex << test_mem[LocalDataCount*num_TRU] << endl;
    SetBufferAddress(num_TRU);
    SetCTLWORD_static();

    FPGADMAWrite(test_mem+LocalDataCount*num_TRU,LocalDataCount,add_ETH_test_mem);
  }

  cout <<"pat file loaded : " << TotalDataCount<<endl;
  return true;
}
//---------------------------------------------------------------------------
/**Enable the test pattern mode.
  Enables the use of the memory containing 8 patterns*/
void TSTU_API::SelTestPatternMode(bool en)
{
  if (en)
    CTLWORD_static|=0x0001;
  else
    CTLWORD_static&= ~(0x0001);

  SetCTLWORD_static();

}
//---------------------------------------------------------------------------
void TSTU_API::DoClearReceptionFIFO()
{
  CTLWORD_static|=0x0001;
}
//---------------------------------------------------------------------------
void TSTU_API::DoEnableReceptionFIFO()
{
  CTLWORD_static&= ~(0x0001);
}
//---------------------------------------------------------------------------
void TSTU_API::SetRecordOnL0(bool val)
{
  if (val)
    CTLWORD_static|=2;
  else
    CTLWORD_static&=~2;

  SetCTLWORD_static();
}
//---------------------------------------------------------------------------
void TSTU_API::SetPhaseValue(int val)
{
  if (val>31 || val<0)
  {
    THardwareAccess HardwareAccess("Phase value too large");
    throw (HardwareAccess);
  }
  CTLWORD_static&=~(0x1F<<1);
  CTLWORD_static|=(val<<1);

}
//---------------------------------------------------------------------------
void TSTU_API::SetBufferAddress(int val)
{
  //if (val>(nb_TRU+1) || val<0)
  if (val>(nb_PORT+1) || val<0)
  {
    THardwareAccess HardwareAccess("Buffer address out of range");
    throw (HardwareAccess);
  }

  CTLWORD_static&=~(0x3F<<6);
  CTLWORD_static|=(val<<6);
}

//---------------------------------------------------------------------------
bool TSTU_API::ApplyPhaseManu()
{
  CTLWORD_pulse=set_phase_manu;
  FPGADMAWrite(&CTLWORD_pulse,1,add_ETH_CTRL_pulse);
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::DoRecomputePhase()
{
  CTLWORD_pulse=recompute_phase_bit;
  FPGADMAWrite(&CTLWORD_pulse,1,add_ETH_CTRL_pulse);
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::DoSlipManu()
{
  CTLWORD_pulse=do_slip_manu;
  FPGADMAWrite(&CTLWORD_pulse,1,add_ETH_CTRL_pulse);
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::DoSetL0Phase()
{
  CTLWORD_pulse=setL0Phase;
  FPGADMAWrite(&CTLWORD_pulse,1,add_ETH_CTRL_pulse);
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::DoMeasJitter()
{
  CTLWORD_pulse=measJitter;
  FPGADMAWrite(&CTLWORD_pulse,1,add_ETH_CTRL_pulse);
  return true;
}

//---------------------------------------------------------------------------
unsigned short TSTU_API::GetBufferStatus()
{
  unsigned short FIFOStatus;
  FPGADMARead(&FIFOStatus,1,add_ETH_FIFO_DESER_status);

  return FIFOStatus;
}
//---------------------------------------------------------------------------
bool TSTU_API::ReadDeserBuffer(unsigned short *dest_buffer,int length)
{
  return FPGADMARead(dest_buffer,length,add_ETH_FIFO_DESER_DATA); 
}
//---------------------------------------------------------------------------
unsigned int TSTU_API::GetMemDelayImage()
{
  unsigned short temp_buf;
  unsigned int MemDelay;
  MemDelay=0;
  //int toto;

  FPGADMARead(&temp_buf,1,add_ETH_mem_delay_LSB);
  MemDelay=(unsigned int)temp_buf;
  //toto=add_ETH_mem_delay_LSB;
  //cout << "add=0x" << hex << toto << " LSB=0x"  << temp_buf << endl;

  FPGADMARead(&temp_buf,1,add_ETH_mem_delay_MSB);
  MemDelay|=(unsigned int)(temp_buf<<16);
  //toto=add_ETH_mem_delay_MSB;
  //cout << "add=0x" << hex << toto << " MSB=0x"  << temp_buf << endl;

  //cout << hex << MemDelay << endl;
  return MemDelay;
}
//---------------------------------------------------------------------------
bool TSTU_API::GetDeserStatus(int &PhaseOptValue,bool &PhasingDone,bool &CharDone,bool &CharFailure,bool &RDY_DELAY,int &align)
{
  unsigned short temp_buf;
  FPGADMARead(&temp_buf,1,add_ETH_synchro_STATUS);

  PhaseOptValue=(unsigned int) (temp_buf& 0x1F);
  PhasingDone=(temp_buf & (1<<5))?true:false;
  CharDone   =(temp_buf & (1<<6))?true:false;
  CharFailure=(temp_buf & (1<<7))?true:false;
  RDY_DELAY=(temp_buf & (1<<8))?true:false;
  align=(temp_buf>>11) & 0x03;
  return true;
}
//---------------------------------------------------------------------------
bool TSTU_API::GetJitterStatus(bool &TestingDone,bool &JitterError)
{
  unsigned short temp_buf;
  FPGADMARead(&temp_buf,1,add_ETH_synchro_STATUS);

  JitterError   =(temp_buf & (1<<9))?true:false;
  TestingDone=(temp_buf & (1<<10))?true:false;
  return true;
}

//---------------------------------------------------------------------------
bool TSTU_API::WriteLoopbackBuffer(unsigned short *src_buffer,int length)
{
  FPGADMAWrite(src_buffer,length,add_ETH_WR_loopback);
  return true;
}
//---------------------------------------------------------------------------
/**Set L0-L1 time width.
*/
void TSTU_API::setL0ProcessingTime(unsigned short value)
{
  if (value>511)
  {
    THardwareAccess HardwareAccess("L0 processing value too large");
    throw (HardwareAccess);
  }

  FPGADMAWrite(&value,1,add_ETH_L1A_WINDOW);
}

//---------------------------------------------------------------------------
void TSTU_API::SendPulse(unsigned short mask)
{
  if ( (mask & (L1a | L2a | L2r)) !=0	&& (CTLWORD_static & UseFakeTrigger) ==0)
  {
    THardwareAccess HardwareAccess("Cannot send fake trigger\n UseFakeTrigger bit is not set");
    throw (HardwareAccess);
  }

  CTLWORD_pulse=mask;

  FPGADMAWrite(&CTLWORD_pulse,1,add_ETH_CTRL_pulse);
}
//---------------------------------------------------------------------------
bool TSTU_API::GetDDLStatus(bool &RDYRX,bool &EOBTR,short &transactionID)
{
  unsigned short temp_buf;
  FPGADMARead(&temp_buf,1,add_ETH_DDL);

  RDYRX= (temp_buf & 1)?true:false;
  EOBTR= (temp_buf & 2)?true:false;
  transactionID= (short)((temp_buf >>2)& 0xF);

  return true;
}
//---------------------------------------------------------------------------
void TSTU_API::GetDDLWORD(unsigned int &DDL_WORD)
{
  unsigned short temp_buf;
  FPGADMARead(&temp_buf,1,add_ETH_DDL_WORD_LSB);
  DDL_WORD=(unsigned int)temp_buf;

  FPGADMARead(&temp_buf,1,add_ETH_DDL_WORD_MSB);
  DDL_WORD|=(unsigned int)(temp_buf<<16);

}
//---------------------------------------------------------------------------
void TSTU_API::SetFlashAddress (unsigned long add)
{
  unsigned short temp;

  temp=(unsigned short) (add & 0xFFFF);
  ISPFLASHDMAWrite(&temp,1,add_ETH_FLASH_ADD_LSB);

  temp=(unsigned short) ((add>>16) & 0xFFFF);
  ISPFLASHDMAWrite(&temp,1,add_ETH_FLASH_ADD_MSB);
}
//---------------------------------------------------------------------------
unsigned short TSTU_API::FlashRead(unsigned long add)
{	
  unsigned short temp_buf;
  SetFlashAddress(add);
  ISPFLASHDMARead(&temp_buf,1,add_ETH_FLASH_ACCESS);
  return temp_buf;
}
//---------------------------------------------------------------------------
void TSTU_API::FlashWrite(unsigned long add,unsigned short data)
{
  SetFlashAddress(add);
  ISPFLASHDMAWrite(&data,1,add_ETH_FLASH_ACCESS);
}
//---------------------------------------------------------------------------
void TSTU_API::SetISPFLASHMODE(bool mux_to_ETH, bool trigger_configuration,bool do_reset)
{
  unsigned short temp_buf;

  temp_buf=(mux_to_ETH)?1:0;
  temp_buf=(trigger_configuration)?temp_buf|(1<<1):temp_buf;
  temp_buf=(do_reset)?temp_buf:temp_buf|(1<<2);
  ISPFLASHDMAWrite(&temp_buf,1,add_ETH_CTRL_REG);
}
//---------------------------------------------------------------------------
void TSTU_API::GetISPFLASHStatus(bool &FlashBusy,bool &DONE_ACTEL,bool &INIT_ACTEL,bool &EN_SELECTMAP)
{
  unsigned short temp_buf;
  ISPFLASHDMARead(&temp_buf,1,add_ETH_STATUS_REG);

  FlashBusy    =(temp_buf & 0x0001)?true:false;
  DONE_ACTEL   =(temp_buf & (1<<1))?true:false;
  INIT_ACTEL   =(temp_buf & (1<<2))?true:false;
  EN_SELECTMAP =(temp_buf & (1<<3))?true:false;
}
//---------------------------------------------------------------------------
void TSTU_API::WriteIspflashCtrl(unsigned short &value)
{
  ISPFLASHDMAWrite(&value,1,add_ETH_CTRL_REG);
}
//---------------------------------------------------------------------------
void TSTU_API::ReadIspflashCtrl(unsigned short &value)
{
  ISPFLASHDMARead(&value,1,add_ETH_CTRL_REG);
}
//---------------------------------------------------------------------------
/**Return flash o read mode.
  This function places the flash in the Read mode described
  in the Data Sheet. In this mode the flash can be read as normal memory.
  */
void TSTU_API::FlashReadReset()
{
  /* Step 1: write command sequence */
  FlashWrite(0x555, 0xAA );  /* 1st Cycle */
  FlashWrite(0x2AA, 0x55 );  /* 2nd Cycle */
  FlashWrite(ANY_ADDR, 0xF0 ); /* 3rd Cycle: write 0xF0 to ANY address */

  /* Step 2: wait 10us */
  //FlashPause( 10 );
}
//---------------------------------------------------------------------------
void TSTU_API::FlashPause(unsigned int uMicroSeconds)
{
  clock_t clkReset,clkCounts;

  /* Step 1: Initialize clkReset variable */
  clkReset=clock();

  /* Step 2: Count to the required size */
  do
  {	
    clkCounts = clock()-clkReset;
  }
  while (clkCounts < ((CLOCKS_PER_SEC/1e6L)*uMicroSeconds));

}
//---------------------------------------------------------------------------
/** Reads the flash electronic signature.
  This function can be used to read the electronic signature of the
  device, the manufacturer code or the protection level of a block.

Arguments:     iFunc should be set to either the Read Signature values or to the
block number. The header file defines the values for reading the Signature.
Note: the first block is Block 0

Return Value:  When iFunc is >= 0 the function returns FLASH_BLOCK_PROTECTED
(01h) if the block is protected and FLASH_BLOCK_UNPROTECTED (00h) if it is
unprotected. See the Auto Select command in the Data Sheet for further 
information.

When iFunc is FLASH_READ_MANUFACTURER (-2) the function returns the
manufacturer's code. The Manufacturer code for ST is 20h.

When iFunc is FLASH_READ_DEVICE_CODE (-1) the function returns the Device
Code.  The device codes for the parts are:
M29F040B   E2h
M29W040B   E3h

When iFunc is invalid the function returns FLASH_BLOCK_INVALID (-5)

Pseudo Code:
Step 1:  Send the Auto Select command to the device
Step 2:  Read the required function from the device.
Step 3:  Return the device to Read mode.

*/
int TSTU_API::FlashAutoSelect(int iFunc)
{
  int iRetVal; /* Holds the return value */

  /* Step 1: Send the Auto Select command */
  FlashWrite(0x555, 0xAA );  /* 1st Cycle */
  FlashWrite(0x2AA, 0x55 );  /* 2nd Cycle */
  FlashWrite(0x555, 0x90 );  /* 3rd Cycle */

  /* Step 2: Read the required function */
  if(iFunc == FLASH_READ_MANUFACTURER )
    iRetVal = (int) FlashRead( 0x0000L ); /* A0 = A1 = 0      =00*/

  else if( iFunc == FLASH_READ_DEVICE_CODE )
    iRetVal = (int) FlashRead( 0x0001L ); /* A0 = 1, A1 = 0   =01 */

  else if( (iFunc >= 0) && (iFunc < NUM_BLOCKS) )	//sector protect verify
    iRetVal = FlashRead( BlockOffset[iFunc] + 0x0002L );
  /* A0 = 0, A1 = 1   =10*/
  else
    iRetVal = FLASH_BLOCK_INVALID;

  /* Step 3: Return to Read mode */
  FlashWrite(ANY_ADDR,0xF0 ); /* Use single instruction cycle method */

  return iRetVal;
}
//---------------------------------------------------------------------------
/**Block erase function.
  The user's array, ucBlock[] is used to report errors on the specified
  blocks. If a time-out occurs because the MPU is too slow then the blocks
  in ucBlocks which are not erased are overwritten with FLASH_BLOCK_NOT_ERASED
  (FFh) and the function returns FLASH_MPU_TOO_SLOW.
  If an error occurs during the erasing of the blocks the function returns
  FLASH_ERASE_FAIL.
  If both errors occur then the function will set the ucBlock array to
  FLASH_BLOCK_NOT_ERASED for the unerased blocks. It will return
  FLASH_ERASE_FAIL even though the FLASH_MPU_TOO_SLOW has also occurred.

Description:  This function erases up to ucNumBlocks in the flash. The blocks
can be listed in any order. The function does not return until the blocks are
erased. If any blocks are protected or invalid none of the blocks are erased.

During the Erase Cycle the Data Toggle Flow Chart of the Data Sheet is
followed. The polling bit, DQ7, is not used.

Pseudo Code:
- Step 1:  Check for correct flash type
- Step 2:  Check for protected or invalid blocks
- Step 3:  Write Block Erase command
- Step 4:  Check for time-out blocks
- Step 5:  Wait for the timer bit to be set.
- Step 6:  Follow Data Toggle Flow Chart until Program/Erase Controller has completed
- Step 7:  Return to Read mode (if an error occurred)

@param ucNumBlocks: holds the number of blocks in the array ucBlock
@param ucBlock: is an array containing the blocks to be erased.
@return: The function returns the following conditions:
FLASH_SUCCESS              (-1)
FLASH_TOO_MANY_BLOCKS      (-3)
FLASH_MPU_TOO_SLOW         (-4)
FLASH_WRONG_TYPE           (-8)
FLASH_ERASE_FAIL           (-14)
Number of the first protected or invalid block
*/
unsigned int TSTU_API::FlashBlockErase(unsigned int ucNumBlocks, unsigned int ucBlock[] )
{
  unsigned int ucCurBlock;    /* Range Variable to track current block */
  int iRetVal = FLASH_SUCCESS; /* Holds return value: optimistic initially! */

  /* Step 1: Check for correct flash type */
  if( !(FlashAutoSelect( FLASH_READ_MANUFACTURER ) == MANUFACTURER_ID)
      ||  !(FlashAutoSelect( FLASH_READ_DEVICE_CODE ) == DEVICE_ID ) )
    return FLASH_WRONG_TYPE;

  /* Step 2: Check for protected or invalid blocks. */
  if( ucNumBlocks > NUM_BLOCKS )     /* Check specified blocks <= NUM_BLOCKS */
    return FLASH_TOO_MANY_BLOCKS;

  for( ucCurBlock = 0; ucCurBlock < ucNumBlocks; ucCurBlock++ )
  {
    /* Use FlashAutoSelect to find protected or invalid blocks*/
    if( FlashAutoSelect(ucBlock[ucCurBlock]) != FLASH_BLOCK_UNPROTECTED )
      return ucBlock[ucCurBlock];  /* Return protected/invalid blocks and exit */
  }

  /* Step 3: Write Block Erase command */
  FlashWrite( 0x555, 0xAA );
  FlashWrite( 0X2AA, 0x55 );
  FlashWrite( 0x555, 0x80 );
  FlashWrite( 0x555, 0xAA );
  FlashWrite( 0X2AA, 0x55 );
  /* DSI!: Time critical section. Additional blocks must be added every 50us */
  for( ucCurBlock = 0; ucCurBlock < ucNumBlocks; ucCurBlock++ )
  {
    FlashWrite( BlockOffset[ucBlock[ucCurBlock]], 0x30 );

    /* Check for Erase Timeout Period (is bit DQ3 set?) */
    if( (FlashRead( BlockOffset[ucBlock[0]] ) & 0x08) == 0x08 )
      break; /* Cannot set any more blocks due to timeout */
  }

  /* Step 4: Check for time-out blocks */
  if( ucCurBlock < ucNumBlocks )
  {
    /* Indicate that some blocks have been timed out of the erase list */
    iRetVal = FLASH_MPU_TOO_SLOW; 

    /* Now specify all other blocks as not being erased */
    /* Note that we cannot tell if the first (potentially timed out) block is
       erasing or not */
    while( ucCurBlock < ucNumBlocks )
    {
      ucBlock[ucCurBlock++] = FLASH_BLOCK_NOT_ERASED;
    }
  }

  /* Step 5: Wait for the Erase Timer Bit (DQ3) to be set */
  time_t start,end;
  time (&start);
  double dif;

  while(1)
  {
    if( ( FlashRead( BlockOffset[ucBlock[0]] ) & 0x08 ) == 0x08 )
      break; /* Break when device starts the erase cycle */
    time (&end);
    dif = difftime (end,start);

    if (dif>1.0)
    {
      iRetVal = FLASH_ERASE_FAIL;
      return iRetVal;
    }
  }

  timeout_val=100.0;
  /* Step 6: Follow Data Toggle Flow Chart until Program/Erase Controller 
     completes */
  if( FlashDataToggle() != FLASH_SUCCESS )
  {    
    iRetVal = FLASH_ERASE_FAIL;
    /* Step 7: Return to Read mode (if an error occurred) */
    FlashReadReset(); 
  }

  return iRetVal;
}
//---------------------------------------------------------------------------
/**Flash chip erase function.
Description: The function can be used to erase the whole flash chip so long as
no blocks are protected. If any blocks are protected then nothing is
erased.
Return Value: On success the function returns FLASH_SUCCESS (-1)
If a block is protected then the function returns the number of the block and
no blocks are erased.
If the erase algorithms fails then the function returns FLASH_ERASE_FAIL (-2)
If the wrong type of flash is detected then FLASH_WRONG_TYPE (-8) is
returned.

Pseudo Code:
- Step 1: Check for correct flash type
- Step 2: Check that all blocks are unprotected
- Step 3: Send Chip Erase Command
- Step 4: Follow Data Toggle Flow Chart until Program/Erase Controller has completed.
- Step 5: Return to Read mode (if an error occurred)
 *******************************************************************************/
int TSTU_API::FlashChipErase()
{
  unsigned int ucCurBlock; /* Used to track the current block in a range */

  /* Step 1: Check for correct flash type */
  if( !(FlashAutoSelect( FLASH_READ_MANUFACTURER ) == MANUFACTURER_ID)
      ||  !(FlashAutoSelect( FLASH_READ_DEVICE_CODE ) == DEVICE_ID ) )
    return FLASH_WRONG_TYPE;

  /* Step 2: Check that all blocks are unprotected */
  for( ucCurBlock = 0; ucCurBlock < NUM_BLOCKS; ucCurBlock++ )
  {

    if( FlashAutoSelect( (int)ucCurBlock ) != FLASH_BLOCK_UNPROTECTED )
    {
      ucCurBlock=ucCurBlock;
      //return (int)ucCurBlock;  // Return the first protected block
    }

  }

  /* Step 3: Send Chip Erase Command */
  FlashWrite( 0x555, 0xAA );
  FlashWrite( 0x2AA, 0x55 );
  FlashWrite( 0x555, 0x80 );
  FlashWrite( 0x555, 0xAA );
  FlashWrite( 0x2AA, 0x55 );
  FlashWrite( 0x555, 0x10 );

  /* Step 4: Follow Data Toggle Flow Chart until Program/Erase Controller has
     completed */
  timeout_val=100.0;
  if( FlashDataToggle() != FLASH_SUCCESS )
  {
    /* Step 5: Return to Read mode (if an error occurred) */
    FlashReadReset();
    return FLASH_ERASE_FAIL;
  }
  else
    return FLASH_SUCCESS;
}


//---------------------------------------------------------------------------
/**Flash programmation function
Description: This function is used to program an array into the flash. It does
not erase the flash first and may fail if the block(s) are not erased first.
On success the function returns FLASH_SUCCESS (-1).
The function returns FLASH_PROGRAM_FAIL (-6) if a programming failure occurs.
If the address range to be programmed exceeds the address range of the Flash 
Device the function returns FLASH_OFFSET_OUT_OF_RANGE (-7) and nothing is 
programmed.
If the wrong type of flash is detected then FLASH_WRONG_TYPE (-8) is
returned and nothing is programmed.
If part of the address range to be programmed falls within a protected block,
the function returns the number of the first protected block encountered and
nothing is programmed.

Pseudo Code:
- Step 1: Check for correct flash type
- Step 2: Check the offset range is valid
- Step 3: Check that the block(s) to be programmed are not protected
- Step 4: While there is more to be programmed
- Step 5:  Check for changes from '0' to '1'
- Step 6:  Program the next byte
- Step 7:  Follow Data Toggle Flow Chart until Program/Erase Controller has completed
- Step 8:  Return to Read mode (if an error occurred)
- Step 9:  Update pointers

@param ulOff: is the byte offset into the flash to be programmed
@param NumBytes: holds the number of bytes in the array.
@param Array: is a pointer to the array to be programmed.

@return  FLASH_SUCCESS              (-1)
@return  FLASH_PROGRAM_FAIL         (-6)
@return  FLASH_OFFSET_OUT_OF_RANGE  (-7)
@return  FLASH_WRONG_TYPE           (-8)
@return  Number of the first protected or invalid block
 *******************************************************************************/
int TSTU_API::FlashProgram( unsigned int ulOff, unsigned int NumBytes, unsigned short *Array )
{
  unsigned short *ucArrayPointer; /* Use an unsigned char to access the array */
  unsigned int ulLastOff;       /* Holds the last offset to be programmed */
  unsigned int ucCurBlock;      /* Range Variable to track current block */

  /* Step 1: Check for correct flash type */
  if( !(FlashAutoSelect( FLASH_READ_MANUFACTURER ) == MANUFACTURER_ID)
      ||  !(FlashAutoSelect( FLASH_READ_DEVICE_CODE ) == DEVICE_ID ) )
    return FLASH_WRONG_TYPE;

  /* Step 2: Check the offset and range are valid */
  ulLastOff = ulOff+NumBytes-1;
  if( ulLastOff >= FLASH_SIZE )
    return FLASH_OFFSET_OUT_OF_RANGE;

  /* Step 3: Check that the block(s) to be programmed are not protected */
  for( ucCurBlock = 0; ucCurBlock < NUM_BLOCKS; ucCurBlock++ )
  {
    /* If the address range to be programmed ends before this block */
    if( BlockOffset[ucCurBlock] > ulLastOff )
      break;                   /* then we are done */
    /* Else if the address range starts beyond this block */
    else if( (ucCurBlock < (NUM_BLOCKS-1)) && (ulOff >= BlockOffset[ucCurBlock+1]) )
      continue;                /* then skip this block */
    /* Otherwise if this block is not unprotected */
    else if( FlashAutoSelect((int)ucCurBlock) != FLASH_BLOCK_UNPROTECTED )
      return (int)ucCurBlock;  /* Return first protected block */
  }

  /* Step 3bis: Issue the UnlockBypass command if available */
  FlashUnlockBypass();

  /* Step 4: While there is more to be programmed */
  ucArrayPointer = (unsigned short *)Array;
  while( ulOff <= ulLastOff )
  {
    /* Step 5: Check for changes from '0' to '1' */
    if( ~FlashRead( ulOff ) & *ucArrayPointer ) /* logical AND, between negated read value and the data tp write*/
      /* Indicate failure as it is not possible to change a '0' to a '1'
         using a Program command. This must be done using an Erase command */
      return FLASH_PROGRAM_FAIL;

    /* Step 6: Program the next byte */
    FlashWrite( ANY_ADDR, 0xA0 );  /* 1st cycle */
    FlashWrite( ulOff, *ucArrayPointer );   /* Program value */

    /* Step 7: Follow Data Toggle Flow Chart until Program/Erase Controller
       has completed */
    /* See Data Toggle Flow Chart of the Data Sheet */
    timeout_val=0.1;
    if( FlashDataToggle() == FLASH_TOGGLE_FAIL )
    {
      /* Step 8: Return to Read mode (if an error occurred) */
      FlashReadReset();
      return FLASH_PROGRAM_FAIL;
    }

    /*teste la programmation courante*/
    /*
       unsigned char ReadData;
       ReadData=FlashRead( ulOff );
       if( ReadData!= *ucArrayPointer ) {
       AnsiString lpText = "Difference at offset 0x" + IntToHex((int)ulOff,0);
       lpText=lpText+ "\nExpected data = 0x" + IntToHex((int)*ucArrayPointer,0);
       lpText=lpText+ "\nRead data = 0x" + IntToHex((int)ReadData,0);
       MessageBox(NULL,lpText.c_str(),NULL,MB_OK | MB_ICONSTOP | MB_APPLMODAL);
       return FLASH_PROGRAM_FAIL;
       }
       */

    /* Step 10: Update pointers */
    ulOff++;
    ucArrayPointer++;
  }
  return FLASH_SUCCESS;
}
//---------------------------------------------------------------------------
/** Reads the flash data toggle.
  The function is used to monitor the Program/Erase Controller during
  erase or program operations. It returns when the Program/Erase Controller has
  completed. In the M29F040B Data Sheet or the M29W040B Data Sheet the Data 
  Toggle Flow Chart shows the operation of the function.
  @return The function returns FLASH_SUCCESS if the Program/Erase Controller
  is successful or FLASH_TOGGLE_FAIL if there is a problem.

  Pseudo Code:
  Step 1: Read DQ6 (into a byte)
  Step 2: Read DQ5 and DQ6 (into another byte)
  Step 3: If DQ6 did not toggle between the two reads then return FLASH_SUCCESS
  Step 4: Else if DQ5 is zero then operation is not yet complete, goto 1
  Step 5: Else (DQ5 != 0), read DQ6 again
  Step 6: If DQ6 did not toggle between the last two reads then return 
  FLASH_SUCCESS
  Step 7: Else return FLASH_TOGGLE_FAIL
 *******************************************************************************/
int TSTU_API::FlashDataToggle( void )
{
  unsigned short uc1, uc2; /* hold values read from any address offset within 
                              the Flash Memory */

  time_t start,end;
  double dif;
  time (&start);

  while(1) 
  {
    /* Step 1: Read DQ6 (into a byte) */
    uc1 = FlashRead( ANY_ADDR );   /* Read DQ6 from the Flash (any address) */

    /* Step 2: Read DQ5 and DQ6 (into another byte) */
    uc2 = FlashRead( ANY_ADDR );   /* Read DQ5 and DQ6 from the Flash (any 
                                      address) */

    /* Step 3: If DQ6 did not toggle between the two reads then return 
       FLASH_SUCCESS */
    if( (uc1&0x40) == (uc2&0x40) ) /* DQ6 == NO Toggle  */
      return FLASH_SUCCESS;

    /* Step 4: Else if DQ5 is zero then operation is not yet complete */
    if( (uc2&0x20) == 0x00 )
      continue;

    /* Step 5: Else (DQ5 == 1), read DQ6 again */
    uc1 = FlashRead( ANY_ADDR );   /* Read DQ6 from the Flash (any address) */

    /* Step 6: If DQ6 did not toggle between the last two reads then
       return FLASH_SUCCESS */
    if( (uc2&0x40) == (uc1&0x40) ) /* DQ6 == NO Toggle  */
      return FLASH_SUCCESS;

    /* Step 7: Else return FLASH_TOGGLE_FAIL */
    else                           /* DQ6 == Toggle here means fail */
      return FLASH_TOGGLE_FAIL;

    time (&end);
    dif = difftime (end,start);
    if (dif>timeout_val)
      return FLASH_TOGGLE_FAIL;
  }  /* end of while loop */
}
//---------------------------------------------------------------------------
/**Returns the message string.
Description:  This function is used to generate a text string describing the
error from the flash. Call with the return value from another flash routine.

Pseudo Code:
Step 1: Check the error message range.
Step 2: Return the correct string.
*/
const char* TSTU_API::FlashErrorStr( int iErrNum )
{
  //char *str[] = { "Flash Success",
  string str[] = { "Flash Success",
    "Flash Poll Failure",
    "Flash Too Many Blocks",
    "MPU is too slow to erase all the blocks",
    "Flash Block selected is invalid",
    "Flash Program Failure",
    "Flash Address Offset Out Of Range",
    "Flash is of Wrong Type",
    "Flash Block Failed Erase",
    "Flash is Unprotected",
    "Flash is Protected",
    "Flash function not supported",
    "Flash Vpp Invalid",
    "Flash Erase Fail",
    "Flash Toggle Flow Chart Failure"};

  /* Step 1: Check the error message range */
  iErrNum = -iErrNum - 1;   /* All errors are negative: make +ve & adjust */

  if( iErrNum < 0 || iErrNum >= (signed) (sizeof(str)/sizeof(str[0]))) /* Check range */
    return "Unknown Error\n";

  /* Step 2: Return the correct string */
  else
    return str[iErrNum].c_str();
}
/**The function is used to unlock the Bypass Program command and save time .*/
void TSTU_API::FlashUnlockBypass( void )
{
  FlashWrite(0x555, 0xAA);  /* 1st cycle */
  FlashWrite(0x2AA, 0x55);  /* 2nd cycle */
  FlashWrite(0x555, 0x20);  /* 3rd cycle */
}
//---------------------------------------------------------------------------
/**The function is used to return to reading array time.*/
void TSTU_API::FlashUnlockBypassReset( void )
{
  FlashWrite(ANY_ADDR, 0x90);  /* 1st cycle */
  FlashWrite(ANY_ADDR, 0x00);  /* 2nd cycle */
}
//---------------------------------------------------------------------------
/**The function is used to detect the flash info.
  @return FLASH_WRONG_TYPE upon failure
  @return FLASH_SUCCESS in case of success
  */
int TSTU_API::FlashDetect(void)
{
  if( !(FlashAutoSelect( FLASH_READ_MANUFACTURER ) == MANUFACTURER_ID)
      ||  !(FlashAutoSelect( FLASH_READ_DEVICE_CODE ) == DEVICE_ID ) )
    return FLASH_WRONG_TYPE;
  else
    return FLASH_SUCCESS;
}
//---------------------------------------------------------------------------
/**Transfer the FPGA configuration file in the flash memory.
*/
void TSTU_API::ConfigureFlash(string ConfFile)
{
  unsigned short ctrl_value;

  ctrl_value=1;
  WriteIspflashCtrl(ctrl_value);


  if (FlashDetect()!=FLASH_SUCCESS)
  {
    THardwareAccess HardwareAccess("Flash not detected");
    throw (HardwareAccess);
  }

  if (!PutConfigToMem(ConfFile))
  {
    THardwareAccess HardwareAccess("FPGA file not found");
    throw (HardwareAccess);
  }

  if (FlashChipErase()!=FLASH_SUCCESS)
  {
    THardwareAccess HardwareAccess("Flash not successfully cleared");
    throw (HardwareAccess);
  }

  int ret_val=FlashProgram(0,ConfigDataIndice/2, (unsigned short *)ConfigData);

  if (ret_val!=FLASH_SUCCESS)
  {
    string Message=FlashErrorStr(ret_val);
    THardwareAccess HardwareAccess("Flash not successfully progammed" + Message);
    throw (HardwareAccess);
  }
}

//---------------------------------------------------------------------------
/**The function request the embedded microcontroller to check the flash signature.*/
bool TSTU_API::EmbeddedFlashDetect()
{
  instruction[0]=INST_FLASH_DETECT;

  if(!STU_mezzanine.SendData(instruction,1))
  {
    THardwareAccess HardwareAccess("Embedded flash detect request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.ReceiveData(instruction,1))
  {
    THardwareAccess HardwareAccess("Embedded flash status request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if (instruction[0]==FLASH_SUCCESS)
    return true;
  else
  {
    cout <<"Flash detect failure" << endl;
    return false;
  }
}
//---------------------------------------------------------------------------
/**The function request the embedded microcontroller to check the flash signature.*/
bool TSTU_API::EmbeddedFlashErase()
{
  instruction[0]=INST_FLASH_ERASE;

  if(!STU_mezzanine.SendData(instruction,1))
  {
    THardwareAccess HardwareAccess("Embedded flash detect request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.ReceiveData(instruction,1))
  {
    THardwareAccess HardwareAccess("Embedded flash status request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if (instruction[0]==FLASH_SUCCESS)
  {
    cout << "Flash erased" << endl;
    return true;
  }
  else
  {
    cout <<"Flash erase request failure" << endl;
    return false;
  }
}
//---------------------------------------------------------------------------
/**The function request the embedded microcontroller to check the flash signature.*/
bool TSTU_API::EmbeddedConfigureFlash(string ConfFile)
{

  if (!PutConfigToMem(ConfFile))
  {
    THardwareAccess HardwareAccess("FPGA file not found");
    throw (HardwareAccess);
  }

  instruction[0]=INST_FLASH_PROGRAM;
  instruction[1]=(char)(ConfigDataIndice>>24);
  instruction[2]=(char)(ConfigDataIndice>>16);
  instruction[3]=(char)(ConfigDataIndice>>8);
  instruction[4]=(char)(ConfigDataIndice>>0);

  if(!STU_mezzanine.SendData(instruction,5))
  {
    THardwareAccess HardwareAccess("Flash program request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if(!STU_mezzanine.SendData(ConfigData,ConfigDataIndice))
  {
    THardwareAccess HardwareAccess("Flash data transfer: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }


  if(!STU_mezzanine.ReceiveData(instruction,1))
  {
    THardwareAccess HardwareAccess("Embedded flash program status request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  if (instruction[0]==FLASH_SUCCESS)
    return true;
  else
    return false;

}
//---------------------------------------------------------------------------
/**The function request the embedded microcontroller to check the flash signature.*/
void TSTU_API::EmbeddedUploadFlash(string ConfFile,unsigned int byte_count)
{
  instruction[0]=INST_FLASH_UPLOAD;
  instruction[1]=(char)(byte_count>>24);
  instruction[2]=(char)(byte_count>>16);
  instruction[3]=(char)(byte_count>>8);
  instruction[4]=(char)(byte_count>>0);

  if(!STU_mezzanine.SendData(instruction,5))
  {
    THardwareAccess HardwareAccess("Flash program request: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  ConfigData=new char[byte_count];
  if(!STU_mezzanine.ReceiveData(ConfigData,byte_count))
  {
    THardwareAccess HardwareAccess("flash data retrieval: " +STU_mezzanine.GetStatus());
    throw (HardwareAccess);
  }

  ofstream sortie;
  sortie.open(ConfFile.c_str(),ios::out);
  if (!sortie)
  {
    string Message="fichier pas ouvert";
  }
  sortie << hex <<setiosflags(ios_base::uppercase) ;

  unsigned char val_to_print;
  for (unsigned int i=0;i<byte_count;i++)
  {
    val_to_print=ConfigData[i];
    sortie << setw(2)<< setfill('0')<< (int)val_to_print << endl;
  }

  sortie.close();

  delete[] ConfigData;

}
//---------------------------------------------------------------------------
bool TSTU_API::SelectmapConfFPGA(string ConfigFile)
{
  cout <<"debut conf fpga" << endl;
  FPGAConfigFile=ConfigFile;
  if (!PutConfigToMem(FPGAConfigFile))
    return false;
  cout << "conf data in mem" << endl;

  //unsigned short value;
  bool FlashBusy;
  bool DONE_ACTEL;
  bool INIT_ACTEL;
  bool EN_SELECTMAP;
  //step 1 : put reset low, and generate a prog pulse and mux to ETH
  SetISPFLASHMODE(/*mux_to_ETH*/true,/*trigger_configuration*/true,/*do_reset*/true);

  //step 2 : wait for init high
  for (int i=0;i<20;i++)
  {
    GetISPFLASHStatus(FlashBusy,DONE_ACTEL,INIT_ACTEL,EN_SELECTMAP);
    if (INIT_ACTEL)
      break;
  }

  if(!INIT_ACTEL)
  {
    THardwareAccess HardwareAccess("INIT still low");
    throw (HardwareAccess);
  }
  cout << "end of init" << endl;






  //step 3 send data
  ISPFLASHDMAWrite((unsigned short *)ConfigData,ConfigDataIndice/2,add_ETH_SELECT_MAP);
  delete[] ConfigData;
  cout << "conf data xfered" <<endl;






  //step 4 : check configuration status
  GetISPFLASHStatus(FlashBusy,DONE_ACTEL,INIT_ACTEL,EN_SELECTMAP);

  if (!DONE_ACTEL)
  {
    THardwareAccess HardwareAccess("FPGA configuration failure ");
    throw (HardwareAccess);
  }
  cout << "conf done" << endl;





  //step5 : release reset and switch mux_to_eth
  SetISPFLASHMODE(/*mux_to_ETH*/true,/*trigger_configuration*/false,/*do_reset*/false);
  SetISPFLASHMODE(/*mux_to_ETH*/false,/*trigger_configuration*/false,/*do_reset*/false);






  return true;
}
//---------------------------------------------------------------------------
/**Use this method in order to perform a write to the I2C control register.
  The bit are organised as follows:
  - bit 0: Enable bunch counter operation
  - bit 1: Enable event counter operation
  - bit 2: selClock40Des2
  - bit 3: Enable cloch40Des2 output
  - bit 4: Enable clockL1a output
  - bit 5: Enable parallel output
  - bit 6: Enable serialB output
  - bit 7: Enable (non deskewed) clock 40 output
  */
void TSTU_API::WriteI2CTTC(unsigned char add, unsigned char data)
{
  unsigned short I2Cword=0;
  //set data content
  I2Cword=(unsigned short) data;


  //set target register
  I2Cword|=(add<<8);

  //set write mode
  I2Cword|=(1<<15);
  FPGADMAWrite(&I2Cword,1,add_ETH_TTC_I2C);
}
//---------------------------------------------------------------------------
/** Use this mthod in order to perform a read of the I2C control register.
  @param I2CData : I2C control register value
  @param PLL_locked : status of the VIRTEX5 PLL (builds the 4000MHz,200MHz, and the 66MHz)
  @param QPLL_error : returns error status of the QPLL
  @param QPLL_locked : returns locking status of the QPLL
  @param TTC_ready : returns  status of the TTC chip
  @param I2C_avail : false when I2CData is meaningless
  @param I2CData : I2C control register value

  @see WriteI2CTTC(char data)
  */
void TSTU_API::ReadTTC(unsigned short add,unsigned short &I2CData,bool &PLL_locked,bool &QPLL_error,bool &QPLL_locked,bool &TTC_ready,bool &I2C_avail)
{
  unsigned short I2Cword=0;
  //set target register
  I2Cword|=(add<<8);

  FPGADMAWrite(&I2Cword,1,add_ETH_TTC_I2C);

  FPGADMARead(&I2Cword,1,add_ETH_TTC_I2C);

  I2CData= (I2Cword & 0xFF);
  PLL_locked= (I2Cword&(1<<8))?true:false;
  QPLL_error= (I2Cword&(1<<9))?true:false;
  QPLL_locked=(I2Cword&(1<<10))?true:false;
  TTC_ready=  (I2Cword&(1<<11))?true:false;
  I2C_avail=  (I2Cword&(1<<12))?true:false;
}
//---------------------------------------------------------------------------
/**This method reads the last V0 received data.
*/
unsigned int TSTU_API::ReadV0Data()
{
  unsigned short V0data_LSB;
  unsigned short V0data_MSB;

  FPGADMARead(&V0data_LSB,1,add_ETH_V0_DATA_LSB);
  FPGADMARead(&V0data_MSB,1,add_ETH_V0_DATA_MSB);

  return  (V0data_LSB | (V0data_MSB<<16));
}
//---------------------------------------------------------------------------
/**This method reads the last V0 link status.
  @param V0_RXREADY  : Confirms that the GLINK is ready to receive
  @param V0_RXERROR  : Flags that the GLINK received an error (will probably not be seen by DCS, last one CLK)
  @param MOD_PRESENT : Flags that the optical transceiver is plugged
  @param V0_RX_LOSS  : Flags that the optical transceiver has lost the carrier
  */
void TSTU_API::ReadV0LinkStatus(bool &V0_RXREADY,bool &V0_RXERROR,bool &MOD_PRESENT,bool &V0_RX_LOSS)
{
  unsigned short LinkStatus;

  FPGADMARead(&LinkStatus,1,add_ETH_V0_link_status);

  V0_RXREADY= (LinkStatus&0x1)?true:false;
  V0_RXERROR= (LinkStatus&(1<<1))?true:false;
  MOD_PRESENT=(LinkStatus&(1<<3))?true:false;
  V0_RX_LOSS= (LinkStatus&(1<<4))?true:false;
}
//---------------------------------------------------------------------------
/**Reads the error counter and the frame counter.
  (both are 8 bits values)
  */
void TSTU_API::ReadV0FrameAndError(short &FrameCount,short &ErrorCount)
{
  unsigned short counters;

  FPGADMARead(&counters,1,add_ETH_V0_frame_error);

  FrameCount=(counters>>8) & 0xFF;
  ErrorCount=counters & 0xFF;
}
//---------------------------------------------------------------------------
/**Set the value of the random generator.
  The higher the value, the higher the rate
  */
void TSTU_API::setRandomGenerator(int value)
{
  unsigned short XferVal;

  XferVal=value & 0xFFFF;
  FPGADMAWrite(&XferVal,1,add_ETH_TRIG_random_LSB);

  XferVal=(value>>16) & 0xFFFF;
  FPGADMAWrite(&XferVal,1,add_ETH_TRIG_random_MSB);
}
//---------------------------------------------------------------------------
/**Set the delay value of the L0 trigger logic.
  @param value : value of the delay in bucnh clock cycles
  */
/*
   void TSTU_API::setL0Delay(unsigned short value)
   {
   FPGADMAWrite(&value,1,add_ETH_TRIG_L0_delay);
   }
   */
//---------------------------------------------------------------------------
/**Set the delay value of the L1 gamma trigger logic.
  @param value : value of the delay in bucnh clock cycles
  */
void TSTU_API::setL1GammaDelay(unsigned short value)
{
  FPGADMAWrite(&value,1,add_ETH_TRIG_L1gamma_delay);
}
//---------------------------------------------------------------------------
/**Set the delay value of the L1 gamma trigger logic.
  @param value : value of the delay in bucnh clock cycles
  */
//void TSTU_API::setL1JetDelay(unsigned short value)
//{
//  FPGADMAWrite(&value,1,add_ETH_TRIG_L1jet_delay);
//}
//---------------------------------------------------------------------------
/**Select the trigger logic mode.
  @param opcode : mode to choose
  @see TRG_normal
  @see TRG_T
  @see TRG_S
  @see TRG_R
  */
void TSTU_API::setTriggerMode(unsigned short opcode)
{
  FPGADMAWrite(&opcode,1,add_ETH_TRIG_opcode);
}
//---------------------------------------------------------------------------
void TSTU_API::GetMonitorStatus(double temp[3],double vccint[3],double vccaux[3])
{
  unsigned short MonitorStatus[9];

  //reset monitor pointer
  FPGADMARead(MonitorStatus,1,add_ETH_IDCODE0);

  FPGADMARead(MonitorStatus,9,add_ETH_Monitor);
  for (int i =0;i<3;i++)
  {
    temp[i]  = (double) ((MonitorStatus[3*i]*503.975)/1014 - 273.15);
    vccint[i]=(double) (3.0*MonitorStatus[3*i+1]/1024);
    vccaux[i]=(double) (3.0*MonitorStatus[3*i+2]/1024);
  }
}
//---------------------------------------------------------------------------
/**Adjust L0 individual input phase delay.
  This function in conjuction with the jitter measurement to adjust the L0 sampling
  by STU
  */
void TSTU_API::SetL0Delay(int bufferAdd,int delay)
{
  if (delay<0 || delay>31)
  {
    THardwareAccess HardwareAccess("Delay value is out of range");
    throw (HardwareAccess);
  }

  //set value
  SetBufferAddress(bufferAdd);
  SetPhaseValue(delay);
  SetCTLWORD_static();

  //execute change
  DoSetL0Phase();
}
//---------------------------------------------------------------------------
int TSTU_API::FindBestPhase(int add)
{
  bool TestingDone;
  bool JitterError;

  int width=0;
  int offset=0;
  int opt_value;

  bool in_unstable=false;
  unsigned int JitterMask=0;

  //for (int i=0;i<32;i++)
  for (int i=0;i<nb_PORT;i++)//HIROKI
  {
    SetL0Delay(add,i);

    DoMeasJitter();

    do{
      GetJitterStatus(TestingDone,JitterError);
    }while(!TestingDone);

    if (!JitterError)
    {
      JitterMask|=(1<<i);
      width++;
      if (in_unstable)
        offset=i;
      in_unstable=false;
    }
    else
      in_unstable=true;
  }

  cout << "Jitter mask #" << dec << add << " val=0x" << hex << JitterMask << " offset=" << dec << offset << " width=" <<width;

  //best value
  opt_value=(offset+width/2)& 0x1F;
  cout << " opt_value=" << opt_value << endl;

  return opt_value;
}
//---------------------------------------------------------------------------
/**L0 counter read.
  This function is used to read the L0 counter that is incremented upon each L0 reception.
  This used in conjunction with SetL0Delay() is used to select the proper
  sampling edge. The counter read is the one pointed by SerialInputNumber.

*/
unsigned short TSTU_API::ReadL0Count()
{
  unsigned short ReadValue;

  FPGADMARead(&ReadValue,1,add_ETH_L0_count);

  return ReadValue;
}
//---------------------------------------------------------------------------
/** L0 delay read.
  In STU there is a counter that is started by the local L0 , and counts while
  waiting the global_L0 comming from the CTP. For this measurement the TRU are put in test mode, and forward the 
  CTP confirmed to the STU.

  @param SerialInputNumber : serializer input to check
  @see void SetL0Delays(int delays[32])
  */
void TSTU_API::ReadL0LatencyDelay(unsigned short &L0latency,unsigned short &L0_L0_delay)
{
  unsigned short ReadValue;

  FPGADMARead(&ReadValue,1,add_ETH_L0_offset);
  L0latency=((ReadValue>>8) & 0xFF);
  L0_L0_delay=ReadValue & 0xFF;
}
//---------------------------------------------------------------------------
/**Program the gamma threshold parameters.
  The equation is of the following type A*V0�+B*V0+C

*/
void TSTU_API::SetGammaThresParameters(short Al, short Bl, short Cl,short Am, short Bm, short Cm, short Ah, short Bh, short Ch)
{
  unsigned short parameters[9];

  parameters[0]=(unsigned) Ch;
  parameters[1]=(unsigned) Bh;
  parameters[2]=(unsigned) Ah;
  parameters[3]=(unsigned) Cm;
  parameters[4]=(unsigned) Bm;
  parameters[5]=(unsigned) Am;
  parameters[6]=(unsigned) Cl;
  parameters[7]=(unsigned) Bl;
  parameters[8]=(unsigned) Al;

  FPGADMAWrite(parameters,9,add_ETH_gamma_param);
}

bool TSTU_API::ReadMonErrorCount(unsigned int &trial, unsigned int &error)
{
  unsigned short toto[5];
  for(int i=0; i<5; i++){
    usleep(1000);
    FPGADMARead(&toto[i],1,add_ETH_mon_error_rate);
  }
  if(!(toto[0]==0xffff)) return false;
  trial = (toto[1] << 16) + toto[2];
  error = (toto[3] << 16) + toto[4];

  return true;
}

////---------------------------------------------------------------------------
//void TSTU_API::SetJetThresParameters(short Al, short Bl, short Cl,short Ah, short Bh, short Ch)
//{
//  unsigned short parameters[6];
//
//  parameters[0]=(unsigned) Ch;
//  parameters[1]=(unsigned) Bh;
//  parameters[2]=(unsigned) Ah;
//  parameters[3]=(unsigned) Cl;
//  parameters[4]=(unsigned) Bl;
//  parameters[5]=(unsigned) Al;
//
//  FPGADMAWrite(parameters,6,add_ETH_jet_param);
//}
////HIROKI ->
////---------------------------------------------------------------------------
//void TSTU_API::SetPHOSScaleParameters(short A, short B, short C,short D)
//{
//  unsigned short parameters[4];
//
//  parameters[0]=(unsigned) D;
//  parameters[1]=(unsigned) C;
//  parameters[2]=(unsigned) B;
//  parameters[3]=(unsigned) A;
//
//  FPGADMAWrite(parameters,4,add_ETH_subregion_scale_param);
//}
////<- HIROKI
////HIROKI ->
////---------------------------------------------------------------------------
//void TSTU_API::SetPatchSize(unsigned short patch_size)
//{
//  FPGADMAWrite(&patch_size,1,add_ETH_jetpatch_mode);
//}
//<- HIROKI
//---------------------------------------------------------------------------
unsigned short TSTU_API::ReadReceptionPointer(int SerialInputNumber)
{
  unsigned short ReadValue;
  SetBufferAddress(SerialInputNumber);

  FPGADMARead(&ReadValue,1,add_ETH_pointer_level);

  return ReadValue;

}
//---------------------------------------------------------------------------
void TSTU_API::RegionEnable(unsigned int region_mask,unsigned int L0_mask)
{
  unsigned short parameters[4];

  parameters[0]=(unsigned short) (L0_mask & 0xFFFF);
  parameters[1]=(unsigned short) ((L0_mask>>16) & 0xFFFF);
  parameters[2]=(unsigned short) (region_mask & 0xFFFF);
  parameters[3]=(unsigned short) ((region_mask>>16) & 0xFFFF);

  FPGADMAWrite(parameters,4,add_ETH_region_enable);
}
