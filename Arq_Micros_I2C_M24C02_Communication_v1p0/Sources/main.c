#include "derivative.h" /* include peripheral declarations */

#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))
//--------------------------------------------------------------
//Declare Variables
int i = 0;
int count = 0;

//--------------------------------------------------------------
//Declare Functions Prototypes
void cfgPorts(void);
void cfgI2C_Master_Second_Form(void);
void I2C1_IRQHandler(void);



int main(void)
{	cfgPorts();
	cfgI2C_Master_Second_Form();
	
	for(;;)
	{
		//I2C1_D = ; 	-> Writing starts transmission
		//I2C1_S 		-> Clear interruption flag
	}
	return 0;
}

void cfgPorts(void)
{
//------------------------------------------------------
//Activate clocks
	SIM_SCGC4 = SIM_SCGC4_I2C1_MASK;
	SIM_SCGC5 = SIM_SCGC5_PORTE_MASK;	
//------------------------------------------------------
// Set pins of PORTE as SDA & SCL 
	PORTE_PCR0= PORT_PCR_MUX(6);
	PORTE_PCR1= PORT_PCR_MUX(6);	
//------------------------------------------------------
//Initialize PORTE
	GPIOE_PDOR = 0x00;	
//------------------------------------------------------
//Initialize PORTE	
	GPIOE_PDDR = 0xFF;
}

void cfgI2C_Master_Second_Form(void)
{	//Activate interruption
	NVIC_ICPR|= 1 << (9%32);
	NVIC_ISER|= 1 << (9%32);
	//Slave address
	I2C1_A1 = 0b10100000; //Last zero is zero by default
	//Frequency divider
	I2C1_F  = 0b10000000;  //0x0F in 5-0 bits -> 0b001111
	//Control register 1
	I2C1_C1 = 0b11000000; //Only IICEN = 1 and IICIE = 1
	//Configure RAM variables
	
	//Control register 1
	I2C1_C1 |= 0b00010000; //TX = 1
	//Control register 1
	I2C1_C1 |= 0b00100000;  //MST= 1
	//Data register
	I2C1_D  =  0b10100000;

}

void I2C1_IRQHandler(void)
{	if((I2C1_C1 & 0b00010000) == 16) //TX MODE.!
	{ 	count ++;
		//Data direction
		//TCF: is cleared by reading the I2C data
		//register in receive mode or by writing to the I2C data register in transmit mode.
		switch(count)
		{	case 1:
				I2C1_D   = 0b00001010;
				break;
			case 2:
				I2C1_D   = 0b00000111;
				break;
		}
		//IICIF: this bit must be cleared by software by writing a 1 to it, such as
		//in the interrupt routine.	
		//I2C1_S  = (1<<4);
		I2C1_S  = (1<<1);
		if (count==3)
		{	I2C1_C1 &= 0b11011111;  //MST= 0
			count = 0;
		}
	}
}
