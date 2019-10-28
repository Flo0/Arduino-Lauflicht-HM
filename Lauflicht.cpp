#include <Arduino.h>

#define LIGHT_UNITS 6

#define TICK_FREQUENCY_HZ 1000
#define OUTPUT_FLANKS_HZ 100000
#define MICRO_DECIMAL 1000000
#define BAUD_RATE 9600L

#define DDR (*(volatile uint8_t *)(DDRB))
#define PORT (*(volatile uint8_t *)(PORTB))
#define SERIAL_POS 0
#define SHIFT_CLOCK_POS 1
#define STORAGE_CLOCK_POS 2

enum Direction {
	UP, DOWN
};

unsigned int tick_delay_micros;
unsigned int output_delay_micros;
Direction currentDirection = UP;
uint8_t currentPos = 0;
uint8_t dataOut = 0;

int main(){

	init();
	Serial.begin(BAUD_RATE);
	tick_delay_micros = MICRO_DECIMAL / TICK_FREQUENCY_HZ;
	output_delay_micros = MICRO_DECIMAL / OUTPUT_FLANKS_HZ;

	// Outputs real tick frequency
	double realFrequency = (double) 1.0 / (tick_delay_micros * ((double) 1.0 / 1000000));
	// 'Rounds' the frequency to one decimal
	realFrequency = ((int)(realFrequency * 10)) / (double) 10.0;
	Serial.println("Starting tick clock with ");
	Serial.print(realFrequency);
	Serial.print(" Hz.");

	// Outputs real output signal frequency
	realFrequency = (double) 1.0 / (output_delay_micros * ((double) 1.0 / 1000000));
	// 'Rounds' the frequency to one decimal
	realFrequency = ((int)(realFrequency * 10)) / (double) 10.0;
	Serial.println("Starting output clock with ");
	Serial.print(realFrequency);
	Serial.print(" Hz.");

	Serial.end();

	// Setting Serial Data Out, Shift clock, Storage clock as output
	DDR |= (1 << SERIAL_POS) | (1 << SHIFT_CLOCK_POS) | (1 << STORAGE_CLOCK_POS);
	// Resetting port
	PORT &= ~((1 << SERIAL_POS) | (1 << SHIFT_CLOCK_POS) | (1 << STORAGE_CLOCK_POS));
	// Start tick loop
	void loopRun();

}

// Loop with delta corrected delay
void loopRun(){
	while(1){
		unsigned long start = micros();
		void tick();
		unsigned long delta = micros() - start;
		delayMicroseconds(tick_delay_micros - delta);
	}
}

void tick(){
	// Increments light opsition
	void next();
	// Writes 8 bit signal to external register
	void serialOut(uint8_t dataOut);
}

void next(){
	switch(currentPos){
	case 0:
		// Lower end direction switch
		void lowerEndSwitch();
		break;
	case LIGHT_UNITS - 1:
		// Upper end direction switch
		void upperEndSwitch();
		break;
	default:
		// Increments or decrements position depending on direction == UP/DOWN
		currentPos += (currentDirection == UP ? 1 : -1);
		break;
	}
	// Sets the current light position + one bit above/below depending on direction == UP/DOWN
	dataOut = (1 << currentPos) | (1 << currentPos + (currentDirection == UP ? 1 : -1));
}

void lowerEndSwitch(){
	if(currentDirection == DOWN){
		currentDirection = UP;
	}else{
		currentPos++;
	}
}

void upperEndSwitch(){
	if(currentDirection == UP){
		currentDirection = DOWN;
	}else{
		currentPos--;
	}
}

// Writes serialOut to output
void serialOut(uint8_t output){
	// Iterate over all bits in 'output'
	for(int n = 0; n < 8; n++){
		// Resets output port
		PORT &= ~(1 << SERIAL_POS);
		// Check if n'th bit is set in output
		// And-links either 1 or 0 to SERIAL_POS in PORT
		PORT |= (output & (1 >> n)) << SERIAL_POS;

		void shiftRegisterClock();
	}

	void storageRegisterClock();
}

// Outputs a HIGH signal to the shift clock for output_delay_micros
void shiftRegisterClock(){
	PORT |= (1 << SHIFT_CLOCK_POS);
	delayMicroseconds(output_delay_micros);
	PORT &= ~(1 << SHIFT_CLOCK_POS);
}

// Outputs a HIGH signal to the storage clock for output_delay_micros
void storageRegisterClock(){
	PORT |= (1 << STORAGE_CLOCK_POS);
	delayMicroseconds(output_delay_micros);
	PORT &= ~(1 << STORAGE_CLOCK_POS);
}
