import asyncio # Lets BLE transactions happen without blocking GUI updates
import threading # Background thread runs the asyncio forever loop and prevents GUI from freezing when reading bluetooth
from tkinter import ttk # Modern looking GUI
import tkinter as tk # shorthand makes code more concise
from bleak import BleakScanner, BleakClient

# Globals
HM10_DATA_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb" # UUID for HM-10's serial data channel

client = None # Holds the connection handle to HM-10
connected = False # Bool bluetooth connection status (for code readability)
loop = None 
actual_rpm_display = None # Actual RPM value
target_rpm_display = None # Target RPM value
current_display = None # Current mA value
status_display = None # Motor status
connection_display = None # Connection status

# Starts the asyncio loop in a background thread and assigns it to the global variable loop so it can be used by other functions
def start_async_loop():
    global loop 
    loop = asyncio.new_event_loop() # loop manages all the asynchronous tasks and events
    asyncio.set_event_loop(loop) # Sets loop global as active event loop
    loop.run_forever()

# Coroutine that pauses to scan for bluetooth devices and connect to the HM-10 if found
async def scan_and_connect():
    global client
    global connected

    connection_display.set("Scanning")
    devices = await BleakScanner.discover(timeout=5.0) # Scans for bluetooth devices for 5 seconds while allowing GUI operation asynchronously

    # Looks for devices with DSD in the name which is the brand that makes this build's HM-10
    hm10 = None
    for device in devices:
        if device.name is not None: # Skips nameless devices to avoid an error when checking for DSD in the name
            if "DSD" in device.name:
                hm10 = device
                break

    if hm10 is None:
        connection_display.set("HM-10 not found")
        return

    try: # Run function to check for error
        client = BleakClient(hm10.address) # Creates a handle for communication to HM-10
        await client.connect()
        connected = True
        connection_display.set(f"Connected: {hm10.name}") 
        await client.start_notify(HM10_DATA_UUID, ble_notification_handler) # Tells HM-10 to start sending data and calls ble_notification_handler when data arrives on the HM10_DATA_UUID channel

    except Exception as connection_error: # Only detects errors that occur while the try block is executed
        connected = False
        connection_display.set(f"Error: {connection_error}")
        print("Connection Error: ", connection_error)

# Called by bleak each time the HM-10 sends data (callback)
# Splits the incoming string into its values and updates the GUI displays
def ble_notification_handler(sender, data):
    try:
        # Translates binary from the HM-10 into text and skips error bytes
        # Removes newline character created by c
        text = data.decode('utf-8', errors="ignore").strip()
        text = text.split(',') # Splits at commas to separate data (RPM, current, status)

        # Empty dictionary to hold the motor data values
        motor_data = {}

        for part in text:
            section = part.split(':') # Splits on colons to separate names from values (actualRPM: 1000, current: 500, etc.)
            motor_data[section[0]] = section[1] # Assigns each value [1] to its name [0] in the motor_data dictionary

        # Gives the value assigned with the specified string in the dictionary, or 0 if the string is not found
        actual_rpm = motor_data.get('actualRPM', 0)
        target_rpm = motor_data.get('targetRPM', 0)
        current = motor_data.get('current', 0)
        system_status = motor_data.get('systemStatus', 0)

        # Updates the GUI (the StringVar boxes) with the values
        actual_rpm_display.set(f"Actual RPM: {actual_rpm}")
        target_rpm_display.set(f"Target RPM: {target_rpm}")
        current_display.set(f"Current: {current} mA")
        status_display.set(f"Status: {system_status}")

    except Exception as display_error:
        print("Display Update Error: ", display_error)

# Coroutine that sends a command to the HM-10 without blocking the GUI
async def send_command(command):
    if client and connected: # Skip if not connected
        # Sends bytes to the HM-10 which sends them to the STM32 via UART.
        # Newline character at the end of each command so HAL_UART_RxCpltCallback knows when to stop reading the command
        await client.write_gatt_char(HM10_DATA_UUID, (command + "\n").encode())

# Sends "START" via bluetooth when the function is called. This job is put on the background loop, transferring the command from the GUI thread to the bluetooth thread
def send_start():
    asyncio.run_coroutine_threadsafe(send_command("START"), loop)

# Same as send_start but sends "STOP" instead
def send_stop():
    asyncio.run_coroutine_threadsafe(send_command("STOP"), loop)

# Same as send_start but sends "RPM:<value>" instead
def send_rpm(value):
    asyncio.run_coroutine_threadsafe(send_command(f"RPM:{value}"), loop)

# GUI Section. Builds boxes and assigns them labels and values
def build_gui():
    root = tk.Tk() # Creates the main window
    root.title("Motor Control GUI") # Sets the title of the window
    root.geometry("400x300") # Small window size because the GUI is simple and only needs to display a few values

    global actual_rpm_display
    global target_rpm_display
    global current_display
    global status_display
    global connection_display

    actual_rpm_display = tk.StringVar(value="Actual RPM: 0") # Creates box to hold the actual RPM value for display
    tk.Label(root, textvariable=actual_rpm_display, font=("Arial", 14)).pack(pady=10) # Displays the actual RPM value in the GUI

    target_rpm_display = tk.StringVar(value="Target RPM: 0")
    tk.Label(root, textvariable=target_rpm_display, font=("Arial", 14)).pack(pady=10)

    current_display = tk.StringVar(value="Current: 0 mA")
    tk.Label(root, textvariable=current_display, font=("Arial", 14)).pack(pady=10)

    status_display = tk.StringVar(value="Status: 0")
    tk.Label(root, textvariable=status_display, font=("Arial", 14)).pack(pady=10)

    connection_display = tk.StringVar(value="Not Connected")
    tk.Label(root, textvariable=connection_display, font=("Arial", 14)).pack(pady=10)

    # RPM Slider
    def slide_display(slide_value):
        slider_display.set(f"RPM: {int(float(slide_value))}") # Updates the target RPM display as the slider is moved

    slider_display = tk.StringVar(value="RPM: 0")
    tk.Label(root, textvariable=slider_display, font=("Arial", 14)).pack(pady=10) # Displays the target RPM value while the slider is being moved

    rpm_slider = ttk.Scale(root, from_=0, to=1000, orient='horizontal', command=slide_display) # Slider for target RPM
    rpm_slider.pack(pady=2)

    # Sets RPM according to the slider value
    def set_rpm():
        send_rpm(int(rpm_slider.get())) # Sends the value on the slider to the motor controller

    tk.Button(root, text="Set RPM", command=set_rpm).pack(pady=10) # Calls set_rpm when button is pressed, confirming the user's selection on the slider

    # Connect Button
    def on_connect(): # Function runs when connect button is pressed
        asyncio.run_coroutine_threadsafe(scan_and_connect(), loop) # Runs scan_and_connect on the background thread 

    tk.Button(root, text="Connect", command=on_connect).pack(pady=10) # Calls on_connect when button is pressed

    # Start and Stop Buttons
    tk.Button(root, text="START", command=send_start).pack(pady=10)
    tk.Button(root, text="STOP", command=send_stop).pack(pady=10)

    return root

if __name__ == "__main__":
    async_thread = threading.Thread(target=start_async_loop, daemon=True) # Creates a new thread to run the asyncio loop in the background so the GUI doesn't freeze when reading bluetooth
    async_thread.start() # Starts the thread that runs the asyncio loop in the background

    root = build_gui() # Makes window
    root.mainloop() # Detects clicks and interactions with the GUI and updates the window accordingly