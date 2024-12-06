import tkinter as tk  # Tkinter for the GUI
from smbus2 import SMBus  # SMBus for I2C communication

# I2C address of the ESP32 (must match the ESP32's configured address)
ESP32_I2C_ADDRESS = 0x08

# Initialize states for the 4 boolean variables
states = [False, False, False, False]

class StateSyncApp:
    def __init__(self, root):
        """
        Initialize the application, set up the I2C bus, and create the GUI.
        """
        self.bus = SMBus(1)  # Initialize I2C communication on bus 1 (default for Raspberry Pi)
        self.create_widgets(root)  # Create the GUI components
        self.listen_for_updates()  # Start listening for updates from the ESP32

    def create_widgets(self, root):
        """
        Create toggle buttons for controlling the 4 boolean variables.
        """
        self.buttons = []  # List to hold the toggle buttons
        for i in range(4):
            # Create a button for each state, with an initial label of "OFF"
            button = tk.Button(
                root, 
                text=f"State {i+1}: OFF",  # Button text indicates the state
                width=15,  # Set button width
                command=lambda idx=i: self.toggle_state(idx)  # Call toggle_state when clicked
            )
            # Place the button in the GUI
            button.grid(row=i, column=0, padx=10, pady=5)
            self.buttons.append(button)  # Add button to the list

    def toggle_state(self, index):
        """
        Toggle the value of a specific boolean variable and update the GUI.
        """
        states[index] = not states[index]  # Toggle the state (True <-> False)
        # Update the button text to reflect the new state
        self.buttons[index].config(text=f"State {index+1}: {'ON' if states[index] else 'OFF'}")
        # Send the updated states to the ESP32
        self.send_states()

    def send_states(self):
        """
        Send the current states of all variables to the ESP32 over I2C.
        """
        try:
            # Convert boolean states to integers (1 for True, 0 for False)
            data = [int(state) for state in states]
            # Send the states to the ESP32 as a block of data
            self.bus.write_i2c_block_data(ESP32_I2C_ADDRESS, 0, data)
            print(f"Sent states: {data}")  # Print sent states for debugging
        except Exception as e:
            # Print an error message if something goes wrong
            print(f"Error sending states: {e}")

    def listen_for_updates(self):
        """
        Continuously check for updates from the ESP32 and update the GUI.
        """
        try:
            # Attempt to read 4 bytes of data from the ESP32
            data = self.bus.read_i2c_block_data(ESP32_I2C_ADDRESS, 0, 4)
            # Update the local states and button labels based on the received data
            for i, state in enumerate(data):
                states[i] = bool(state)  # Convert the received integer (1/0) to boolean
                # Update the button text to match the new state
                self.buttons[i].config(text=f"State {i+1}: {'ON' if states[i] else 'OFF'}")
            print(f"Received states: {data}")  # Print received states for debugging
        except Exception as e:
            # Print an error message if reading from the ESP32 fails
            print(f"Error receiving states: {e}")
        # Schedule the next check for updates after 500 milliseconds
        root.after(500, self.listen_for_updates)

# Create the main Tkinter window
root = tk.Tk()
root.title("State Sync App")  # Set the window title

# Create an instance of the StateSyncApp class
app = StateSyncApp(root)

# Start the Tkinter main loop to display the GUI and handle user interactions
root.mainloop()

