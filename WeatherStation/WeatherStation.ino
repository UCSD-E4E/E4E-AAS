/**
 * E4E Weather Station code
 * Using Sparkfun SEN-08942
 * 5V
 * GND
 * A0 (wind vane)
 * D2 (anemometer)
 */
// Includes

// Defines
#define SPEED_UPDATE_PERIOD	1000
#define DIRECTION_UPDATE_PERIOD 1000
#define UI_UPDATE_PERIOD 1000

// Variables
volatile uint16_t numRevs = 0;
uint8_t windOffset = 0;
uint32_t time, updateSpeed, updateDirection, updateUI;
String direction = "ERROR IN ADC!";
float speed;
// dirOffset is offset in degrees.
uint16_t dirOffset = 0;
float anemometerFixFactor = 0.099;

// Implement all 16 directions anyways, because they can be triggered.  Likelihood is low, but better to have accuracy
const uint16_t DIR_LUT[16] = {79, 138, 198, 240, 324, 397, 428, 567, 623, 744, 787, 847, 906, 940, 949, 967};
// Mapping between DIR_LUT and cardinal directions for offset
const uint8_t DIR_OFF_LUT[16] = {12, 14, 13, 0, 15, 10, 11, 2, 1, 8, 9, 6, 7, 4, 3, 5};
const String CARD_LUT[16] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};

void setup(void){
    Serial.begin(57600);
    Serial.println("E4E Weather Station v1.0b10");
	
	pinMode(2, INPUT);
	digitalWrite(2, HIGH);
	attachInterrupt(0, countAnemometer, FALLING);
	updateSpeed = millis() + SPEED_UPDATE_PERIOD;
	updateDirection = millis() + DIRECTION_UPDATE_PERIOD;
	updateUI = millis() + UI_UPDATE_PERIOD;
}

void countAnemometer(){
	numRevs++;
}

void loop(void){
	time = millis();
	if(time >= updateDirection){
		for(int i = 0; i < 16; i++){
			if(DIR_LUT[i] >= analogRead(A0)){
				direction = CARD_LUT[DIR_OFF_LUT[(i + (int)(dirOffset / 22.5)) % 16]];
				break;
			}
		}
		updateDirection += DIRECTION_UPDATE_PERIOD;
	}
	if(time >= updateSpeed){
		speed = 4793.333/ SPEED_UPDATE_PERIOD * numRevs * anemometerFixFactor;
		numRevs = 0;
		updateSpeed += SPEED_UPDATE_PERIOD;
	}
	if(time >= updateUI){
		Serial.print("Direction: ");
		Serial.print(direction);
		Serial.print(" degrees\tSpeed: ");
		Serial.print(speed);
		Serial.println("MPH");
		updateUI += UI_UPDATE_PERIOD;
	}
}