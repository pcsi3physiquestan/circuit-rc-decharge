// Attention, ce programme ne fonctionne qu'avec un programme Python.
// Programme pour observer la décharge d'un condensateur à travers une résistance.
// Condition sur les composantes : l'échantillonnage ne peut se faire à moins de 0.4ms. 
// Il est donc conseillé de ne pas chercher à mesurer un temps caractéristique inférieur à 10ms environ.

const int INPUT_PIN = 2;  // PIN simulant l'entrée du circuit
const int LED_PIN = 4;  // Fac. Numéro de la PIN pour la LED signalant que la communication est établie
const int MESURE_PIN = A0;  // PIN de mesure analogique

const long DUREE_MAX = 1000000;  // En microsecondes, duree max pour éviter des durées d'acquisition trop grand.
const long BAUD_RATE = 115200;
const unsigned short T_ECH = 500;

bool isPython = false;  // Pour adapter le programme à l'IDE Arduino et à un programme Python.
unsigned long time0;
unsigned long timem;
unsigned short timet;
int duree;  // Sera la durée d'acquisition
int nmeas;  //Fac. Pour la période d'échantillonnage si besoin.
short measure;  // Sera la valeur mesurée (entre 0 pour 0V et 1023 pour 5V)

void setup()
{
  // Initialisation de la liaison série
  Serial.begin(BAUD_RATE);
  // Initialisation des broches
  pinMode(INPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(INPUT_PIN, HIGH); // On charge le condensateur
  digitalWrite(LED_PIN, LOW);  // Fac. s'il n'y a pas de LED
}

void loop()
{
  if(Serial.available()){
    digitalWrite(LED_PIN, HIGH);
    delay(100);  //Délai au cas où
    // Les lignes suivantes permettent de recevoir la demi-période du créneau et le nombre d'échantillon.
    // Il n'est pas nécessaire de comprendre les détails. La condition if sert à adapter la réception à l'IDE et à Python
    int type = Serial.peek();
    if(type == 80){
      isPython = true;
      Serial.read();
      receive_int(duree);  // On récupère la demie-période
    } else {
      while(Serial.available()){Serial.read();}
      Serial.println("Ce programme ne fonctionne pour l'instant avec l'IDE");
      duree = 10;
    }
    
    if(duree > 0){
      long duree_2 = 1000 * (long) duree;  // On passe la durée en microsecondes pour plus de précision
      duree_2 = min(duree_2, DUREE_MAX);
      delay(10);
      digitalWrite(INPUT_PIN, LOW);  // On passe en régime libre
      time0 = micros();  // Mesure du temps initial
      timem = time0;
      timet = timem;
      while((timem - time0) < duree_2){  // On fait des mesures tant qu'on a pas atteint la durée d'acquisition
        timet = micros() - timem;
        measure = analogRead(MESURE_PIN);  // Mesure de la tension
        Serial.write((uint8_t*)&timet,2);  // On envoie le temps
        Serial.write((uint8_t*)&measure,2);  // On envoie la mesure de tension
        timem = timet + timem;
      }
    } else {
    }    
    duree = 0;
    digitalWrite(INPUT_PIN, HIGH);  // On recharge le condensateur
    delay(1000);  // Nécessaire pour être sûr que le condensateur s'est rechargé.
    digitalWrite(LED_PIN, LOW);
    delay(1000);  // Nécessaire pour être sûr que le condensateur s'est rechargé.
  }
}


void receive_int(int &ampl){ // Programme qui permet de recevoir un nombre entier depuis le programme Python
  char *c;        
  c = (char *)&ampl;
  while (Serial.available()==0) {};
  *(c+1)=Serial.read();
  while (Serial.available()==0) {};
  *c=Serial.read();
}
