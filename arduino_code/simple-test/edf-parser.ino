#include <SoftwareSerial.h>
SoftwareSerial* cptSerial;

#define startFrame 0x02
#define endFrame   0x03
#define lineStart  0x0A
#define lineEnd    0x0D
#define fieldSep   0x20
#define lineLength 30


inline char getCharSerial(){
  // on "zappe" le 8ème bit, car d'après la doc EDF 
  // la tramission se fait en 7 bits
  return cptSerial->read() & 0x7F;
}

int  __charIndex = 0;
char __simulatedString[] = "ADCO 030622716812 =\nOPTARIF HC.. <\nISOUSC 30 9\nHCHC 035318160 !\nHCHP 039061379 9\nPTEC HP..  \nIINST 001 X\nIMAX 029 J\nPAPP 00180 *\nHHPHC C .\nMOTDETAT 000000 B\n";
char getCharSimulated(){
  char c = __simulatedString[__charIndex];
  __charIndex = (__charIndex + 1) % 170;
  return c;
}

inline char getChar(){
  return getCharSerial();
  //return getCharSimulated();
}

void lireTrame(){
  char charIn = 0;
  // Boucle d'attente du caractère de début de trame
  while (charIn != startFrame)
  {
    // on bloque tant que l'on a pas le début de la trame
    charIn = getChar();
  }

  byte index = 0;
  char line[lineLength];
  while (charIn != endFrame)
  {    
    // on lit jusqu'à la fin de la trame
    
    charIn = getChar();
    if (charIn == lineStart){
      char line[lineLength];
      index = 0;
    }
    else if (charIn == lineEnd){
      // fin de ligne, on parse le contenu
      
      if(strcmp(line, "OPTARIF") == 0){
        // parse tarif 
        byte tarifHC = isTarifHC(line);
      }
      if(strcmp(line, "HCHC") == 0){
        // parse valeur HC
        double HC = parse(line);
      }
      if(strcmp(line, "HCHP") == 0){
        // parse valeur HP
        double HP = parse(line);
      }
      if(strcmp(line, "IINST") == 0){
        // parse intensite instentanee
        int intInst = (int)(parse(line));
      }
    }
    else{
      // si c'est un caractère normal
      if(index > lineLength){
        //oulala
        break;
      }
      line[index] = charIn;
      index++;
    }
  }
}


inline byte isTarifHC(char line[]){
  // exemple :  OPTARIF HC.. <
  if(strstr(line, "HC") != NULL){
    return 1;
  }
  if(strstr(line, "HP") != NULL){
    return 0;
  }
  return 2; //error
}

inline double parse(char* const s) {
  return atol(strchr(s, fieldSep));
}

void setup(){
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
