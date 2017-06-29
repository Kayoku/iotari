// test : send 2 differents structs the same way and parse them in python using pyserial
// the structs are well defined (ie we know each field on both sides, but the reveiver shouldn't care

struct test1 {
  byte id;
  int a;
  long b;
  int c;
};

struct test2 {
  byte id;
  int a;
  int b;
  int c;
};


test1 t1 = {1, 2, 323, 4};
test2 t2 = {2, 10, 11, 12};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // I also wanted to confirm the value I read.
  Serial.write((const uint8_t *)&t1, sizeof(t1));

  delay(5000);
  Serial.write((const uint8_t *)&t2, sizeof(t2));

}

void loop() {
  // put your main code here, to run repeatedly:
  
}
