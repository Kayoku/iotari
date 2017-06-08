#define startFrame 0x3A
#define endFrame   0x3F
#define lineStart  0x0A
#define lineEnd    0x0D
#define lineLength 30

struct values_we_care_about_s {
  long HC;
  long HP;
  int IINST;
  boolean IS_HP;
  unsigned long timestamp;
};

struct values_we_care_about_s v;

char sep = ' ';
int  __charIndex = 0;
char __simulatedString[] = ":\nADCO 030622716812 =\r\nOPTARIF HC.. <\r\nISOUSC 30 9\r\nHCHC 035318160 !\r\nHCHP 039061379 9\r\nPTEC HP..  \r\nIINST 001 X\r\nIMAX 029 J\r\nPAPP 00180 *\r\nHHPHC C .\r\nMOTDETAT 000000 B\r?";
char getCharSimulated(){
  char c = __simulatedString[__charIndex];
  __charIndex = (__charIndex + 1) % 170;
  return c;
}

void displayV() {
  Serial.print("heure creuse : ");
  Serial.println(v.HC);
  Serial.print("heure pleine : ");
  Serial.println(v.HP);
  Serial.print("intensite : ");
  Serial.println(v.IINST);
  Serial.print("heure pleine ? ");
  Serial.println(v.IS_HP);
  Serial.print("heure : ");
  Serial.println(v.timestamp);
}

void initStruct() {
  v.timestamp = millis();
  v.HC = 0;
  v.HP = 0;
  v.IINST = 0;
  v.IS_HP = false;
}

void lireTrame() {
  initStruct();
  
  /* Recherche de début de trame */
  char currentChar = 0;

  while (currentChar != startFrame)
    currentChar = getCharSimulated();
  
  /* Boucle principale */
  int index = 0;
  char line[lineLength] = {0};

  while (currentChar != endFrame)
  {
    /* Lecture d'un caractère */
    currentChar = getCharSimulated();

    /* Soit c'est le début d'une ligne */
    if (currentChar == lineStart)
    {
      memset(line, 0, sizeof(line));
      index = 0;
    }
    /* Soit c'est la fin d'une ligne */
    else if (currentChar == lineEnd)
    {
      char* name;
      char* value;

      name = strtok(line, &sep);
      value = strtok(NULL, &sep);

      if(strcmp(name, "PTEC") == 0)
      {
        if(strcmp(value, "HP..") == 0)
          v.IS_HP = true;
      }
      else if(strcmp(name, "HCHC") == 0)
        v.HC = atol(value);
      else if(strcmp(name, "HCHP") == 0)
        v.HP = atol(value);
      else if(strcmp(name, "IINST") == 0)
        v.IINST = atol(value);
    }
    /* Soit c'est un caractère à enregistrer */
    else
    {
      if(index > lineLength)
      {
        Serial.println("ERREUR TAILLE LIGNE");
        continue;
      }
      line[index] = currentChar;
      index++;
    }
  }
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  initStruct();
  lireTrame();
  displayV();
  delay(10000);
}
