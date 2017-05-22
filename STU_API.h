#ifndef STU_APIH
#define STU_APIH

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <ctime>
using namespace std;

#include "STU_mezzanine.h"
#include "SocketClient_API.h"

//Static control word  parameters
#define sel_test_mem 1
#define EnableReceptionBuffer 1
#define Enable_L0_over_TTC  (1<<12)
#define GetRaw              (1<<13)
#define UseTestPattern      (1<<14)
#define UseFakeTrigger      (1<<15)

//Bit pulser affectation
#define set_phase_manu  1
#define recompute_phase_bit (1<<1)
#define do_slip_manu        (1<<2)
#define L1a                 (1<<3)
#define L2a                 (1<<4)
#define L2r                 (1<<5)
#define ClearL0Cnt          (1<<6)
#define fake_L0             (1<<7)
#define setL0Phase          (1<<8)
#define measJitter          (1<<9)
#define SOR_reset           (1<<10)
#define clear_pointer_test  (1<<11)
#define busy_clear_bit      (1<<12)

//trigger logic commands
#define TRG_normal  0x00 /*<Normal trigger mode */
#define TRG_T       0x01 /*<Toggling trigger mode */
#define TRG_S       0x02 /*<Signature emission trigger mode (every 25 µs)*/
#define TRG_R       0x03 /*<Random trigger generation mode */

/**Exception class.
Allows to pass error message to the upper level.
*/

//####################################################################################################
class THardwareAccess
{
  public:
    THardwareAccess(string Mess);
    /** Contains the last error Message.*/
    string Message;

};

//####################################################################################################
//!API for accessing the hardware via Ethernet
//  This class call provide all the necessary method required in order to drive the electronic board. That is the
//  FPGA configuration, Register initialisation, ...
class TSTU_API
{
  private:
    /**Socket Client. provide all the necessary function in order to communicate via a TCP socket  
    */
    TSocketClient_API STU_mezzanine;
    string  FPGAConfigFile;
    string  statusMessage;
    char*   ConfigData;
    int     ConfigDataIndice; //<Contains the size of the configuration file*/
    char    instruction[10];
    bool    PutConfigToMem(string src_file);

    //DMA read for FPGA space access.
    //  Since all access are 16 bit, the granularity choseen is unsigned short. The
    //  DCS board being big endian, and most PC (intel) being little endian, the endianness
    //  conversion takes place in the DCS board.
    //  @param dest_buffer : pointer to the destination buffer
    //  @param count : number of 16 bit words to transfer
    //  @param add : offset to the desired buffer
    bool FPGADMARead(unsigned short *dest_buffer, int count, char add);

    //DMA write for FPGA space access.
    //  Since all access are 16 bit, the granularity choseen is unsigned short. The
    //  DCS board being big endian, and most PC (intel) being little endian, the endianness
    //  conversion takes place in the DCS board.
    //  @param src_buffer : pointer to the source buffer
    //  @param count : number of 16 bit words to transfer
    //  @param add : offset to the desired buffer
    bool FPGADMAWrite(unsigned short *src_buffer, int count, char add);

    //DMA read for ISPFLASH space access.
    //  Since all access are 16 bit, the granularity choseen is unsigned short. The
    //  DCS board being big endian, and most PC (intel) being little endian, the endianness
    //  conversion takes place in the DCS board.
    //  @param dest_buffer : pointer to the destination buffer
    //  @param count : number of 16 bit words to transfer
    //  @param add : offset to the desired buffer
    bool ISPFLASHDMARead(unsigned short *dest_buffer, int count, char add);

    //DMA write for ISPFLASH space access.
    //  Since all access are 16 bit, the granularity choseen is unsigned short. The
    //  DCS board being big endian, and most PC (intel) being little endian, the endianness
    //  conversion takes place in the DCS board.
    //  @param src_buffer : pointer to the source buffer
    //  @param count : number of 16 bit words to transfer
    //  @param add : offset to the desired buffer
    bool ISPFLASHDMAWrite(unsigned short *src_buffer, int count, char add);

    unsigned short CTLWORD_static;
    unsigned short CTRLWORD_static2;
    unsigned short CTLWORD_pulse;

    //flash definitions
    unsigned long *BlockOffset;
    unsigned long *BlockOffsetRead;

    //Flash memory hardware access
    void    FlashPause(unsigned int uMicroSeconds );
    int     FlashDataToggle();
    void    FlashReadReset();
    void    FlashUnlockBypass();
    void    FlashUnlockBypassReset();
    double  timeout_val;

  public :
    //Initiate a TCP client connection.
    //  This method has to be called once before any communication attempt to the hardware
    bool Connect();

    //Configure STU FPGA.
    //  When called, the Virtex 5 FPGA embedded on the board is programmed
    bool ConfFPGA();

    //Configure STU FPGA via slave serial. 
    //  When called, the Virtex 5 FPGA embedded on the board is programmed with the specified file,
    //  instead of the default one
    bool ConfFPGA(string ConfigFile);

    //Configure STU FPGA vial slave select map. 
    //  When called, the Virtex 5 FPGA embedded on the board is programmed with the specified file,
    //  instead of the default one
    bool SelectmapConfFPGA(string ConfigFile);

    //Virtex 5 FPGA Communication test.
    //  Try to read 2 words form FPGA IO space.
    //  @param IDCODE0 a short referenced argument
    //  @param IDCODE1 a short referenced argument
    //  @return true upon success
    bool GetIDCODES(unsigned short &IDCODE0,unsigned short &IDCODE1);

    //ispflash FPGA Communication test.
    //  Try to read 2 words form FPGA IO space.
    //  @param IDCODE0 a short referenced argument
    //  @param IDCODE1 a short referenced argument
    void GetIspFlashIDCODES(unsigned short &IDCODE0,unsigned short &IDCODE1);

    //Controls the ISPFLASH mode.
    //  @param mux_to_ETH : When true, the flash memory is working in 16 bit mode and is connected to DCS
    //  @param trigger_configuration : When true initiate a VIRTEX5 configuration and mux_to_ETH must be false
    void SetISPFLASHMODE(bool mux_to_ETH, bool trigger_configuration,bool do_reset);

    //Get ISPFLASH status.
    //  @param FlashBusy: True when flash is busy erasing or programming
    //  @param DONE_ACTEL: True when VIRTEX5 FPGA is configured
    //  @param INIT_ACTEL: False when there is a bitsream error, CRC error (SEU event detected
    //  @param EN_SELECTMAP: True when ISPFLASH is selected for configuring FPGA
    void GetISPFLASHStatus(bool &FlashBusy,bool &DONE_ACTEL,bool &INIT_ACTEL,bool &EN_SELECTMAP);

    //Set the Static control word.
    //  Using this command affects :
    //  - Bit 0 : the reset of the reception buffer (unused now)
    //  - Bit (5..1) : Value of the manual setting of the phase
    //  - Bit (11..6) : Address of the selected buffer
    //  - Bit 12 : Enable_L0_over_TTC (selection between direct LVDS connection or via TTC).
    //  - Bit 13 : GetRaw, for getting or not the primitive triggering data via the DDL.
    //  - Bit 14 : UseTestPattern, for using the testing pattern instead of real data.
    //  - Bit 15 : UseFakeTrigger.
    bool SetCTLWORD_static(unsigned short &value);
    bool SetCTLWORD_static();

    //Clear FIFO by setting appropriate bit in CTLWORD_static.
    //  After using this method a call to SetCTLWORD_static() is required
    //  for applying the changes.   This function is used only in the TRU_STU validation bench.
    //  @see SetCTLWORD_static()
    void DoClearReceptionFIFO();

    //Reenable FIFO by setting appropriate bit in CTLWORD_static.
    //  After using this method a call to SetCTLWORD_static() is required
    //  for applying the changes. This function is used only in the TRU_STU validation bench.
    //  @see SetCTLWORD_static()
    void DoEnableReceptionFIFO();

    //Set the phase manual calue bit in CTLWORD_static.
    //  After using this method a call to SetCTLWORD_static() and to ApplyPhaseManu()  is required
    //  for applying the changes. The change is applied to the buffer selected by SetBufferAddress(int val).
    //  @param val : delay value to apply range from 0 to 31
    //  @see SetCTLWORD_static()
    //  @see ApplyPhaseManu()
    //  @see SetBufferAddress(int val)
    void SetPhaseValue(int val);

    //Select the buffer to read and the desrializer to control by setting appropriate bit in CTLWORD_static.
    //  After using this method a call to SetCTLWORD_static() is required
    //  for applying the changes.
    //  @param val : Selects the nb_TRU address of the deserializer and the loopback buffer (nb_TRU+1)
    //  @see SetCTLWORD_static()
    //  @see nb_TRU
    void SetBufferAddress(int val);

    //Read back of the control word.
    //  This function is used only in the TRU_STU validation bench.
    //  @see SetCTLWORD_static(unsigned short &value)
    bool CheckCTLWORD_static(unsigned short &value);

    //Request the selected FPGA deserializer to apply the manual phase value.
    //  @see SetPhaseValue(int val)
    //  @see SetBufferAddress(int val)
    void SetRecordOnL0(bool val);
    bool ApplyPhaseManu();

    void SelTestPatternMode(bool);

    bool load_test_memory(const char*pfile);
    void build_test_pattern();

    //Request the selected FPGA deserializer to automatically search for its best phasing.
    //  @see SetBufferAddress(int val)
    bool DoRecomputePhase();

    //Request the selected FPGA deserializer to skip on character in its framing.
    //  @see SetBufferAddress(int val)
    bool DoSlipManu();

    //Request the selected FPGA deserializer to change the delay on L0 input.
    //  @see SetBufferAddress(int val)
    bool DoSetL0Phase();

    //Request the selected FPGA deserializer to measure jitter.
    //  @see SetBufferAddress(int val)
    bool DoMeasJitter();

    //returns the fill level of the deserializer FIFO.
    //  This function is used only in the TRU_STU validation bench.
    //  @see SetBufferAddress(int val)
    unsigned short GetBufferStatus();

    //Read the pointed buffer.
    //  This function is used only in the TRU_STU validation bench.
    //  @param dest_buffer : address of the destination buffer
    //  @param length : number of word to read
    //  @see SetBufferAddress(int val)
    bool ReadDeserBuffer(unsigned short *dest_buffer,int length);

    //Write the loopback buffer.
    //  @param length : number of word to read
    bool WriteLoopbackBuffer(unsigned short *src_buffer,int length);

    //Returns the mem delay buffer image of the selected deserializer.
    //  @see SetBufferAddress(int val)
    unsigned int GetMemDelayImage();

    unsigned short ReadErrorCount();
    unsigned short GetErrorCount(int add);
    void GetErrorCount(unsigned short *count_table);

    //Returns the selected deserializer status.
    //  @param PhaseOptValue : Optimal phase value computed by the FPGA
    //  @param PhasingDone : true when phasing search is done and successful
    //  @param CharDone : true when pattern finding is successful
    //  @param CharFailure : true when pattern finding failed
    //  @param RDY_DELAY : true when delay control element is locked and working
    //  @see SetBufferAddress(int val)
    bool GetDeserStatus(int &PhaseOptValue,bool &PhasingDone,bool &CharDone,bool &CharFailure,bool &RDY_DELAY,int &align);

    //Returns the selected deserializer jitter status.
    //  @param PhaseOptValue : Optimal phase value computed by the FPGA
    //  @param PhasingDone : true when phasing search is done and successful
    //  @param CharDone : true when pattern finding is successful
    //  @param CharFailure : true when pattern finding failed
    //  @param RDY_DELAY : true when delay control element is locked and working
    //  @see SetBufferAddress(int val)
    bool GetJitterStatus(bool &TestingDone,bool &JitterError);

    //Reset the whole electronics board through the DCS board.The registers and FIFO are cleaned, however the FPGA configuration is not
    //  erased.
    bool DCSResetBoard();

    //Reset the whole electronics board through the ACTEL FPGA.The registers and FIFO are cleaned, however the FPGA configuration is not
    //  erased.
    void ACTELResetBoard();

    void ResetTTConly();

    //initiate a fake trigger form DCS.
    //  The fake trigger is generated only if UseFakeTrigger is set in CTLWORD_static
    //  @see UseFakeTrigger
    //  @see L1a
    //  @see L2a
    //  @see L2r
    void SendPulse(unsigned short mask);

    //Returns the status of the DDL link.
    //  @param RDYRX : true if the SIU-DDL is allowed to send data
    //  @param EOBTR : true if the SIU-DDL has received the End of Block Transfer
    //  @transactionID : 4 bit value giving the transaction ID
    bool GetDDLStatus(bool &RDYRX,bool &EOBTR,short &transactionID);

    //Returns the last 32 word/command written over the DDL link.
    void GetDDLWORD(unsigned int &DDL_WORD);

    void WriteIspflashCtrl(unsigned short &value);
    void ReadIspflashCtrl(unsigned short &value);

    //Write the flash address pointer.
    void SetFlashAddress (unsigned long add);

    // Reads the flash memory.
    unsigned short FlashRead(unsigned long add);

    // Writes the flash memory.
    void FlashWrite(unsigned long add,unsigned short data);

    int FlashChipErase( );
    unsigned int FlashBlockErase( unsigned int ucNumBlocks, unsigned int ucBlock[]);
    int FlashProgram( unsigned int ulOff, unsigned int NumBytes, unsigned short *Array );
    int FlashAutoSelect(int iFunc);
    int FlashDetect();
    const char * FlashErrorStr(int iErrNum);

    //V0 fonctions
    unsigned int ReadV0Data();
    void ReadV0LinkStatus(bool &V0_RXREADY,bool &V0_RXERROR, bool &MOD_PRESENT,bool &V0_RX_LOSS);
    void ReadV0FrameAndError(short &FrameCount,short &ErrorCount);

    //trigger logic
    void setRandomGenerator(int value);
    //  void setL0Delay(unsigned short value);
    void setL1GammaDelay(unsigned short value);
    //void setL1JetDelay(unsigned short value);
    void setTriggerMode(unsigned short opcode);

    void setL0ProcessingTime(unsigned short value);

    void ConfigureFlash(string ConfFile);

    //Monitor mangement
    void GetMonitorStatus(double temp[3],double vccint[3],double vccaux[3]);

    //fonctions for embedded flash programmation and control
    bool EmbeddedFlashDetect();
    bool EmbeddedFlashErase();
    bool EmbeddedConfigureFlash(string ConfFile);
    void EmbeddedUploadFlash(string ConfFile,unsigned int byte_count);

    void WriteI2CTTC(unsigned char add, unsigned char data);
    void ReadTTC(unsigned short add,unsigned short &I2CData,bool &PLL_locked,bool &QPLL_error,bool &QPLL_locked,bool &TTC_ready,bool &I2C_avail);

    void SetL0Delay(int bufferAdd,int delay);

    unsigned short ReadL0Count();
    void ReadL0LatencyDelay(unsigned short &L0latency,unsigned short &L0_L0_delay);
    unsigned short ReadReceptionPointer(int SerialInputNumber);

    void SetGammaThresParameters(short Al, short Bl, short Cl,short Am, short Bm, short Cm,short Ah, short Bh, short Ch);
    
    bool ReadMonErrorCount(unsigned int &trial, unsigned int &error);//for Run2

    void RegionEnable(unsigned int region_mask,unsigned int L0_mask);
    int FindBestPhase(int add);

    TSTU_API();         // constructeur
    virtual ~TSTU_API();// destructeur
};
#endif
