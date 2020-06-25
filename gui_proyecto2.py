# -*- coding: utf-8 -*-
"""
Created on Thu Nov 14 14:34:28 2019

@author: macar
"""

from tkinter import *
import tkinter as tk
import serial
from    serial import Serial
import numpy as np
import time
import array
import matplotlib
matplotlib.use('TkAgg')
from    matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from    matplotlib.figure import Figure
from    matplotlib import pyplot as plt
from    drawnow import*
from    datetime import date
from    datetime import datetime

window = Tk()
puerto   =  serial.Serial(port = 'COM3',
                         baudrate = 115200,
                         bytesize = serial.EIGHTBITS,
                         parity   = serial.PARITY_EVEN,
                         stopbits = serial.STOPBITS_ONE)


def clicked1():

    puerto.open()
    puerto.write(ord('#'))
    puerto.write(ord('@'))
    time.sleep(0.5)
    pre_dato = puerto.read()
    print(ord(pre_dato))
    
    while ord(pre_dato) != data_year:
        pre_dato = puerto.read()
        print(ord(pre_dato))
    
    dato_incl_total = puerto.readline()
    datos_incl = array.array('B',dato_incl_total)
    num_data = len(datos_incl)
    datos_fecha = np.concatenate((data_year, datos_incl[0:4]), axis=None)
    print(datos_fecha)
    datos_leidos=datos_incl[5:(num_data-1)]
    if len(datos_leidos)%2 == 0:
        imax = len(datos_leidos)
    else:
        imax = len(datos_leidos)-1
        
    datos_y = [0 for i in range(imax/2)]
    j = 0
    for i in range(0,imax,2):    
        dato_total= 256*int(datos_leidos[i]) + int(datos_leidos[i+1])
        dato_volt = (dato_total*4.096)/(4096*16)   
        dato_degree = ((((dato_volt-2)/34.3))/(0.035))*1.25
        datos_y[j] = dato_degree
        j = j+1

    puerto.close()
    print(datos_y)
    txt.insert(END, "Inclinación")
    txt.insert(END, "   ")
    txt.insert(END, datos_y)
    txt.insert(END,'\n')

    plt.plot(datos_y)
    plt.xlabel("Tiempo")
##   plt.ylabel("Inclinación (°)")
##    plt.suptitle("Inclinación vs Tiempo " + str(datos_fecha[2]) + "-" + str(datos_fecha[1]) + "-" + str(datos_fecha[0]) + "-" + str(datos_fecha[3]) + ":" + str(datos_fecha[4]))
    plt.show()
        
def clicked2():

    puerto.open()
    puerto.write(ord('#'))
    puerto.write(ord('%'))
    time.sleep(0.5)
    dato_pan = puerto.read()
    dato_pan2 = ord(dato_pan)
    dato_pan_volt = 0.01606*(int(dato_pan2))*3
    puerto.close()
    print(dato_pan2)
    print(int(dato_pan2))
    print(float(dato_pan_volt))
    txt.insert(END, "V Panel   ")
    txt.insert(END, dato_pan_volt)
    txt.insert(END, " V") 
    txt.insert(END, "\n")
  



def clicked3():

    puerto.open()
    puerto.write(ord('#'))
    puerto.write(ord('$'))
    dato_bat = puerto.read()
    dato_bat2 = ord(dato_bat)
    dato_bat_volt = 0.01606*(int(dato_bat2))*2
    puerto.close()
    print(dato_bat2)
    print(int(dato_bat2))
    print(dato_bat_volt)
    txt.insert(END, "V Batería   ")
    txt.insert(END, dato_bat_volt)
    txt.insert(END, " V") 
    txt.insert(END, "\n")



def clicked4():
    puerto.open()
    puerto.write(ord('a'))
    puerto.close()

def clicked5():
    puerto.open()
    puerto.write(ord('#'))
    puerto.write(ord('+'))
    puerto.close()

def clicked6():
    puerto.open()
    global data_year
    data_year=var1.get()
    bcd_year = int(str(data_year),16)

    data_month=var2.get()
    bcd_month = int(str(data_month),16)

##    time.sleep(0.1)
    data_day=var3.get()
    bcd_day = int(str(data_day),16)


##    time.sleep(0.1)
    data_hour=var4.get()
    bcd_hour = int(str(data_hour),16)

##    time.sleep(0.1)
    data_minute=var5.get()
    bcd_minute = int(str(data_minute),16)

##    time.sleep(0.1)
    data_second=var6.get()
    bcd_second = int(str(data_second),16)

##    time.sleep(0.1)
    checksum = sum([bcd_year,bcd_month,bcd_day,bcd_hour,bcd_minute,bcd_second])

    datos_bcd = [bcd_year, bcd_month, bcd_day, bcd_hour, bcd_minute, bcd_second,checksum]
    datos_bcdbyte = bytearray(datos_bcd)
    puerto.write(ord("#"))
    puerto.write(ord("&"))
    puerto.write(datos_bcdbyte)
    puerto.close()


puerto.close()
txt = Text(window, width=80, height=12)
txt.place(x=30, y=60)
scrollb = Scrollbar(window, command=txt.yview)
scrollb.place(x=650, y=62)
txt['yscrollcommand'] = scrollb.set

inc_button = Button (window, text="Inclinación", command=clicked1)
inc_button.place(x = 30,y = 30)

panel_button = Button (window, text="Panel", command=clicked2)
panel_button.place(x = 150,y = 30)

bat_button = Button (window, text="Batería", command=clicked3)
bat_button.place(x = 250,y = 30)

leave_SB = Button(window, text="Salir StandBy", command=clicked4)
leave_SB.place(x = 360,y = 30)

LPM = Button (window, text="Low Power Mode", command=clicked5)
LPM.place(x = 500,y = 30)

RTC = Button (window, text="Configurar RTC", command=clicked6)
RTC.place(x = 700,y = 30)



var1 =IntVar()
s1 = Spinbox(window, from_=0, to=100, width=5, textvariable=var1).place(x = 760,y = 60)

var2 =IntVar()
s2 = Spinbox(window, from_=0, to=100, width=5,values=(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12), textvariable=var2).place(x = 760,y = 90)

var3 =IntVar()
s3 = Spinbox(window, from_=0, to=100, width=5, values=(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31), textvariable=var3).place(x = 760,y = 120)

var4 =IntVar()
s4 = Spinbox(window, from_=0, to=100, width=5, values=(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24), textvariable=var4).place(x = 760,y = 150)

var5 =IntVar()
s5 = Spinbox(window, from_=0, to=100, width=5, textvariable=var5).place(x = 760,y = 180)

var6 =IntVar()
s6 = Spinbox(window, from_=0, to=100, width=5, textvariable=var6).place(x = 760,y = 210)

year = Label(window, text = "Year").place(x = 700,y = 60)
month = Label(window, text = "Month").place(x =700 ,y = 90)
day = Label(window, text = "Day").place(x = 700,y = 120)
hour = Label(window, text = "Hour").place(x = 700,y = 150)
minute = Label(window, text = "Minute").place(x = 700,y = 180)
second = Label(window, text = "Second").place(x = 700,y = 210)

              
window.title("Aplicacion monitoreo de inclinacion")
window.geometry('850x300')
window.mainloop()