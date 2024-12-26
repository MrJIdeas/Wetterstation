import socket
import threading
from sense_hat import SenseHat

# SenseHat-Instanz erstellen
sense = SenseHat()

# Funktion zum Abfragen der Sensorwerte
def get_sensor_data():
    temperature = sense.get_temperature()
    pressure = sense.get_pressure()
    humidity = sense.get_humidity()

    # Sensorwerte runden
    temperature = round(temperature, 2)
    pressure = round(pressure, 2)
    humidity = round(humidity, 2)

    return temperature, pressure, humidity

# Funktion zur Verarbeitung von Anfragen mit dauerhafter Verbindung
def handle_client(client_socket):
    try:
        while True:
            # Empfang der Anfrage vom Client (hier auf maximale Länge von 1024 Bytes beschränkt)
            request = client_socket.recv(1024).decode('utf-8').strip()
            # Verbindung beenden, wenn keine Anfrage kommt (leere Anfrage)
            if not request:
                print("[*] Client disconnected.")
                break

            # Prüfen, ob die Anfrage dem Format <RA></RA> entspricht
            if request == "<RA></RA>":
                temperature, pressure, humidity = get_sensor_data()

                # Antwort im gewünschten Format zusammenstellen
                response = f"<RA>T={temperature};P={pressure};H={humidity};OK</RA>"
                client_socket.send(response.encode('utf-8'))
            else:
                # Ungültige Anfrage
                client_socket.send("<RA>Invalid Request</RA>".encode('utf-8'))
    except Exception as e:
        print(f"[!] Error: {e}")
    finally:
        # Schließen der Verbindung zum Client
        client_socket.close()

# Funktion zum Starten des TCP-Servers
def start_server(host='0.0.0.0', port=5000):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((host, port))
    server.listen(5)
    print(f"[*] Server listening on {host}:{port}")

    while True:
        # Auf eingehende Verbindungen warten
        client_socket, addr = server.accept()
        print(f"[*] Connection accepted from {addr}")

        client_handler = threading.Thread(target=handle_client, args=(client_socket,))
        client_handler.start()

if __name__ == "__main__":
    try:
        start_server()
    except KeyboardInterrupt:
        print("\n[*] Server shutting down.")
