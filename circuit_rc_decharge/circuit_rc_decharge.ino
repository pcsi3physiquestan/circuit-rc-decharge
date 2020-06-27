// Attention, ce programme fonctionne avec un programme Python ou Tinkercad (plus lent).
// Programme pour observer la décharge d'un condensateur à travers une résistance.
// Condition sur les composantes : l'échantillonnage ne peut se faire à moins de 0.4ms. 
// Il est donc conseillé de ne pas chercher à mesurer un temps caractéristique inférieur à 10ms environ.

const int INPUT_PIN = 2;  // PIN simulant l'entrée du circuit
const int LED_PIN = 4;  // Fac. Numéro de la PIN pour la LED signalant que la communication est établie
const int MESURE_PIN = A0;  // PIN de mesure analogique

const long DUREE_MAX = 1000000;  // En microsecondes, duree max pour éviter des durées d'acquisition trop grand.
const long BAUD_RATE = 115200;

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
  Serial.println("Entrer la durée d'observation en millisecondes (Max 1000): ");
}

void loop()
{
  if(Serial.available()){
    digitalWrite(LED_PIN, HIGH);
    delay(100);  //Délai au cas où
    int type = Serial.peek();
    if(type == 80){  // Communication avec Python
      isPython = true;
      Serial.read();
      receive_int(duree);  // On récupère la durée d'acquisition
      if(duree > 0){
        long duree_2 = duree_en_micro(duree);
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
      }
    } else {  // Communication avec IDE Arduino/TinkerCad
      duree = Serial.parseInt();  // On récupère le nombre d'échantillons
      while(Serial.available()){Serial.read();}  // On vide le buffer
      if(duree > 0){
        long duree_2 = duree_en_micro(duree);
        digitalWrite(INPUT_PIN, LOW);  // On passe en régime libre
        time0 = micros();  // Mesure du temps initial
        timem = time0;
        timet = timem;
        while((timem - time0) < duree_2){  // On fait des mesures tant qu'on a pas atteint la durée d'acquisition
          timet = micros() - timem;
          measure = analogRead(MESURE_PIN);  // Mesure de la tension
          Serial.print(timet);  // On envoie le temps
          Serial.print(',');
          Serial.println(measure);  // On envoie la mesure de tension
          timem = timet + timem;
        }
      }
    }
    duree = 0;
    digitalWrite(INPUT_PIN, HIGH);  // On recharge le condensateur
    delay(1000);  // Nécessaire pour être sûr que le condensateur s'est rechargé.
    digitalWrite(LED_PIN, LOW);
    delay(1000);  // Nécessaire pour être sûr que le condensateur s'est rechargé.
  }
}

long duree_en_micro(int duree){
    long duree_2 = 1000 * (long) duree;  // On passe la durée en microsecondes pour plus de précision
    duree_2 = min(duree_2, DUREE_MAX);
    delay(10);
    return duree_2; 
}

void receive_int(int &ampl){ // Programme qui permet de recevoir un nombre entier depuis le programme Python
  char *c;        
  c = (char *)&ampl;
  while (Serial.available()==0) {};
  *(c+1)=Serial.read();
  while (Serial.available()==0) {};
  *c=Serial.read();
}
