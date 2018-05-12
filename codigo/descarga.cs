using System;
using System.IO.Ports;
using System.IO;

public class comunicacion
{
    static int opcion;//variable global
    static bool qwerty=true;//controla el final de la descarga
    static DateTimeOffset thisDay = DateTimeOffset.Now;
    public static void Main()
    {       
        string  mens,cadena;
        int i = 0;
        string[] ports = SerialPort.GetPortNames();//Obtenemos el listado de puertos disponibles
        bool cont = true;//para salir del While
        Console.WriteLine("De esta lista escoge el puerto, (numero y enter)");
        foreach (string port in ports)
        {

            Console.WriteLine("numero {1}: {0}", port, i);
            i++;
        }

        Console.WriteLine("Que puerto cogemos?");
        i = Convert.ToInt32(System.Console.ReadLine());
        SerialPort mySerialPort = new SerialPort(ports[i]);
        mySerialPort.BaudRate = 9600;
        // Establecer los tiempos de espera de lectura / escritura.
        //mySerialPort.ReadTimeout = 1000; // Milisegundos.
        //mySerialPort.WriteTimeout = 1000;
        mySerialPort.Open();
        Console.WriteLine("escoge una opcion");
        mySerialPort.Write("3");
        mySerialPort.DataReceived += new SerialDataReceivedEventHandler(Dato_recibido);

        while (cont)
        {
            mySerialPort.DiscardInBuffer();
            mySerialPort.DiscardOutBuffer();                     
            mens = System.Console.ReadLine(); 
            mySerialPort.Write(mens);
            Console.Clear();
            Int32.TryParse(mens, out opcion);
            if(opcion==5) cont=false;
            
            if (opcion == 1)
            {
                mySerialPort.DiscardInBuffer();
                thisDay = DateTimeOffset.Now;
                //Console.WriteLine(thisDay);
                Console.Clear();
                Console.WriteLine("presionar enter coge la fecha del PC");
                mens = System.Console.ReadLine();
                mens=DateTime.Now.ToString("yy-MM-dd-HH-mm");
                Console.WriteLine(mens);
                //mySerialPort.WriteLine(mens);    
                mySerialPort.Write(mens);
            }

        }
    }   
    //MSDN
    private static void Dato_recibido(object pepe, SerialDataReceivedEventArgs e)
    {
        SerialPort sp = (SerialPort)pepe;
        String input = sp.ReadLine();
        String fichero;
        fichero=DateTime.Now.ToString("yy-MM-dd-HH-mm")+".txt";
       
        if (opcion == 2)
        {           
            StreamWriter f;
            if(input == "fin\r")qwerty=false;
            if(qwerty)
            {
              if( File.Exists(fichero))
                {
                    f = File.AppendText(fichero);
                    f.WriteLine(input);
                    f.Close();                
                }
              else
                {
                     f = File.CreateText(fichero);
                     f.WriteLine(input);
                     f.Close();                    
               }
            }
        }
        Console.WriteLine(input);      
    }      
}