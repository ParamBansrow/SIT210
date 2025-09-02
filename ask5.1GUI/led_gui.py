import tkinter as tk
import RPi.GPIO as GPIO

#  GPIO Setup 
GPIO.setmode(GPIO.BCM)
led_pins = {"Red": 17, "Green": 27, "Blue": 22}
for pin in led_pins.values():
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)

# Functions 
def turn_on_led(color):
    for c, pin in led_pins.items():
        GPIO.output(pin, GPIO.HIGH if c == color else GPIO.LOW)

def exit_program():
    GPIO.cleanup()
    root.destroy()

#  Tkinter GUI 
root = tk.Tk()
root.title("LED Control Panel")
root.geometry("300x200")

selected_color = tk.StringVar(value="")

# Radio buttons
for color in led_pins.keys():
    tk.Radiobutton(
        root,
        text=color,
        variable=selected_color,
        value=color,
        command=lambda: turn_on_led(selected_color.get())
    ).pack(anchor="w", pady=5)

# Exit button
tk.Button(root, text="Exit", command=exit_program, bg="red", fg="white").pack(pady=10)

# Start GUI loop
root.mainloop()
