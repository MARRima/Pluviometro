#include <Wire.h>  // Incluye la libreria Wire
#include <RTClib.h> // Incluye la libreria RTClib
#include <SD.h>// libreria para el manejo de ficheros
#define pin D0

RTC_DS1307 RTC; // Crea el objeto RTC
DateTime now;//Obtiene la fecha y la hora del RTC

//Declaramos las variables que utilizaremos durante la ejecucón
const int chipSelect = D8;//CS de la SD
int nHour,nMin,nSec = 1;
boolean alarma1 = true;
int nDay,nMonth,nYear = 1;
String file,logN,ruta = "";
char fecha[18];
int tiempo[6];
bool garfagnana= true;
int opcion=0;

//Está es la función principal que se ejecuta.
void setup () {
  
  pinMode(pin,INPUT);
  Serial.begin(9600); // Establece la velocidad de datos del puerto serie
  Wire.begin(); // Establece la velocidad de datos del bus I2C
  RTC.begin(); // Establece la velocidad de datos del RTC
  SD.begin(chipSelect);
  
   // He tomado un ejemplo de la pagina de Arduino, de tal forma que con el goto
   //Se anula la opción de poner en reposo el microcontrolador saltando toda esa parte
   // Por defecto solo se ejecuta setup(), entra en reposo y cuando despierta ejecuta de nuevo setup ()
   
  //ESP.deepSleep(0);
  if(digitalRead(pin)==HIGH)goto bailout;//pin es para pasar al modo configuración del microcontrolador
  obtenerHora();
  generarRuta();
  guardarArchivo(); 
  ESP.deepSleep(0);//envia en reposo al dispositivo
  bailout:;
}
/* *********************************************************
 * Loop () se ejecuta por defecto siempre de forma          *  
 * salvo en este caso con la función deepsleep solo entrara *
 * en el caso de estar activado el botón de                 *
 * configuración.                                           *
 ***********************************************************/
void loop () {


  if(digitalRead(pin)==LOW)ESP.deepSleep(0);
  // si no esta activo el botón, quiere decir que hemos acabado con la configuración y lo envía en reposo.

  switch(opcion)
  {
    //garfagnana lo utilizo para saber cuando pasa el programa por la función.
    
    case 1:
    garfagnana=true;
    if (Serial.available() > 0)//Si hay datos en la entrada se entra en el if
    {
      agiorna ();// función para actualizar la fecha 
      garfagnana=true;
      opcion=0;//Para poner el marcador a 0 y llamar el caso default.
      break;
    }else {opcion=1;
     if (garfagnana==false)Serial.println("formato xx-xx-xx-xx-xx, quiere decir Anno-mes-dia-hora-min dos cifras!!");
    garfagnana=true;
    break;
    }
    
    case 2:
    leerymostrar();//Para mostrar por pantalla toda la información almacenada en la tarjeta de memoria
    opcion=0;
    garfagnana=true;
    break;
    case 3:
    //El caso 3 es un tester para controlar que todo es correcto.
     obtenerHora();
     generarRuta();
     Serial.println();
     Serial.println(logN);
     Serial.println();
     opcion=0;
     garfagnana=true;
    break;          
    case 4:
    //Caso 4: Es borrar la tarjeta de memoria
        opcion=0;
        borrar();
        garfagnana=true;
         break;         
    case 5:
    //Para acabar y dejar en reposo
      opcion=0;
      ESP.deepSleep(0);
    break;  
    default:
    if(garfagnana)
    {
      garfagnana=false;
      ordenes();
      
    }
    
    if(Serial.available()>0){
    opcion=Serial.parseInt();//Almacena la opción tomada
    //Serial.print("la opcion es = ");
    //Serial.println(opcion);
    }
    break;
  }

}

void agiorna ()
{
    Serial.println("cadena aceptada");
    (Serial.readString()).toCharArray(&fecha[0],18) ;
    Serial.println(fecha);
    //El siguinete bucle  es para transformar la String en un entero, 
    //la información contenida en el String se encuentra en ASCII y se debe convertir
    for(int i=0;i<5;i++)
    {
      tiempo[i]=10*(fecha[3*i]-'0')+fecha[3*i+1]-'0';
      Serial.println(tiempo[i]) ;
    }
/*
 * va esperar el siguiente formato xx-xx-xx-xx-xx, quiere decir Anno-mes-dia-hora-min dos cifras!!
 * tiempo[0]:anno
 * tiempo[2]:mes
 * tiempo[3]:dia
 * tiempo[4]:min
 * 0: Segundos
*/
    RTC.adjust(DateTime(tiempo[0], tiempo[1],tiempo[2], tiempo[3], tiempo[4], 0));

}
 /**********************************************
 * Guarda el archivo de los datos recuperados  * 
 * del reloj, creando una carpeta, y dentro de *
 * esa carpeta aloja el archivo de ese dia de  * 
 * los eventos sucedidos, un dia una hoja.     *
 **********************************************/
 
 void guardarArchivo(){
  File f;      
      Serial.println(ruta);
      Serial.println(logN);
      f = SD.open(ruta, FILE_WRITE);//abrimos  el archivo   
    
          if (f) 
      { 
       
            Serial.println("escrito");              
            f.println(logN);
            Serial.println("escribimos el log");
            f.close(); //cerramos el archivo  
               
      }    
  }

 /*********************************************
* Funcion acceder al RTC, obtenemos la fecha *
* a la que se producira el evento            *
*********************************************/

void obtenerHora(){
    //Serial.println("obteniendo hora");
    now = RTC.now();// obtiene la fecha y la hora del RTC
    nHour = now.hour();
    nMin = now.minute();
    nSec = now.second();
    nDay = now.day();
    nMonth = now.month();
    nYear = now.year();
  }

/***************************************************
*FUNCION RUTA: Creamos la ruta con formato:         *
*       file   -->/ano.mes.dia.csv                  *
*       LogN   -->ano-mes-dia-,hora:minutos:segundos*
*       ruta   -->/ano.mes/ano.mes.dia.csv          *
 ***************************************************/
void generarRuta(){
  //Serial.println("generando ruta");
    float v;
      v=analogRead(A0)*5.0/523; 
      
      ruta =String(nMonth) + ".csv";// fecha+extension
      logN = String(nDay)+"/"+ String(nMonth)+"/"+String(nYear) +  "," //fecha
                      + String(nHour) + ":" + String(nMin) + ":" 
                      + String(nSec)+ ","+String(v); //hora;
  }

/* ************************************************
 * Funcion LEER LECTURAS,                         *
 * extrae la información de la tarjeta de memoria * 
 *************************************************/
 
void leerymostrar()
{
  String lectura;
  File f;

  for(int i=0;i<=11;i++)
    {
      ruta = String(i+1) + ".csv";// fecha+extension
      if( f=SD.open(ruta)){
        
       while(f.available())Serial.write(f.read());
       
      }//else Serial.println("falso");
    }
    Serial.println("fin");
}
void ordenes(){
  Serial.println("opciones disponibles");
  Serial.println("1-Actualizar fecha");
  Serial.println("2-descargar datos");
  Serial.println("3-Dame la fecha");
  Serial.println("4-FormatearSD");
  Serial.println("5-acabar");
  
}

/* ************************************************
 * Funcion Borrar ,                               *
 * Borra toda la información contenida en la      *
 * tarjeta de memoria                             * 
 *************************************************/
void borrar()
{
    File f;

  for(int i=0;i<=11;i++)
    {
      ruta = String(i+1) + ".csv";// fecha+extension
      if( f=SD.open(ruta)) SD.remove(ruta);  
    }  

}
