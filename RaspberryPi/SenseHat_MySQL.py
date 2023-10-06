from sense_hat import SenseHat
from time import sleep
from datetime import datetime
import MySQLdb
import sys

def GetAktData(hat):
    pressure=hat.get_pressure()
    temp=hat.get_temperature()
    humidity=hat.get_humidity()
    return pressure,temp,humidity

def StartMySQL(hostn,usern,passwdn,dbn):
    db = MySQLdb.connect(host=hostn,user=usern,passwd=passwdn,db=dbn)
    cur = db.cursor()
    print("MySQL-Connection initialized at %s." % datetime.now())
    return db,cur

def InsertData(hat,db,cur):
    pres,temp,hum=GetAktData(hat)
    print("Data at %s.:" % datetime.now())
    print("Pressure: %s" % pres)
    print("Temperature: %s" % temp)
    print("Humidity: %s \n" % hum)
    cur.execute("CALL `<DBNAME>`.`new_Weatherdata`('RPI_WS_NAME', %s,%s, %s);" % (hum,temp,pres))
    db.commit()
    return

if __name__ == '__main__':
    print("Program Start at %s." % datetime.now())
    sense=SenseHat()
    sense.clear()
    try:
        while True:
            con,com=StartMySQL("<SERVERURL>","<USERNAME>","<PASSWORD>","<DBNAME>") // OPEN MySQL Connection
            InsertData(sense,con,com)
            com.close()
            con.close() // Close the Connection
            sleep(60) // do again after 60 seconds
    except Exception as e:
        print(e)
    finally:
        print("Program Ending at %s" % datetime.now())



