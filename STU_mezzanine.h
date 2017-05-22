//instructions
#define INST_DMA_READ             0x01
#define INST_DMA_WRITE            0x02
#define INST_CONFIG_FPGA          0x07
#define INST_PATCH_ENABLE         0x08
#define INST_GPIO_SELECT          0x09
#define INST_RESET_BOARD          0x0A
#define INST_ISPFLASH_DMA_READ    0x0B
#define INST_ISPFLASH_DMA_WRITE   0x0C

//embedded controlledFLASH configuration
#define INST_FLASH_DETECT         0x0D
#define INST_FLASH_ERASE          0x0E
#define INST_FLASH_PROGRAM        0x0F
#define INST_FLASH_UPLOAD         0x10


//parametre de commandes
#define  FPGA_BASE           0x3000000

#define  add_ETH_IDCODE0            0x00 //Read only
#define  add_ETH_IDCODE1            0x02 //Read only
#define  add_ETH_CTRL_static        0x04 //RW
#define  add_ETH_CTRL_pulse         0x06 //Write only

#define  add_ETH_FIFO_DESER_status  0x08 //Read only
#define  add_ETH_TTC_I2C            0x08 //Write only

#define  add_ETH_FIFO_DESER_DATA    0x0A //Read only
#define  add_ETH_DDL                0x0A 

#define  add_ETH_MEM_DELAY_LSB      0x0C //Read only
#define  add_ETH_DDL_WORD_LSB       0x0C 

#define  add_ETH_MEM_DELAY_MSB      0x0E //Read only
#define  add_ETH_DDL_WORD_MSB       0x0E 

//#define  add_ETH_synchro_STATUS       0x10
#define add_ETH_V0_DATA_LSB         0x10 //Read only

#define  add_ETH_WR_loopback        0x12 //Write only
#define add_ETH_V0_DATA_MSB         0x12 //Read only

#define nb_TRU            28
#define nb_PORT           28
#define nb_MODULE         112

#define add_ETH_V0_link_status      0x14 //Read only
#define add_ETH_V0_frame_error      0x16 //Read only

#define add_ETH_TRIG_random_LSB     0x18 //RW
#define add_ETH_TRIG_random_MSB     0x1A //RW
#define add_ETH_TRIG_L0_delay       0x1C //write only
#define add_ETH_TRIG_L1gamma_delay  0x20 //write only
//#define add_ETH_TRIG_L1jet_delay    0x22 //write only
#define add_ETH_TRIG_opcode         0x24 //write only
#define add_ETH_Monitor             0x26 //write only

#define  add_ETH_region_enable      0x28 //Write only
#define  add_ETH_synchro_STATUS     0x2C //Read only
#define  add_ETH_mem_delay_LSB      0x2E //Read only
#define  add_ETH_mem_delay_MSB      0x30 //Read only
#define  add_ETH_pointer_level      0x32 //Read only
#define  add_ETH_L0_count           0x34 //Read only
#define  add_ETH_L0_offset          0x36 //Read only
#define  add_ETH_L0_phase_select    0x38 //Write only

#define  add_ETH_L0_delay_select    0x3C //Write only

//#define  add_ETH_jet_param          0x44 //Write only
#define  add_ETH_gamma_param        0x46 //Write only
#define  add_ETH_L1A_WINDOW         0x48 //Write only
#define  add_ETH_test_mem           0x4A //Write only
#define  add_ETH_CTRL_static2       0x4C //Write only
#define  add_ETH_error_count        0x3A //read only
#define  add_ETH_mon_error_rate     0x52 //read only  //for Run2

//#define  add_ETH_subregion_scale_param  0x4E //Write only //HIROKI
//#define  add_ETH_jetpatch_mode          0x50 //Write only //HIROKI

//ISPFLASH register affectation (has to be between 0x3010000 and 0x4000000)
#define  ISPFLASH_BASE         0x3010000

#define  add_ETH_FLASH_ADD_LSB      0x04
#define  add_ETH_FLASH_ADD_MSB      0x06
#define  add_ETH_FLASH_ACCESS       0x08
#define  add_ETH_CTRL_REG           0x0A
#define  add_ETH_STATUS_REG         0x0C
#define  add_ETH_SELECT_MAP         0x0E

/*******************************************************************************
  Commands for the various functions
 *******************************************************************************/
#define FLASH_READ_MANUFACTURER       (-2)
#define FLASH_READ_DEVICE_CODE        (-1)

/*******************************************************************************
  Error Conditions and return values.
 *******************************************************************************/
#define FLASH_BLOCK_PROTECTED      (0x01)
#define FLASH_BLOCK_UNPROTECTED    (0x00)
#define FLASH_BLOCK_NOT_ERASED     (0xFF)

#define FLASH_SUCCESS              (-1)
#define FLASH_POLL_FAIL            (-2)
#define FLASH_TOO_MANY_BLOCKS      (-3)
#define FLASH_MPU_TOO_SLOW         (-4)
#define FLASH_BLOCK_INVALID        (-5)
#define FLASH_PROGRAM_FAIL         (-6)
#define FLASH_OFFSET_OUT_OF_RANGE  (-7)
#define FLASH_WRONG_TYPE           (-8)
#define FLASH_ERASE_FAIL           (-14)
#define FLASH_TOGGLE_FAIL          (-15)

#define COUNTS_PER_MICROSECOND  (10000)
#define ANY_ADDR                (0x0000L)
#define MANUFACTURER_ID         0x20        //<expected manufacturer ID
#define DEVICE_ID               0x22FD      //<expected device ID
#define NUM_BLOCKS              135
#define FLASH_SIZE              (0x400000)  //< Total device size in Words

static const unsigned int BlockOffset[] = {
  0x000000,  /* Start offset of block 0  */
  0x001000,  /* Start offset of block 1  */
  0x002000,  /* Start offset of block 2  */
  0x003000,  /* Start offset of block 3  */
  0x004000,  /* Start offset of block 4  */
  0x005000,  /* Start offset of block 5  */
  0x006000,  /* Start offset of block 6  */
  0x007000,  /* Start offset of block 7  */
  0x008000,  /* Start offset of block 8  */
  0x010000,  /* Start offset of block 9  */
  0x018000,  /* Start offset of block 10  */
  0x020000,  /* Start offset of block 11  */
  0x028000,  /* Start offset of block 12  */
  0x030000,  /* Start offset of block 13  */
  0x038000,  /* Start offset of block 14  */
  0x040000,  /* Start offset of block 15  */
  0x048000,  /* Start offset of block 16  */
  0x050000,  /* Start offset of block 17  */
  0x058000,  /* Start offset of block 18  */
  0x060000,  /* Start offset of block 19  */
  0x068000,  /* Start offset of block 20  */
  0x070000,  /* Start offset of block 21  */
  0x078000,  /* Start offset of block 22  */
  0x080000,  /* Start offset of block 23  */
  0x088000,  /* Start offset of block 24  */
  0x090000,  /* Start offset of block 25  */
  0x098000,  /* Start offset of block 26  */
  0x0A0000,  /* Start offset of block 27  */
  0x0A8000,  /* Start offset of block 28  */
  0x0B0000,  /* Start offset of block 29  */
  0x0B8000,  /* Start offset of block 30  */
  0x0C0000,  /* Start offset of block 31  */
  0x0C8000,  /* Start offset of block 32  */
  0x0D0000,  /* Start offset of block 33  */
  0x0D8000,  /* Start offset of block 34  */
  0x0E0000,  /* Start offset of block 35  */
  0x0E8000,  /* Start offset of block 36  */
  0x0F0000,  /* Start offset of block 37  */
  0x0F8000,  /* Start offset of block 38  */
  0x100000,  /* Start offset of block 39  */
  0x108000,  /* Start offset of block 40  */
  0x110000,  /* Start offset of block 41  */
  0x118000,  /* Start offset of block 42  */
  0x120000,  /* Start offset of block 43  */
  0x128000,  /* Start offset of block 44  */
  0x130000,  /* Start offset of block 45  */
  0x138000,  /* Start offset of block 46  */
  0x140000,  /* Start offset of block 47  */
  0x148000,  /* Start offset of block 48  */
  0x150000,  /* Start offset of block 49  */
  0x158000,  /* Start offset of block 50  */
  0x160000,  /* Start offset of block 51  */
  0x168000,  /* Start offset of block 52  */
  0x170000,  /* Start offset of block 53  */
  0x178000,  /* Start offset of block 54  */
  0x180000,  /* Start offset of block 55  */
  0x188000,  /* Start offset of block 56  */
  0x190000,  /* Start offset of block 57  */
  0x198000,  /* Start offset of block 58  */
  0x1A0000,  /* Start offset of block 59  */
  0x1A8000,  /* Start offset of block 60  */
  0x1B0000,  /* Start offset of block 61  */
  0x1B8000,  /* Start offset of block 62  */
  0x1C0000,  /* Start offset of block 63  */
  0x1C8000,  /* Start offset of block 64  */
  0x1D0000,  /* Start offset of block 65  */
  0x1D8000,  /* Start offset of block 66  */
  0x1E0000,  /* Start offset of block 67  */
  0x1E8000,  /* Start offset of block 68  */
  0x1F0000,  /* Start offset of block 69  */
  0x1F8000,  /* Start offset of block 70  */
  0x200000,  /* Start offset of block 71  */
  0x208000,  /* Start offset of block 72  */
  0x210000,  /* Start offset of block 73  */
  0x218000,  /* Start offset of block 74  */
  0x220000,  /* Start offset of block 75  */
  0x228000,  /* Start offset of block 76  */
  0x230000,  /* Start offset of block 77  */
  0x238000,  /* Start offset of block 78  */
  0x240000,  /* Start offset of block 79  */
  0x248000,  /* Start offset of block 80  */
  0x250000,  /* Start offset of block 81  */
  0x258000,  /* Start offset of block 82  */
  0x260000,  /* Start offset of block 83  */
  0x268000,  /* Start offset of block 84  */
  0x270000,  /* Start offset of block 85  */
  0x278000,  /* Start offset of block 86  */
  0x280000,  /* Start offset of block 87  */
  0x288000,  /* Start offset of block 88  */
  0x290000,  /* Start offset of block 89  */
  0x298000,  /* Start offset of block 90  */
  0x2A0000,  /* Start offset of block 91  */
  0x2A8000,  /* Start offset of block 92  */
  0x2B0000,  /* Start offset of block 93  */
  0x2B8000,  /* Start offset of block 94  */
  0x2C0000,  /* Start offset of block 95  */
  0x2C8000,  /* Start offset of block 96  */
  0x2D0000,  /* Start offset of block 97  */
  0x2D8000,  /* Start offset of block 98  */
  0x2E0000,  /* Start offset of block 99  */
  0x2E8000,  /* Start offset of block 100  */
  0x2F0000,  /* Start offset of block 101  */
  0x2F8000,  /* Start offset of block 102  */
  0x300000,  /* Start offset of block 103  */
  0x308000,  /* Start offset of block 104  */
  0x310000,  /* Start offset of block 105  */
  0x318000,  /* Start offset of block 106  */
  0x320000,  /* Start offset of block 107  */
  0x328000,  /* Start offset of block 108  */
  0x330000,  /* Start offset of block 109  */
  0x338000,  /* Start offset of block 110  */
  0x340000,  /* Start offset of block 111  */
  0x348000,  /* Start offset of block 112  */
  0x350000,  /* Start offset of block 113  */
  0x358000,  /* Start offset of block 114  */
  0x360000,  /* Start offset of block 115  */
  0x368000,  /* Start offset of block 116  */
  0x370000,  /* Start offset of block 117  */
  0x378000,  /* Start offset of block 118  */
  0x380000,  /* Start offset of block 119  */
  0x388000,  /* Start offset of block 120  */
  0x390000,  /* Start offset of block 121  */
  0x398000,  /* Start offset of block 122  */
  0x3A0000,  /* Start offset of block 123  */
  0x3A8000,  /* Start offset of block 124  */
  0x3B0000,  /* Start offset of block 125  */
  0x3B8000,  /* Start offset of block 126  */
  0x3C0000,  /* Start offset of block 127  */
  0x3C8000,  /* Start offset of block 128  */
  0x3D0000,  /* Start offset of block 129  */
  0x3D8000,  /* Start offset of block 130  */
  0x3E0000,  /* Start offset of block 131  */
  0x3E8000,  /* Start offset of block 132  */
  0x3F0000,  /* Start offset of block 133  */
  0x3F8000,  /* Start offset of block 134  */
}; /* EndArray BlockOffset[] */
