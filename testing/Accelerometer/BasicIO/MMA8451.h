
#define MMA8451_DEFAULT_ADDRESS	0x3A    


#define MMA8451_REG_OUT_X_MSB     0x01

#define MMA8451_REG_OUT_X_LSB     0x02

#define MMA8451_REG_OUT_Y_MSB     0x03

#define MMA8451_REG_OUT_Y_LSB     0x04

#define MMA8451_REG_OUT_Z_MSB     0x05

#define MMA8451_REG_OUT_Z_LSB     0x06

#define MMA8451_REG_SYSMOD        0x0B

#define MMA8451_REG_WHOAMI        0x0D

#define MMA8451_REG_XYZ_DATA_CFG  0x0E

#define MMA8451_REG_PL_STATUS     0x10

#define MMA8451_REG_PL_CFG        0x11

#define MMA8451_REG_CTRL_REG1     0x2A

#define MMA8451_REG_CTRL_REG2     0x2B

#define MMA8451_REG_CTRL_REG4     0x2D

#define MMA8451_REG_CTRL_REG5     0x2E




#define MMA8451_PL_PUF            0

#define MMA8451_PL_PUB            1

#define MMA8451_PL_PDF            2

#define MMA8451_PL_PDB            3

#define MMA8451_PL_LRF            4

#define MMA8451_PL_LRB            5

#define MMA8451_PL_LLF            6

#define MMA8451_PL_LLB            7



typedef enum
{
  
MMA8451_RANGE_8_G           = 0b10,   // +/- 8g
  
MMA8451_RANGE_4_G           = 0b01,   // +/- 4g
  
MMA8451_RANGE_2_G           = 0b00    // +/- 2g (default value)
} mma8451_range_t;


/* Used with register 0x2A (MMA8451_REG_CTRL_REG1) to set bandwidth */


typedef enum
{
  
MMA8451_DATARATE_800_HZ     = 0b000, //  800Hz
  
MMA8451_DATARATE_400_HZ     = 0b001, //  400Hz
  
MMA8451_DATARATE_200_HZ     = 0b010, //  200Hz
  
MMA8451_DATARATE_100_HZ     = 0b011, //  100Hz
  
MMA8451_DATARATE_50_HZ      = 0b100, //   50Hz
  
MMA8451_DATARATE_12_5_HZ    = 0b101, // 12.5Hz
  
MMA8451_DATARATE_6_25HZ     = 0b110, // 6.25Hz
  
MMA8451_DATARATE_1_56_HZ    = 0b111, // 1.56Hz

  
MMA8451_DATARATE_MASK       = 0b111
} mma8451_dataRate_t;

