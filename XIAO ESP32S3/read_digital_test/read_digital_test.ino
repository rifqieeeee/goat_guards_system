#define pin_unihiker 1

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_unihiker, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  int data = digitalRead(pin_unihiker);
  Serial.println(data);

}
