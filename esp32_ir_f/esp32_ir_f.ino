const uint8_t ir_pins[16] = {23,19,18,5,17,16,4,34,35,32,33,25,26,27,14,12};

uint8_t ir_readings[16];

void readIRs() {
  for (uint8_t ir = 0; ir < 16; ir++) ir_readings[ir] = digitalRead(ir_pins[ir]);
}

void setup() {
  Serial1.begin(115200, SERIAL_8N1, 21, 22); //RX = 21, TX = 22

  for (uint8_t pin = 0; pin < 16; pin++) pinMode(ir_pins[pin], INPUT);
}

void loop() {
  readIRs();
  Serial1.write(ir_readings, sizeof(ir_readings));
  delay(10);
}
