import tkinter as tk
from PIL import Image, ImageTk
from AdaFruit.AdaFruit_IO import AdaFruit, adafruit, EnviarAccion

def Ventana_Home():
    root = tk.Tk()
    root.title("Ventana Home")
    root.geometry("440x956")

    # Cargar imagen de fondo
    imagen = Image.open("fondos/HOME.png")
    fondo = ImageTk.PhotoImage(imagen)

    canvas = tk.Canvas(root, width=440, height=956)
    canvas.pack(fill="both", expand=True)
    canvas.fondo = fondo  # Mantener referencia para que no desaparezca
    canvas.create_image(0, 0, image=canvas.fondo, anchor="nw")

    # Label para modo actual
    LabelModos = tk.Label(canvas, text=f"El modo es: {adafruit.valores.get('nummode')}", font=("Arial", 14))
    LabelModos.place(x=100, y=50)

    # Función para manejar clics
    def accionBoton(event, accion):
        print("Accion:", accion)
        EnviarAccion(accion)

    # Función para crear zonas visibles
    def crear_zona(x1, y1, x2, y2, accion, handler, color="blue"):
        rect = canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="black", tags=accion)
        canvas.tag_bind(accion, "<Button-1>", lambda event: handler(event, accion))
        return rect

    # Definir posiciones de los 16 botones (ejemplo)
    botones = [
        (185, 453, 255, 523, "Apagado/encendido"),
        (63, 570, 126, 633, "Modo1"),
        (153, 570, 216, 633, "Modo2"),
        (243, 570, 303, 633, "Modo3"),
        (333, 570, 399, 693, "Modo4"),
        (148, 730, 160, 742, "Brillo,51"),
        (190, 728, 206, 744, "Brillo,102"),
        (232, 726, 252, 746, "Brillo,153"),
        (274, 724, 298, 748, "Brillo,204"),
        (317, 722, 345, 750, "Brillo,255"),
        (148, 799, 160, 811, "Sonido,6"),
        (190, 797, 206, 813, "Sonido,12"),
        (232, 795, 252, 815, "Sonido,18"),
        (274, 793, 298, 817, "Sonido,24"),
        (316, 790, 344, 818, "Sonido,30"),
     
    ]

    # Crear todos los botones visibles
    for x1, y1, x2, y2, accion in botones:
        crear_zona(x1, y1, x2, y2, accion, accionBoton, color="blue")  # Azul visible

    # Función para actualizar modo desde Adafruit
    def actualizar_modo(feed_id, valor):
        if feed_id == "nummode":
            LabelModos.config(text=f"El modo es: {valor}")

    adafruit.registrar_observador(actualizar_modo)

    root.mainloop()


if __name__ == "__main__":
    Ventana_Home()
