import tkinter as tk
from PIL import Image, ImageTk

from Ventanas.Ventana_Home import Ventana_Home

class Ventana_Login:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Ventana de Login")
        self.root.geometry("440x956")

        # Cargar imagen de fondo
        imagen = Image.open("fondos/LOGIN.png")
        self.fondo = ImageTk.PhotoImage(imagen)

        # Canvas para mostrar fondo
        self.canvas = tk.Canvas(self.root, width=440, height=956)
        self.canvas.pack(fill="both", expand=True)
        self.canvas.create_image(0, 0, image=self.fondo, anchor="nw")

        # Campo de texto usuario
        self.textUsuario = tk.Entry(self.root, font=("Coolvetica", 18))
        self.textUsuario.config(
            bg="#F8EDBB",
            fg="black",
            insertbackground="black",
            borderwidth=0,
            highlightthickness=0,
            relief="flat"
        )
        self.canvas.create_window(220, 447, window=self.textUsuario, width=300, height=45)

        # Campo de texto contraseña
        self.textContraseña = tk.Entry(self.root, font=("Coolvetica", 18), show="*")
        self.textContraseña.config(
            bg="#F8EDBB",
            fg="black",
            insertbackground="black",
            borderwidth=0,
            highlightthickness=0,
            relief="flat"
        )
        self.canvas.create_window(210, 515, window=self.textContraseña, width=280, height=45)

        # Zona invisible clickeable
        self.crear_zona(self.canvas, 45, 600, 388, 653, "VentanaHome", self.accionBoton)

        self.root.mainloop()

    def accionBoton(self, event, accion): #esta es la funcion con la que envias que hace cada boton
        usuario = self.textUsuario.get()
        contraseña = self.textContraseña.get()
        if not usuario or not contraseña:
            print("Rellena los espacios")
            return
        print("Accion:", accion)
        if(accion == "VentanaHome"): #Puedes usar esto para diferenciar si es una accion a adafruit o si es cambio de ventana
            self.root.destroy() #Elimina la ventana de login
            Ventana_Home()


    def crear_zona(self, canvas, x1, y1, x2, y2, accion, handler): #Para crear los botones invisibles
        zona = canvas.create_rectangle(x1, y1, x2, y2, outline="", fill="", tags=accion)
        canvas.tag_bind(accion, "<Button-1>", lambda event: handler(event, accion))
    

if __name__ == "__main__": #Para llamarse asi mismo y hacer pruebas
    Ventana_Login()