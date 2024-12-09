import tkinter as tk
from smbus import SMBus

addr = 0x8
bus = SMBus(1)

# bus.read_i2c_block_data(addr, 0x00, 2)
# bus.write_i2c_block_data(addr, 0x00, [0,0]})

# Função para ler e atualizar os valores enviados pela ESP32
def update_esp_values():    
    data = bus.read_i2c_block_data(addr, 0x00, 2) #Lê valores recebidos

    match data[0]:
        case 0:
            toggleS()

        case 1:
            toggle_red()

        case 2:
            toggle_green()

        case 3:
            toggle_blue()

        case 4:
            potR = data[1]

        case 5:
            potG = data[1]

        case 6:
            potB = data[1]

# Função para atualizar a cor do fundo
def update_color():
    r = valueR.get()
    g = valueG.get()
    b = valueB.get()
    color = f"#{r:02x}{g:02x}{b:02x}"  # Converter RGB para o formato Hex
    canvas.config(bg=color)  # Atualizar o fundo do canvas com a cor selecionada
    # Atualizar os rótulos para mostrar os valores atuais de RGB
    red_label.config(text=f"Red: {r}")
    green_label.config(text=f"Green: {g}")
    blue_label.config(text=f"Blue: {b}")

# Função para alternar o valor do toggle e atualizar os valores de value (RGB)
def toggleS():
    toggleSvar.set(1 - toggleSvar.get())  # Inverter o valor binário de toggleB
    value = toggleSvar.get()
    bus.write_i2c_block_data(addr, 0x00, [0,value])

def toggle_red():
    toggleR.set(1 - toggleR.get())  # Inverter o valor binário de toggleR
    update_value()
    value = toggleR.get()
    bus.write_i2c_block_data(addr, 0x00, [1,value])

def toggle_green():
    toggleG.set(1 - toggleG.get())  # Inverter o valor binário de toggleG
    update_value()
    value = toggleG.get()
    bus.write_i2c_block_data(addr, 0x00, [2,value])

def toggle_blue():
    toggleB.set(1 - toggleB.get())  # Inverter o valor binário de toggleB
    update_value()
    value = toggleB.get()
    bus.write_i2c_block_data(addr, 0x00, [3,value])

# Função para atualizar os valores das cores (Red, Green, Blue)
def update_value():
    # Se toggleR for 1, usa sliderR; caso contrário, usa potR
    if toggleR.get() == 1:
        valueR.set(sliderR.get())
        value = sliderR.get()
        bus.write_i2c_block_data(addr, 0x00, [4, value])
    else:
        valueR.set(potR)

    # Se toggleG for 1, usa sliderG; caso contrário, usa potG
    if toggleG.get() == 1:
        valueG.set(sliderG.get())
        value = sliderG.get()
        bus.write_i2c_block_data(addr, 0x00, [5,value])
    else:
        valueG.set(potG)

    # Se toggleB for 1, usa sliderB; caso contrário, usa potB
    if toggleB.get() == 1:
        valueB.set(sliderB.get())
        value = sliderB.get()
        bus.write_i2c_block_data(addr, 0x00, [6,value])
    else:
        valueB.set(potB)

    update_color()

# Criar a janela principal
root = tk.Tk()
root.title("RGB Controller")

# Variáveis para armazenar os valores das cores
valueR = tk.IntVar(value=0)
valueG = tk.IntVar(value=0)
valueB = tk.IntVar(value=0)

# Variáveis de toggle (0 ou 1)
toggleSvar = tk.IntVar(value=0)
toggleR = tk.IntVar(value=0)
toggleG = tk.IntVar(value=0)
toggleB = tk.IntVar(value=0)

# Variáveis de slider para as cores Red, Green e Blue
sliderR = tk.Scale(root, from_=0, to=255, orient="horizontal", command=lambda _: update_value())
sliderG = tk.Scale(root, from_=0, to=255, orient="horizontal", command=lambda _: update_value())
sliderB = tk.Scale(root, from_=0, to=255, orient="horizontal", command=lambda _: update_value())

# Variáveis de potenciômetro (valores fixos para simulação)
potR = 0  # Valor fixo do potenciômetro para Red
potG = 0  # Valor fixo do potenciômetro para Green
potB = 0  # Valor fixo do potenciômetro para Blue

# Criar os rótulos e sliders para as cores Red, Green e Blue
red_label = tk.Label(root, text="Red: 0")
red_label.pack()
sliderR.pack()

green_label = tk.Label(root, text="Green: 0")
green_label.pack()
sliderG.pack()

blue_label = tk.Label(root, text="Blue: 0")
blue_label.pack()
sliderB.pack()

# Criar os botões para alternar entre o slider e o potenciômetro para cada cor e o botão send
send_button = tk.Button(root, text="Send to Sheets", command=toggleS)
send_button.pack()

red_button = tk.Button(root, text="Toggle Red", command=toggle_red)
red_button.pack()

green_button = tk.Button(root, text="Toggle Green", command=toggle_green)
green_button.pack()

blue_button = tk.Button(root, text="Toggle Blue", command=toggle_blue)
blue_button.pack()

# Criar o canvas para mostrar a cor resultante
canvas = tk.Canvas(root, width=200, height=200, bg="#000000")
canvas.pack()

# Iniciar o loop principal do Tkinter
root.mainloop()
