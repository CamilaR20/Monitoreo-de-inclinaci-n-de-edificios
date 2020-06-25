# Monitoreo de inclinación de edificios
El código en este repositorio se utilizó para un sistema que hace uso de un sensor de inclinación (inclinómetro tipo MEMS SCA 100T-D02) para monitorear una edificación cercana a una zona de construcción. La señal proveniente del sensor ya acondicionada es la entrada al ADC de un microcontrolador (PIC24FV32KA304). El sistema almacena la información relevante en una memoria EEPROM (AT24C64D) de 256Kb. Los datos se almacenan en la memoria con una etiqueta (timestamp) HH:MM:SS proporcionada por un RTC externo al microcontrolador (RX8900SA), la etiqueta indica el momento en que se empezaron a almacenar los datos. 
La comunicación entre la memoria, el RTC y el microcontrolador se da mediante I2C.

Con una máquina de estados se implemento un protocolo de comunicación para conectar el sistema con una interfaz gráfica desarrollada en Python.

**Nota:** El sistema cuenta con baterías y panel solar, y el voltaje actual de estos se puede obtener mediate la interfaz.

## Interfaz 💻
![alt text](https://github.com/CamilaR20/Monitoreo-de-inclinacion-de-edificios/blob/master/Interfaz.png?raw=true)

### Funciones de la interfaz
**Cuadro de texto:** En este espacio se visualizan los datos precisados por el usuario y que son enviados por el microcontrolador desde su puerto serial.
1. Panel: Al oprimir este botón, el computador envía por puerto serial los comandos ‘#’ y ‘%’, lo cual le indica a la máquina de estados del sistema que debe enviar el dato del voltaje del panel, el cual es visualizado en el cuadro de texto de la aplicación.
2.  Batería: Al oprimir este botón, el computador envía por puerto serial los comandos ‘#’ y ‘$’, para recibir el dato del voltaje de la batería, que aparecerá en el cuadro de texto de la aplicación.
3. Salir StandBy: Al oprimir este botón, el computador envía por puerto serial la letra ‘a’, lo cual le indica a la máquina de estados que debe salir del modo de StandBy, es fundamental este comando ya que el sistema entra en StandBy después de enviar cada dato pedido por el usuario.
4. Low Power Mode: Al oprimir este botón, el computador envía por puerto serial los comandos ‘#’ y ‘+’, lo cual le indica al microcontrolador que debe entrar en modo de bajo consumo, para salir de este estado es necesario oprimir el botón de Salir StandBy.
5.  Configurar RTC y Spinboxes de configuración: Estos bloques sirven para configurar el RTC desde la GUI, una vez los datos estén completos, se debe oprimir el botón de Configurar RTC, el cual en primer lugar envía los comandos ‘#’ y ‘&’, para indicarle al circuito que debe entrar en modo de configuración del RTC, para después enviar todos los datos ingresados por el usuario. Tan pronto el RTC es configurado, se comienzan a guardar datos de inclinación en memoria.
8. Inclinación: Al oprimir este botón, se envía por el puerto serial los comandos ‘#’ y ‘@’ para pedir los datos de inclinación almacenados en la memoria desde que se configuró el RTC, los datos se guardan en memoria cada 15 minutos; la visualización de los datos se realiza de forma numérica en el cuadro de texto y además los datos son graficados en otra ventana.


## Construido con 🛠️
* [MPLAB x IDE](https://www.microchip.com/mplab/mplab-x-ide) - Programa del microcontrolador

## Autores ✒️
* **Camila Roa**
* **Pablo Mosquera**
* **Santiago Rojas** 

## Licencia 📄

Ver [LICENSE.md](LICENSE.md) para detalles
