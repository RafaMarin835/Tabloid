from AdaFruit.AdaFruit_IO import AdaFruit, adafruit, EnviarAccion

#Para recibir un dato (convertido a texto) se puede hacer asi solo se recibe la primera vez que lo llames
f"Dato: {adafruit.valores.get('nummode')}" #en el get(), solo debes sustituir el dato que ocupas
#Los datos pueden ser (brillo, colorluces, horaactual, horahumificador, nummode, sonido, led)

#Si ocupa recibir un dato constantemente (actualizar en tiempo real)
def actualizar_modo(feed_id, valor): #Aca le das el nombre que quiera al metodo, actualiza en tiempo real
    if feed_id == "nummode": #aca le pasas que dato quieres mantener actualizado (brillo, colorluces, horaactual, horahumificador, nummode, sonido, led)
        LabelModos.config(text=f"El modo es: {valor}")  #aca actualizo el label si se hizo un cambio en adafruit, 
                #esta palabra es cambiar el nombre del LabelModos por el label que ocupes actualizar
adafruit.registrar_observador(actualizar_modo) #Esta linea es la que mantiene la actualizacion en tiempo real


#Si ocupas enviar una accion, ejemplo prender o apagar la led o cambiar el modo
EnviarAccion(accion) #Usas este, el accion es lo que quieres cambiar, seria la siguiente lista
""" Cuando vas a enviar un adato lo pasas por una varibale desde un boton o otros
    accion puede tomar varios valores
    
    accion = Modo1 (puede ser modo3 y asi con otros)
    accion = "Color,#0055d3" -> debes pasarlo con ese indice de "Color" una coma y el color a poner
    accion = "Sonido,5"  -> lo mismo que color pero con un rango de 0 a 10
    accion = "Brillo,110" -> lo mismo pero con un rango de 0 a 255

    
    accion = "EstadoHumificador,ON" -> puede ser OFF
    accion = "IntervaloHumificador,HH:MM:SS" -> se refiere a pasar horas,minutos y segundos, en ese orden, este es para cada cuanto se lanza el humificador
    accion = "DuracionHumificador,HH:MM:SS" -> se refiere a pasar horas,minutos y segundos, en ese orden, es para la duracion del humificador
"""
