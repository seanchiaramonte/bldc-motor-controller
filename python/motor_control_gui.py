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

# Coroutine that pauses to scan for bluetooth devices and connect to the H-10 if found
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