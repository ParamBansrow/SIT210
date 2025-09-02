import RPi.GPIO as GPIO
import tkinter as tk

# GPIO setup
GPIO.setmode(GPIO.BCM)
led_pins = [18, 23, 24]
pwm_leds = []

# Setup pins and start PWM
for pin in led_pins:
    GPIO.setup(pin, GPIO.OUT)
    pwm = GPIO.PWM(pin, 100)  # 100Hz
    pwm.start(0)
    pwm_leds.append(pwm)

# Function to update PWM duty cycle
def update_brightness(value, led_index):
    duty_cycle = int(value)
    pwm_leds[led_index].ChangeDutyCycle(duty_cycle)

# Create GUI
root = tk.Tk()
root.title("LED Intensity Controller")

# Create 3 sliders
for i in range(3):
    slider = tk.Scale(root,
                      from_=0,
                      to=100,
                      orient="horizontal",
                      label=f"LED {i+1} Brightness",
                      command=lambda val, idx=i: update_brightness(val, idx))
    slider.pack()

# Safe exit
def on_closing():
    for pwm in pwm_leds:
        pwm.stop()
    GPIO.cleanup()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)
root.mainloop()
