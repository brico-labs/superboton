#define silencio  0 // Silencio
#define parada    1 // Parada
#define fin       2 // Final
#define DO   262 // Do 3
#define DO_  277 // D0# 3
#define RE   294 // Re 3
#define RE_  311 // Re# 3
#define MI   330 // Mi 3
#define MI_  349 // Mi# 3
#define FA   370 // Fa 3
#define SOL  392 // Sol 3
#define SOL_ 415 // Sol# 3
#define LA   440 // La 3
#define LA_  466 // La# 3
#define SI   494 // Si 3


int LEDPIN [] = {8, 9, 10, 11};
int vibraPIN  = 9;
int sonidoPIN = 4;
int salidaPIN = 5;

char paradas [] = {12, 21, 33, 40};
int indiceParada = 0;
unsigned long tiempoParada = 10000;
unsigned long millisParada = 0;
int numParadas;




unsigned long salidaTiempoOn;
unsigned long tiempoTemporiza = 20000;

unsigned int frecuencia_nota [12];

int casa = SOL;

int longitudMelodia;



unsigned long ahora;
int estado, estadoAnterior;


// boton
boolean botonPulsado, botonAunPulsado;
unsigned long millisBoton;
int botonPIN  = 3;
int tiempoBoton = 500;  // tiempo de filtrado del boton.

// salida
unsigned long millisSalidaON, millisSalidaOFF;
unsigned long tiempoSalidaON = 10000;
unsigned long tiempoSalidaOFF = 5000;
boolean salidaActivada;

// melodia
boolean sonidoActivado = LOW;
unsigned long millisSonidoOFF;
int contadorNotas;

int contadorParadas;





char duracion [] = {2,1,1,2,2,  1,1,1,1,2,2,  0,
                    2,1,1,1,1,1,1,  4,1,2,4,  0,
                    2,1,1,2,2,  1,1,1,1,2,2,   2,1,1,1,1,1,1,   4,2,  0};

int nota [] = {SOL,SOL,FA,MI,MI,  SOL,SOL,SOL,FA,MI,MI,  parada, 
               SOL,SOL,SOL,LA,SOL,FA,MI,  FA,MI, RE, silencio,  parada,
               FA,FA,MI,RE,RE,  FA,FA,FA,MI,RE,RE,   FA,FA,FA,SOL,FA,MI,RE,   MI,DO, fin};



void setup() 
{
  // Configura los pines de entrada y salida
  pinMode (botonPIN, INPUT);
  for (int i = 0; i < 4; i++) {pinMode (LEDPIN [i], OUTPUT);}

  pinMode (vibraPIN,  OUTPUT);
  pinMode (sonidoPIN, OUTPUT);
  pinMode (salidaPIN, OUTPUT);

  // Calcula la longitud del array con la melodia
  longitudMelodia = sizeof (duracion);
  
  // Calcula el numero de paradas y establece la ultima
  numParadas = int (sizeof (paradas));
  paradas [numParadas - 1] = longitudMelodia;
  
  estado = 0; estadoAnterior = estado;

  Serial.begin (9600);
  
  Serial.print ("Longitud de la cancion = "); Serial.println (longitudMelodia);
  Serial.print ("          numero de paradas "); Serial.println (numParadas);
  for (int i = 0; i < numParadas; i++)
  {Serial.print ("          parada de la melodia en "); Serial.println (int (paradas [i]));}
  
}

void loop() 
{
  // estado = 0: reposo esperando pulsacion de boton
  // estado = 1: boton pulsado esperando a que se cumpla el tiempo de filtrado
  // estado = 2: Reproduce la musica hasta el punto de parada
  // estado = 3: completa la temporizacion
  // estado = 4: espera a que se suelte el boton
  // estado = 5: decide si vuelve a iniciar el contador de paradas de la melodia
  // estado = 6: reproduce la melodia completa
  // estado = 7: reproduce la melodia completa y vuelve al ciclo de funcionamiento

  
  int resultado;

  botonPulsado = digitalRead (botonPIN);
  ahora = millis();
  
  switch (estado) {
    case 0:
      if (botonPulsado)
      {
        millisBoton = ahora;
        estado = 1;
      }
      break;
      
    case 1:
      if (!botonPulsado) estado = 0;
      else
      {
        if (ahora > (millisBoton + tiempoBoton))
        {
          millisSalidaON = ahora; 
          salidaActivada = true;
          sonidoActivado = true;
          estado = 2;
        }
      }
      break;
      
    case 2:
      // Llama repetidamente a la funcion melodia hasta que
      // llegue a una señal de parada o se acaben las notas
      resultado = melodia(1);
      
      // Si llega a una señal de parada interrumpe la melodia
      if (resultado == 1)
      {
        ahora = millis();
        millisSonidoOFF = ahora;
        estado = 3;
      }
      
      // Si llega al final de la melodia la reproduce salta al 
      // bloque numero 6

      if (resultado == 2) 
      {
        delay (2000);
        estado = 6;
      }
      break;
      
    case 3:
      // contadorNotas = 0;
      if ((ahora > millisSalidaON + tiempoSalidaON) && (ahora > millisSonidoOFF + 5000))
      {
        millisSalidaOFF = ahora;
          salidaActivada = false;
          sonidoActivado = false;
        estado = 4;
      }
      break;
      
    case 4:
      if (!botonPulsado) estado = 5;
      break;
      
    case 5:
      if (ahora > millisSalidaOFF + tiempoSalidaOFF)
      {
        contadorParadas = 0;
        estado = 0;
      }
      else if (botonPulsado) estado = 0;
      break;

    case 6:   
      // Llama repetidamente a la funcion melodia en modo melodia completa
      if (melodia (2) == 2) 
      {
        delay (2000);
        estado = 7;
      }
      break;

    case 7:
      // Llama repetidamente a la funcion melodia en modo melodia completa
      // y vuelve al estado 3
      if (melodia (2) == 2) 
      {
        ahora = millis();
        millisSonidoOFF = ahora;
        estado = 3;
      }
  }
  
  // Actualiza el valor de la salida
  digitalWrite (salidaPIN, salidaActivada);

  
  if (estado != estadoAnterior)
  {
    Serial.print ("Cambio de estado desde ");
    Serial.print (estadoAnterior);
    Serial.print ("  ->  ");
    Serial.print (estado);
    Serial.println ();
    
    estadoAnterior = estado;
  }
  
}



int melodia (int modo)
{
  // modo = 0 inicia la melodia desde el principio
  // modo = 1 continua la melodia donde estaba
  // modo = 2 reproduce desde el principio sin paradas
  
  
  // return = 0 indica que hay mas notas para reproducir
  // return = 1 indica que llego a una señal de parada
  // return = 2 indica que llego al final.
  
  
  static int contadorNotas;
  // Si es necesario empieza la melodia desde le principio
  if ((modo == 0)) contadorNotas = 0;
  
  // Si llega a la ultima nota no hace nada y devuelve un 2
  if (nota [contadorNotas] == fin) 
  {
    contadorNotas = 0;
    return 2;
  }
  
  // Para si llega a una señal de parada y no esta en modo continuo
  // indica que hay 
  else if ((nota [contadorNotas] == parada) && (modo != 2))
  {
    contadorNotas ++;
    return 1;
  }
  
  // En otro caso reproduce la nota correspondiente  
  else
  {
    
    // Si la nota es un silencio no reproduce ninguna nota
    if (nota [contadorNotas] == silencio) noTone (sonidoPIN);
    
    // En caso contrario enciende una luz en secuencia aleatoria y suena la nota
    else
    {
      digitalWrite (LEDPIN [random (4)], HIGH);
      tone (sonidoPIN, int (nota [contadorNotas])); 
    }

    delay (int (duracion [contadorNotas]) * 200);
    noTone (sonidoPIN);
    for (int i = 0; i < 4; i++) {digitalWrite (LEDPIN [i], LOW);}
    
    // Imprime informacion sobre la nota
    Serial.print (contadorNotas);        
    Serial.print ("\tNota ");            Serial.print (nota [contadorNotas]); 
    Serial.print ("\tduracion ");        Serial.print (int (duracion [contadorNotas]));
    Serial.print ("\tmilisSalidaOn = "); Serial.print (millisSalidaON);
    Serial.println ();


    // Espera la duracion de la nota y apaga todas las luces
     contadorNotas ++;
     delay (10);
     return 0;
  }
}

