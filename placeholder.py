import tkinter as tk
import os
import time
from tkinter import filedialog

from tkinter.filedialog import askopenfilename, asksaveasfilename

def open_file():
    filepath = askopenfilename(
        filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")]
    )
    if not filepath:
        return
    text_right.delete("1.0", tk.END)
    with open(filepath, mode="r", encoding="utf-8") as input_file:
        code = input_file.read()
        text_right.insert(tk.END, code)
    window.title(f"AMIR - {filepath}")

def save_file():
    filepath = asksaveasfilename(
        defaultextension=".txt",
        filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")],
    )
    if not filepath:
        return
    with open(filepath, mode="w", encoding="utf-8") as output_file:
        code = text_right.get("1.0", tk.END)
        output_file.write(code)
    window.title(f"AMIR - {filepath}")

def run_translation():
    # Save current input to 'example.txt'
    with open('example.txt', 'w', encoding='utf-8') as file:
        input_text = text_right.get("1.0", tk.END)
        file.write(input_text)

    # Enclose the executable path in double quotes
    c_program_path = '"C:\\Study\\SEGP\\amir gui 3\\operation.exe"'

    # Call C backend to process 'example.txt'
    os.system(c_program_path)

    # Optionally add a delay
    #time.sleep(1)  # Wait for 1 second (adjust as needed)

    # Check if the output file exists before trying to open it
    if os.path.exists('output.txt'):
        with open('output.txt', 'r', encoding='utf-8') as file:
            output_text = file.read()
            text_left.delete("1.0", tk.END)
            text_left.insert(tk.END, output_text)
    else:
        print("Error: 'output.txt' not found. Check if the C program ran correctly.")

def show_about():
    pass

def toggle_theme():
    global current_theme
    current_theme = "dark" if current_theme == "light" else "light"
    theme = themes[current_theme]
    
    # Update widget colors
    text_left.config(bg=theme["bg"], fg=theme["fg"], insertbackground=theme["fg"])
    text_right.config(bg=theme["bg"], fg=theme["fg"], insertbackground=theme["fg"])
    top_frame.config(bg=theme["frame_bg"])
    button_frame.config(bg=theme["frame_bg"])
    for button in buttons:
        button.config(bg=theme["button_bg"], fg=theme["button_fg"])
    
    # Update the button text based on the current theme
    dark_mode_btn.config(text="Light Mode" if current_theme == "dark" else "Dark Mode")

# Define themes
themes = {
    "light": {
        "bg": "white", "fg": "black", "button_bg": "SystemButtonFace", "button_fg": "black", "frame_bg": "SystemButtonFace"
    },
    "dark": {
        "bg": "#363636", "fg": "white", "button_bg": "#505050", "button_fg": "white", "frame_bg": "#404040"
    }
}
current_theme = "light"  # Start with the light theme


# Create the main window
window = tk.Tk()
window.title("AMIR")

# Create a Menu
menu = tk.Menu(window)
window.config(menu=menu)

# Get the screen dimension
screen_width = window.winfo_screenwidth()
screen_height = window.winfo_screenheight()

# Calculate the window size as a fraction of the screen size
# Here, we are using 80% of the screen size
window_width = int(screen_width * 0.8)
window_height = int(screen_height * 0.8)

# Calculate the position to center the window on the screen
x_position = int((screen_width - window_width) / 2)
y_position = int((screen_height - window_height) / 2)

# Set the window size and position
window.geometry(f'{window_width}x{window_height}+{x_position}+{y_position}')

top_frame = tk.Frame(window)
top_frame.pack(side=tk.TOP, fill=tk.X)

button_frame = tk.Frame(top_frame)
button_frame.pack(side=tk.LEFT, padx=5)

# Add a button for toggling dark/light mode
dark_mode_btn = tk.Button(button_frame, text="Dark Mode", command=toggle_theme)

buttons = [
    tk.Button(button_frame, text="Open", command=open_file),
    tk.Button(button_frame, text="Save As", command=save_file),
    tk.Button(button_frame, text="Run All", command=run_translation),
    tk.Button(button_frame, text="Run By Line", command=run_translation),
    tk.Button(button_frame, text="About", command=show_about),
    dark_mode_btn  # Add the toggle theme button to the list
]

for btn in buttons:
    btn.pack(side=tk.LEFT)


# Create the paned window widget
paned_window = tk.PanedWindow(window, orient=tk.HORIZONTAL)
paned_window.pack(fill=tk.BOTH, expand=True)

# Create the left text widget
text_left = tk.Text(paned_window, wrap="none")
paned_window.add(text_left, width=480)

# Create the right text widget
text_right = tk.Text(paned_window, wrap="none")
paned_window.add(text_right, width=200)

# Start the GUI event loop
window.mainloop()