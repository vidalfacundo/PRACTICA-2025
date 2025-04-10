/*
PROYECTO: "Control de temperatura de estación transformadora subterránea”
ALUMNO: FACUNDO VIDAL
PROFESOR: LANFRANCO LISANDRO
*/
#include <LiquidCrystal_I2C.h>  //Incluye la libreria de LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2); //direccion, columnas, lineas

#include <DHT.h>    //Incluye la libreria de DHT
#define DHTPIN 2     // Pin al que está conectado el DHT22
#define DHTTYPE DHT11   // Definir el tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

#include <avr/interrupt.h> //Incluye la libreria de interrupciones de AVR
unsigned int desborde = 62500; //   16000000/256=62500 pulsos x segundo
int contador = 0;


const int botonSubirPin = 5;    // Pin del pulsador de incremento
const int botonBajarPin = 4;  // Pin del pulsador de decremento
const int ModuloRelay = 3;  // Pin del Modulo Relay
int set_point = 20;         // Variable a incrementar/decrementar
unsigned long tiempo_actual;
unsigned long tiempo_anterior1=0;
unsigned long delta_tiempo1 =0;
bool bandera_tiempo;
int i=0;
int temperatura;


void setup() {
  
  lcd.init(); // Iniciar LCD
  lcd.backlight(); // luz de fondo LCD
  Serial.begin(9600); // abre puerto serie
  dht.begin();
  
  pinMode(ModuloRelay, OUTPUT);    // Configurar el pin 
  digitalWrite(ModuloRelay, LOW);  // Asegurarse de que el relé esté apagado al inicio
  pinMode(botonSubirPin, INPUT);    // Configurar el pin 
  pinMode(botonBajarPin, INPUT);  // Configurar el pin 

  TCCR1A = 0;
  TCCR1B = 0; 
  OCR1A = desborde;
  TCCR1B = (1<<WGM12) | (1<<CS12);
  TIMSK1 = (1<<OCIE1A);
  sei(); 
}


void loop() {

 
  // LECTURA DE TEMPERATURA
  int temperature = dht.readTemperature(); //lectura de sensor DHT22
  temperatura=temperature; // valor de temperatura a una variable global para usarla en interrupcion
  

  // ESCRITURA EN LCD
  lcd.setCursor(0, 0);
  lcd.print("temp medida:");
  lcd.setCursor(12, 0);
  lcd.print(temperature);
  lcd.setCursor(14, 0);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("set  point: ");
  lcd.setCursor(12, 1);
  lcd.print(set_point);
  lcd.setCursor(14, 1);
  lcd.print((char)223);
  lcd.print("C");

  //METRODO DE CONTROL DE TEMPERATURA
  if (temperature > set_point+2) {  //rango de optimo set poit + 2 grados.
    
    //RETARDO A LA CONEXION
    for(;i<1; i++){
    delta_tiempo1=millis(); //guardo 1 valor instantaneo de millis
    }
    tiempo_actual=millis(); // actualizo valor de millis en otra variable
      if (tiempo_actual >= delta_tiempo1+5000 && bandera_tiempo==false){ 
        digitalWrite(ModuloRelay, HIGH);  // Encender el relé
        bandera_tiempo=true;
        }
      
  } 
    if (temperature <= set_point){
    digitalWrite(ModuloRelay, LOW);   // Apagar el relé
    bandera_tiempo=false;
    i=0;
  }
  

////METRODO DE CONTROL DE BOTONERA
if(digitalRead(botonSubirPin)==HIGH){
    set_point++;
    if(set_point > 40){
      set_point=40;
    }
    delay(200);  // Esperar para evitar rebotes y cambios rápidos
}
if(digitalRead(botonBajarPin)==HIGH){
    set_point--;
    if(set_point < 20){
      set_point=20;
    }
    delay(200);  // Esperar para evitar rebotes y cambios rápidos
}

}

//Interrupcion para enviar info por purto serie cada 1 segundo
ISR(TIMER1_COMPA_vect){
  //contador++;
  //if (contador == 2){
    Serial.print(temperatura);
    Serial.print(",");
    Serial.println(set_point);
    //contador=0;
   //}
}
  
