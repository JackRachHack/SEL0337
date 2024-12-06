import tkinter as tk  # Tkinter for GUI
from smbus2 import SMBus  # SMBus for I2C communication
import logging  # Logging for debugging and status reporting

# Configure logging to display messages in the console
logging.basicConfig(level=logging.INFO)

# I2C address of the ESP32 (must match the address set on the ESP32)
ESP32_I2C_ADDRESS = 0x08

class RGBControllerApp:
    def __init__(self, root):
        """
        Initialize the application with a Tkinter root window and set up the I2C bus.
        """
        self.bus = SMBus(1)  # Use I2C bus 1 on Raspberry Pi
        self.create_widgets(root)  # Create the GUI elements

    def create_widgets(self, root):
        """
        Create the sliders and buttons for controlling RGB values.
        """
        # Create a slider for the red color
        self.red_slider = self.add_slider(root, "Red", 0)
        # Create a slider for the green color
        self.green_slider = self.add_slider(root, "Green", 1)
        # Create a slider for the blue color
        self.blue_slider = self.add_slider(root, "Blue", 2)

        # Create a button to send the RGB values via I2C
        send_button = tk.Button(root, text="Send", command=self.send_rgb)
        send_button.grid(row=3, column=0, columnspan=2, pady=10)

        # Create a label to display the status of the operation (success/error)
        self.status_label = tk.Label(root, text="")
        self.status_label.grid(row=4, column=0, columnspan=2)

    def add_slider(self, root, label, row):
        """
        Add a slider to the GUI for controlling a specific RGB component.
        - label: The label for the slider (e.g., "Red").
        - row: The row in the grid layout where the slider should appear.
        """
        # Create and place the label for the slider
        tk.Label(root, text=label).grid(row=row, column=0, padx=10, pady=5)
        # Create the slider with values ranging from 0 to 255
        slider = tk.Scale(root, from_=0, to=255, orient=tk.HORIZONTAL)
        slider.grid(row=row, column=1, padx=10, pady=5)
        return slider

    def send_rgb(self):
        """
        Read the RGB values from the sliders and send them via I2C to the ESP32.
        """
        # Read the current values from the sliders
        rgb_values = [self.red_slider.get(), self.green_slider.get(), self.blue_slider.get()]
        try:
            # Send the RGB values as a block of data to the ESP32
            self.bus.write_i2c_block_data(ESP32_I2C_ADDRESS, 0, rgb_values)
            # Update the status label to indicate success
            self.status_label.config(text="Sent successfully!", fg="green")
            # Log the sent values for debugging
            logging.info(f"RGB Sent: {rgb_values}")
        except Exception as e:
            # If an error occurs, display it in the status label
            self.status_label.config(text=f"Error: {e}", fg="red")
            # Log the error for debugging
            logging.error(f"Failed to send RGB: {e}")

# Create the main Tkinter window
root = tk.Tk()
root.title("RGB LED Controller")  # Set the window title

# Create an instance of the RGBControllerApp class
app = RGBControllerApp(root)

# Start the Tkinter main loop to display the GUI
root.mainloop()
