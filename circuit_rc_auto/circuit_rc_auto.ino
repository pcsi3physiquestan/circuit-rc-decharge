// Ce programme est réaliser pour fonctionner avec l'IDE Arduino/Tinkercad ou un programme Python.
// Programme pour mesurer automatiquement le temps caractéristique d'un circuit RC plusieurs fois de suite.
// Condition sur les composantes : l'échantillonnage ne peut se faire à moins de 0.4ms. 
// Il est donc conseillé de ne pas chercher à mesurer un temps caractéristique inférieur à 10ms environ.

const int INPUT_PIN = 2;  // PIN simulant la tension d'entrée créneau
const int LED_PIN = 4;  // FAC. PIN pour la LED signalant que la communication est établie
const int MESURE_PIN = A0;  // PIN de mesure de uC

const long T_MAX = 1000;  // Demi-période maximale du créneau (1s)
const short N_MAX = 5000;  // Nombre maximal d'échantillon pour la mesure de tau

unsigned long time0;  // Sera le t0 correspondant au début du régime libre
unsigned long timem;  // Sera la mesure de temps
int n_ech;  // Sera le nombre d'échantillon à prendre
int duree;  // Sera la demi-période du créneau
float measure;  // Sera la mesure de tension
bool isPython = false;  // Pour adapter le programme à l'IDE Arduino et à un programme Python.

void setup()
{
  // On initialise la communication série
  Serial.begin(115200);
  // On initialise les broches
  pinMode(INPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(INPUT_PIN, HIGH); // On charge le condensateur
  digitalWrite(LED_PIN, LOW);
  Serial.println("Entrez le nombre d'échantillons de mesure");
}

void loop()
{
  if(Serial.available()){ // L'arduino a reçu des données par le port série
    digitalWrite(LED_PIN, HIGH);
    delay(100);  //Délai au cas où
    // Les lignes suivantes permettent de recevoir la demi-période du créneau et le nombre d'échantillon.
    // Il n'est pas nécessaire de comprendre les détails. La condition if sert à adapter la réception à l'IDE et à Python
    int type = Serial.peek();
    if(type == 80){  // Communication avec Python
      isPython = true;
      Serial.read();
      receive_int(n_ech);  // On récupère le nombre d'échantillons
      receive_int(duree);  // On récupère la demie-période
    } else {  // Communication avec IDE Arduino/TinkerCad
      n_ech = Serial.parseInt();  // On récupère le nombre d'échantillons
      while(Serial.available()){Serial.read();}  // On vide le buffer
      Serial.println("Entrez la demie-période du créneau en ms");
      while(Serial.available() == 0){};  // On attent une saisie dans le moniteur série de l'IDE
      duree = Serial.parseInt();  // On récupère la demie-période
      while(Serial.available()){Serial.read();}  // On vide le buffer
    }
    
    n_ech = min(n_ech, N_MAX);
    duree = min(duree, T_MAX);
    if(n_ech > 0 and duree >0){
      long duree_2 = 1000 * (long) duree;  // On passe la durée en microsecondes pour plus de précisions
      for(int n=0; n<n_ech; n++){  // On va réaliser les n_ech échantillons
        measure = analogRead(MESURE_PIN);  // Mesure de la tension du condensateur chargé
        measure = (float) analogRead(MESURE_PIN) * 37 / 100; // On calcule les 37%
        digitalWrite(INPUT_PIN, LOW);  // On passe en régime libre.
        time0 = micros();  // Mesure de t0
        while(((float)analogRead(MESURE_PIN) - measure > 0) && timem < duree_2){ // On attend d'atteindre les 37%
          // Une étude expérimentale montre que les mesures de tensions où lien environ toutes les 0.3 à 0.4 ms
          timem = micros() - time0;
        }
        if(isPython){
          Serial.write((uint8_t*)&timem,2);  // Envoi du temps mesuré si programme Python
        } else {
          Serial.print(timem);  // Envoi du temps mesuré si IDE Arduino
          Serial.print(',');
        }
        digitalWrite(INPUT_PIN, HIGH);  // On recharge le condensateur
        delay(duree_2/1000);  // On attend que le condensateur est bien chargé.
      }
    } else {
      if(isPython){Serial.println('A');}
      else {Serial.println("Erreur dans les paramètres");}
    }
    digitalWrite(LED_PIN, LOW);
    if(!isPython){Serial.println("Entrez le nombre d'échantillons de mesure");}
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
