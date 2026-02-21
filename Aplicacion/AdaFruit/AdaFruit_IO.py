from Adafruit_IO import MQTTClient, Client

class AdaFruit:
    valores = {}
    observadores = []

    def __init__(self):
        # Recuperar valores iniciales con HTTP
        http = Client("XXX", "XXX") #Por seguridad se anula, pero se debe respetar en adafruit los controles del feed
        for feed in ['brillo', 'colorluces', 'estadohumificador', 'intervalohumificador', 'duracionhumificador', 'nummode', 'sonido']:
            try:
                dato = http.receive(feed)
                self.valores[feed] = dato.value
                print(f"Valor inicial de {feed}: {dato.value}")
            except Exception as e:
                print(f"Error al recibir {feed}: {e}")

        #Inicial la recuperacion por tiempo real
        self.client = MQTTClient("XXX", "XX") #Son el usario y la calve de acceso a AdaFruit
        self.client.on_connect = self.conectado
        self.client.on_message = self.mensaje
        self.client.connect()
        self.client.loop_background()

    def conectado(self, client):
        print('Conectado a Adafruit IO!')
        for feed in ['brillo', 'colorluces', 'estadohumificador', 'intervalohumificador', 'duracionhumificador', 'nummode', 'sonido']:
            client.subscribe(feed)

    def mensaje(self, client, feed_id, payload):
        AdaFruit.valores[feed_id] = payload
        print(f'Dato recibido en {feed_id}: {payload}')
        for callback in AdaFruit.observadores:
            callback(feed_id, payload)

    def enviar(self, feed, valor):
        self.client.publish(feed, valor)

    def registrar_observador(self, funcion):
        AdaFruit.observadores.append(funcion)
    

#Recibir la accion de un boton
def EnviarAccion(accion):
    partes = "X,Y".split(",")
    if "," in accion:
        partes = accion.split(",")
        
    if(accion == "Modo1"):
        print("Entrando a modo 1")
        adafruit.enviar("nummode", 1)
    elif(accion == "Modo2"):
        print("Entrando a modo 2")
        adafruit.enviar("nummode", 2)
    elif(accion == "Modo2"):
        print("Entrando a modo 3")
        adafruit.enviar("nummode", 3)
    elif(accion == "Modo2"):
        print("Entrando a modo 4")
        adafruit.enviar("nummode", 4)
        
    elif(partes[0] == "Color"):
        print("Enviando color")
        adafruit.enviar("colorluces", partes[1])

    elif(partes[0] == "Sonido"):
        print("Enviando volumen")
        adafruit.enviar("sonido", partes[1])
    
    elif(partes[0] == "Brillo"):
        print("Enviando brillo")
        adafruit.enviar("brillo", partes[1])

    elif(partes[0] == "EstadoHumificador"):
        print("Enviando estado humificador")
        adafruit.enviar("estadohumificador", partes[1])

    elif(partes[0] == "IntervaloHumificador"):
        print("Enviando intervalo del humificador")
        adafruit.enviar("intervalohumificador", partes[1])

    elif(partes[0] == "DuracionHumificador"):
        print("Enviando duracion humificador")
        adafruit.enviar("duracionhumificador", partes[1])

    else:
        print("No es una accion para adafruit")

# Instancia global
adafruit = AdaFruit()