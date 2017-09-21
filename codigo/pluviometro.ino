#include <ESP8266WiFi.h>//libreria para el deep-sleep, bajo consumo
#include <SD.h>// libreria para el manejo de ficheros
#include <SPI.h>// libreria para comunicacion SPI
#include <RTClib.h>//libreria para el control del reloj(RTC)

//declaracion de variables globales//

const int chipSelect = D8;//??
RTC_DS1307 RTC;//Crea el objeto RTC
DateTime now;//Obtiene la fecha y la hora del RTC
//Me ne frega.
  int State=0;
//TIME
    int nHour,nMin,nSec = 1;
    boolean alarma1 = true;

//DATE
    int nDay,nMonth,nYear = 1;
   
//FILES
  String folder,file,logN,ruta = "";
void setup() {
//  Serial.begin(9600);

 WiFi.forceSleepBegin();
  //pinMode(lectura,INPUT);  //Declaramos el Pin para el swicht magnetico
 // pinMode(LED_BUILTIN, OUTPUT);//Led del WEMOS Parpadeo si swicht on
  /*
  Serial.begin(9600);//PUERTO serial
  Serial.println("estoy despierto");
  /////**SD**
  Serial.print(F("Iniciando SD ..."));
  if (!SD.begin(chipSelect))
  {
  Serial.println(F("Error al iniciar"));
   return;
  }
*/
SD.begin(chipSelect);
  //inizialimaos el reloj
  RTC.begin();  
  //delay (100);
 // digitalWrite(LED_BUILTIN, LOW);  
  //Serial.println(F("Iniciado correctamente"));
  obtenerHora();
  generarRuta();
  guardarArchivo();   
  
  //Serial.println("vamos a dormir");
  
//MODO dormir para siempre (0); despierta enviando a tierra "rst"
//el balancin hace la funcion de swicht, cada movimiento provoca 
//que despierte del reposo, anote los datos y vuelve al reposo.
if(nDay==1)recuento();
ESP.deepSleep(0);
}

void loop() {
}

 /***************************************************
*FUNCION RUTA: Creamos la ruta con formato:         *
*       folder --> /ano.mes                         *
*       file   -->/ano.mes.dia.csv                  *
*       LogN   -->ano-mes-dia-,hora:minutos:segundos*
*       ruta   -->/ano.mes/ano.mes.dia.csv          *
 ***************************************************/
void generarRuta(){
  //Serial.println("generando ruta");
      folder ="/" + String(nYear) + String(nMonth);
      file ="/"+ String(nYear)  + String(nMonth) + String(nDay) + ".csv";// fecha+extension
      logN = String(nYear) + "-" + String(nMonth) + "-" + String(nDay) + "," //fecha
                      + String(nHour) + ":" + String(nMin) + ":" 
                      + String(nSec) ; //hora;
      ruta = folder + file;
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
 /**********************************************
 * Guarda el archivo de los datos recuperados  * 
 * del reloj, creando una carpeta, y dentro de *
 * esa carpeta aloja el archivo de ese dia de  * 
 * los eventos sucedidos, un dia una hoja.     *
 **********************************************/
 void guardarArchivo(){
  File f;

//      Serial.println("");
//      Serial.println(folder);
//      Serial.println(ruta);
//     Serial.println(logN);
      char fichero[10];
      char dir[50];
      folder.toCharArray(fichero,10);
      ruta.toCharArray(dir,50);      
      //SD.mkdir(fichero);
      if(!SD.exists(fichero)){
        //Serial.println("creando carpeta");
          SD.mkdir(fichero);
      }
      f = SD.open(dir, FILE_WRITE);//abrimos  el archivo
//      Serial.println("ok");
//      Serial.println(dir);
          if (f) { 
//            Serial.println("escrito");              
            f.println(logN);
//            Serial.println("escribimos el log");
            f.close(); //cerramos el archivo  
               
      }    
  }
 
 /********************************************** ****************
 * Esta funcion revisa todos los datos almacenados en un mes,   *   
 * realiza un recuento, y crea un nuevo archivo que consta      *
 * el mes y la cantidad registrada                              *
 ****************************************************************/
 
  void recuento(){
  float suma = 0;
  String mese;
  File l;
  int i,k=0;//contador
  //La funcion SD.open("",.. ) hay que mandarle un char
      char dirr[50];  
  if(nMonth==1){//Debemos de descartar el mes 0.
    mese=12;
  }else{mese=nMonth-1;}
  for (i=1;i<=31;i++){//realizamos un barrido de todos los dias
    ruta ="/" + String(nYear) + 
    String(mese)
    +"/"+ String(nYear)  + String(mese) +"-" + String(i) + ".csv";
    ruta.toCharArray(dirr,25); 

////
if(SD.exists(dirr)){// Como hemos realizado el barrido debemos descartar los que no existan

    l=SD.open(dirr,FILE_READ);
//    Serial.println("hemos leido ");
    if(l){
//       Serial.println("l es verdad");
        while(l.available()){
        /*if( l.peek() == '\n' ){
          k++;//Buscamos el indicador de salto de linea
      Serial.println(folder);
      Serial.println(ruta);
          }*/
          if(l.read()=='\n')k++;
          }
      l.close();
      suma=suma+k*0.44853;//El valor numérico debe ser la relacion del volumen del balancin, y tamano de embudo
      }

   }    
    }
    ruta="/"+ String(nYear) + ".csv" ;
    ruta.toCharArray(dirr,25);  
    //A continuacion iniciamos la traduccion del numero a letra, 1->Enero, 2->Febr..
    if(mese=1)mese = "Enero";
    if(mese=2)mese = "Febrero";
    if(mese=3)mese = "Marzo";
    if(mese=4)mese = "Abril";
    if(mese=5)mese = "Mayo";
    if(mese=6)mese = "Junio";
    if(mese=7)mese = "Julio";
    if(mese=8)mese = "Agosto";
    if(mese=9)mese = "Septiembre";
    if(mese=10)mese = "Octubre";
    if(mese=11)mese = "Noviembre";
    if(mese=12)mese = "Diciembre";

    if(l){
      l.println(mese+","+String(suma)+","+"mm");
//      Serial.println("hecho");
    }
    l.close();
  
}

