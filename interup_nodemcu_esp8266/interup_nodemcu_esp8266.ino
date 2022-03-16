#define FlashButtonPIN 0
int x;
void IRAM_ATTR handleInterrupt() {
  x++; 
  if(x==1){Serial.println("Start");}
  if(x==2){Serial.println("END");x=0;}
  Serial.println("Botton Pressed");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  x=0;
  pinMode(FlashButtonPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FlashButtonPIN), handleInterrupt, FALLING);
}

void loop() {
}
