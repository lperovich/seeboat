// Output sine wave with frequency sweep between 100Hz and 10kHz (32 samples) on analog pin A0 using the DAC and DMAC
// Generously initially donated by MartinL2
// Written for M0.  Tested on Adafruit Feather M0 with ATWINC-1500 WiFi
//
// BDL change to 100Hz to 25000Hz
// BDL change to sweep in frequency, rather than period.  This makes it look better on an FFT
// BDL change to have constant power in FFT bins.  Each frequency operates for the same amount of time yielding constant 
// power in the FFT
// Uploaded to Adafruit forum 17 July 2019

#define const_pwr       // ifndef get constant (single cycle per frequency) chirp.  
                        // ifdef  get constant power per fft bin

//#define serial        // uncomment to get some serial data - leave undef for full performance

volatile uint16_t sintable1[32];
volatile uint32_t freq=10000;
static uint16_t period = 48000000/(freq * 32) -1;

float frac = .1;  //sine wave amplitude will be multipled by this factor

typedef struct                                                                    // DMAC descriptor structure
{
  uint16_t btctrl;
  uint16_t btcnt;
  uint32_t srcaddr;
  uint32_t dstaddr;
  uint32_t descaddr;
} dmacdescriptor ;

volatile dmacdescriptor wrb[12] __attribute__ ((aligned (16)));                   // Write-back DMAC descriptors
volatile dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));    // DMAC channel descriptors
dmacdescriptor descriptor __attribute__ ((aligned (16)));                         // Place holder descriptor

void setup()
{
  // ================================================================================================================
 // #ifdef serial
  Serial.begin(115200);                                           // Comment this line and next for full performance
  //while(!Serial);
 // #endif
  // ================================================================================================================
  
  for (uint16_t i = 0; i < 32; i++)                                // Calculate the sine table with 32 entries
  {
    sintable1[i] = (uint16_t)( frac *  (sinf(2 * PI * (float)i / 32) * 511) + 512);
  }

  analogWriteResolution(10);                                        // Set the DAC's resolution to 10-bits
  analogReadResolution(12);
  analogWrite(A0, 0);                                               // Initialise the DAC

  //===============
  // Initialize DMA
  //===============
  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;                // Set the descriptor section base address
  DMAC->WRBADDR.reg = (uint32_t)wrb;                                // Set the write-back descriptor base adddress
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);      // Enable the DMAC and priority levels

  DMAC->CHID.reg = DMAC_CHID_ID(0);                                 // Select DMAC channel 0
  DMAC->CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                      // Enable suspend channel interrupts on each channel
  DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) |                         // Set DMAC priority to level 0 (lowest)
                      DMAC_CHCTRLB_TRIGSRC(TCC0_DMAC_ID_OVF) |      // Trigger on timer TCC0 overflow
                      DMAC_CHCTRLB_TRIGACT_BEAT;                    // Trigger every beat
  descriptor.descaddr = (uint32_t)&descriptor_section[0];                  // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&sintable1[0] + 32 * sizeof(uint16_t);    // Read the current value in the sine table
  descriptor.dstaddr = (uint32_t)&DAC->DATA.reg;                           // Copy it into the DAC data register
  descriptor.btcnt = 32;                                                   // This takes the number of sine table entries = 256 beats
  descriptor.btctrl = DMAC_BTCTRL_BLOCKACT_SUSPEND |                // Suspend DMAC channel at end of block transfer
                      DMAC_BTCTRL_BEATSIZE_HWORD |                  // Set the beat size to 16-bits (Half Word)
                      DMAC_BTCTRL_SRCINC |                          // Increment the source address every beat
                      DMAC_BTCTRL_VALID;                            // Flag the descriptor as valid
  memcpy((void*)&descriptor_section[0], &descriptor, sizeof(dmacdescriptor));  // Copy to the channel 0 descriptor  
 
  NVIC_SetPriority(DMAC_IRQn, 0);           // Set the Nested Vector Interrupt Controller (NVIC) priority for the DMAC to 0 (highest) 
  NVIC_EnableIRQ(DMAC_IRQn);                // Connect the DMAC to the Nested Vector Interrupt Controller (NVIC)

  //=============================
  // Initialize clocks and timer
  //=============================
  
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
                     GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                      GCLK_GENCTRL_GENEN |         // Enable GCLK4
                      GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                      GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                      GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                      GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NFRQ;          // Setup TCC0 in Normal Frequency (NFRQ) mode
  while (TCC0->SYNCBUSY.bit.WAVE);                 // Wait for synchronization
  
//  freq = basefreq;                                 // set freq to minimum
                 
  #ifdef serial
  Serial.print("Start period is: ");  Serial.println(48000000/(freq *32) - 1);
  #endif
  TCC0->PER.reg = 48000000/(freq *32) - 1;
  
  while(TCC0->SYNCBUSY.bit.PER);                   // Wait for synchronization
 
  TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1;      // Set the TCC0 prescaler to 1 giving 48MHz (20.83ns) timer tick
  TCC0->CTRLA.bit.ENABLE = 1;                      // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE);               // Wait for synchronization
 
  DMAC->CHID.reg = DMAC_CHID_ID(0);                // Select DMAC channel 
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;        // Enable DMAC channel 
}
///////////////////////////////////////
void loop() {
  
    int sensorValue = analogRead(A1);
    Serial.print(sensorValue);
    Serial.print(",");
    Serial.println();
    delay(1000);

  
  
  }                                      // We don't do anything in the loop
///////////////////////////////////
void DMAC_Handler()
{

     
  
  DMAC->CHID.reg = DMAC_CHID_ID(DMAC->INTPEND.bit.ID);    // Find the DMAC channel generating the interrupt
  descriptor_section[0].btctrl &= ~DMAC_BTCTRL_VALID;     // Disable the descriptor   
  TCC0->PERB.reg = period;                                // 2kHz sine wave, 128 samples: 48MHz / (2000 * 128) - 1
  while(TCC0->SYNCBUSY.bit.PERB);                         // Wait for synchronization
  
  #ifdef serial
  Serial.print("idx  :"); Serial.println(idx);
  Serial.print("count:"); Serial.println(count);
  #endif

  
  descriptor_section[0].btctrl |= DMAC_BTCTRL_VALID;      // Enable the descriptor                       
  DMAC->CHCTRLB.reg |= DMAC_CHCTRLB_CMD_RESUME;           // Resume the DMAC channel
  DMAC->CHINTFLAG.bit.SUSP = 1;                           // Clear the DMAC channel suspend (SUSP) interrupt flag  
}
