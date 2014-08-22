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
String direction = "ERROR IN ADC!\n";
float speed;
uint16_t dirOffset = 0;
float anemometerFixFactor = 1;

const uint8_t DIR_LUT[16] = {14, 26, 39, 49, 67, 84, 92, 125, 140, 171, 183, 199, 216, 226, 229, 234};
const uint8_t DIR_OFF_LUT[16] = {12, 14, 13, 0, 15, 10, 11, 2, 1, 8, 9, 6, 7, 4, 3, 5};
const char* CARD_LUT[16] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};

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
				direction = CARD_LUT[DIR_OFF_LUT[(i + dirOffset) % 16]];
				break;
			}
		}
		updateDirection += DIRECTION_UPDATE_PERIOD;
	}
	if(time >= updateSpeed){
		speed = 4793.333/ SPEED_UPDATE_PERIOD * numRevs * anemometerFixFactor;
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