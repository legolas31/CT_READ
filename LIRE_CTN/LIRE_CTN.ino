#include <math.h>
#define PIN_NTC 0
int LedGreenPin = 13;  
int LedRedPin = 12;  
double Rref = 2490.0; //Résistance de référence à 25°C
double V_IN = 5.0; //Alimentation électrique

//Information de la thermistance

float Tun = 10.0;
float Run = 9950.0;
//print('\t----------------------------------')
float Tdeux = 40.0;
float Rdeux = 2663.0;
//print('\t----------------------------------')
float Ttrois = 120.0;
float Rtrois = 194.0;

// calculs en kelvin
float KTun = Tun + 273;
float KTdeux = Tdeux + 273;
float KTtrois = Ttrois + 273;

double Yun = 1/KTun;
double Ydeux = 1/KTdeux;
double Ytrois = 1/KTtrois;

float Lun = log(Run);
float Ldeux = log(Rdeux);
float Ltrois = log(Rtrois);

// calculs intermediaires
float a = (Ldeux-Ltrois)/(Lun-Ldeux)*(pow (Ldeux,3) - pow (Lun,3)) + (pow (Ldeux,3) - pow (Ltrois,3));
float b = Ydeux - Ytrois - ((Ldeux-Ltrois)/(Lun-Ldeux))*(Yun-Ydeux);

// calculs de A, B et C
double C = b / a;
double B = (1/(Lun-Ldeux))*(Yun-Ydeux-C*(pow(Lun,3) - pow(Ldeux,3)));
double A = Yun - B*Lun - C*pow (Lun,3);

double A_1 = Yun - B*Lun - C*pow (Lun,3);
double B_1 = (1/(Lun-Ldeux))*(Yun-Ydeux-C*(pow(Lun,3) - pow(Ldeux,3)));
double C_1 = b/a;
//double D_1 = 6.383091E-8;

void printDouble( double val, unsigned long precision){
// prints val with number of decimal places determine by precision
// NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
// example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

   Serial.print (int(val));  //prints the int part
   Serial.print("."); // print the decimal point
   unsigned int frac;
   if(val >= 0)
     frac = (val - int(val)) * precision;
   else
      frac = (int(val)- val ) * precision;
   int frac1 = frac;
   while( frac1 /= 10 )
       precision /= 10;
   precision /= 10;
   while(  precision /= 10)
       Serial.print("0");

   Serial.println(frac,DEC) ;
}

double SteinhartHart(double R)
{
  //Division de l'équation en 4 parties. La premiere est 
  //uniquement A1
  
  double equationB1 = B_1 * log(R);
  double equationC1 = C_1 * pow(log(R), 3);
  double equation = A_1 + equationB1 + equationC1;
  return pow(equation, -1);

}

void setup() {
  Serial.begin(19200);
  pinMode(LedGreenPin, OUTPUT);
  pinMode(LedRedPin, OUTPUT);
}

void loop() {

  //Calcul de la tension sur la borne analogique
  double valeurAnalog = analogRead(PIN_NTC);
  double V =  valeurAnalog / 1024 * V_IN;
  //printDouble(B_1,10000000);
  //Calcul de la résistance de la thermistance
  double Rth = (Rref * V ) / (V_IN - V);
  Serial.print("Rth = ");
  Serial.println(Rth);

  //Calcul de la température en kelvin( Steinhart and Hart)
  double kelvin = SteinhartHart(Rth);
  double celsius = kelvin - 273.15; //Conversion en celsius
  Serial.print("Ohm  -  T = ");
  Serial.print(celsius);
  Serial.println("C\n");
  //gestion des LED
  if (celsius < 50) {
    digitalWrite(LedGreenPin, HIGH);
    digitalWrite(LedRedPin, LOW);
  }
  else if (celsius >= 60 && celsius < 70) {
    //Warning! User attention required
  }
  else {
     digitalWrite(LedGreenPin, LOW);
     digitalWrite(LedRedPin, HIGH);
  }

  delay(1000);
}
