void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(2, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(3, LOW);
  digitalWrite(3, HIGH);
  delayMicroseconds(60);

}
