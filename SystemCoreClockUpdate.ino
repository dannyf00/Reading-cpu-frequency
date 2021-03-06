//SystemCoreClockUpdate.ino
#include <avr/boot.h>				//we read fuse bits

//user configuration - user specified
#define F_XTAL		16000000ul		//external oscillator / crystal
#define F_LPOSC		32768ul			//external low power oscillator

//global defines
#define F_RC128K	128000ul		//low freuqency internal RC oscillator, 128Khz
#define F_RC8M		8000000ul		//calibrated internal RC oscillator, 8Mhz

//global variables
extern uint32_t SystemCoreClock=F_RC8M;	//default clock is 8M internal oscillator
#define F_CPU		(SystemCoreClock)	//cpu speed

//read back systemcoreclock for ATMega328
uint32_t SystemCoreClockUpdate(void) {
	uint32_t tmp;
	char lfuse, hfuse, efuse, lbits;	//low and high fuse, extended fuse and lock bits

	//read the fuses - only the low fuse is used
	cli(); 
	lfuse = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
	hfuse = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS); 
	efuse = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS); 
	lbits = boot_lock_fuse_bits_get(GET_LOCK_BITS); 
	sei();
	
	//calculate SystemCoreClock
	switch (lfuse & 0x0f) {
		case 0b0000:					//external clock
			tmp = F_XTAL; break;
		//case 0b0001:					//reserved - do nothing
		case 0b0010:					//calibrated internal RC oscillator
			tmp = F_RC8M; break;
		case 0b0011:					//internal 128Khz oscillator
			tmp = F_RC128K; break;
		case 0b0100:					//external low power oscillator (32768Khz)
		case 0b0101:					
			tmp = F_LPOSC; break;
		case 0b0110:					//external full swing oscillator, 0.4-20Mhz
		case 0b0111:					//0.4-20Mhz
			tmp = F_XTAL; break;
		case 0b1000:					//low power crystal oscillator, 0.4-0.9Mhz
		case 0b1001:					//0.4-0.9Mhz
		case 0b1010:					//0.9-3.0Mhz
		case 0b1011:					//0.9-3.0Mhz
		case 0b1100:					//3.0-8.0Mhz
		case 0b1101:					//3.0-8.0Mhz
		case 0b1110:					//8.0-16.Mhz
		case 0b1111:					//8.0-16.Mhz
			tmp = F_XTAL; break;
		}
	
	tmp = (lfuse & 0x80)?tmp:(tmp / 8);	//if MSB=0 (programmed), output frequency divided by 8
	return SystemCoreClock=tmp;			//return the systemcoreclock
}

int led = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
    digitalWrite(led, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(100);               // wait for a second
    digitalWrite(led, LOW);   // turn the LED off by making the voltage LOW
    delay(100);               // wait for a second

    SystemCoreClockUpdate();
    Serial.print("SystemCoreClock = "); Serial.print(F_CPU); Serial.print("\r\n");
}
