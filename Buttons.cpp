#include "Buttons.h"

SharedData *Buttons::shrd;


Buttons::Buttons(void){}
Buttons::~Buttons(void){}

void Buttons::setSharedData(SharedData *data) {
	shrd = data;
}

bool flangeActualyPressed = false;
int flangePressedTime = 0;
void Buttons::flangeLoop() {
	int flangeButtonState = digitalRead(BUTTON_FLANGE_PIN);

	if(flangeButtonState == LOW) {
		if(!flangeActualyPressed) {
			flangePressedTime = millis();
			flangeActualyPressed = true;
		}

		if(millis() - flangePressedTime > BUTTON_FLANGE_PRESSED_TIME) {
			shrd->switchFlange(true);
		}
	}

	if(flangeButtonState == HIGH) {
		flangeActualyPressed = false;
		flangePressedTime = 0;
	}
}

void Buttons::loop() {
	flangeLoop();
	
	delay(10);
}

void Buttons::init() {
    
}