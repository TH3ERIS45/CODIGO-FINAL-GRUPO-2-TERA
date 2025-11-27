// Generates a ~1 MHz square wave on Arduino UNO pin 9 (OC1A)
// Connect this pin to P8253 CLK0 (and common ground).
void setup() {
  // No Serial needed
  pinMode(9, OUTPUT); // OC1A
  // Stop Timer1
  TCCR1A = 0;
  TCCR1B = 0;

  // CTC mode, toggle OC1A on compare match:
  // WGM12 = 1 -> CTC with OCR1A top
  // COM1A0 = 1 -> Toggle OC1A on compare match
  TCCR1A = (1 << COM1A0);
  TCCR1B = (1 << WGM12);

  // Prescaler = 1 (CS12:0 = 001)
  TCCR1B |= (1 << CS10);

  // Set OCR1A for 1 MHz: OCR1A = 7
  OCR1A = 7;
}

void loop() {
}
