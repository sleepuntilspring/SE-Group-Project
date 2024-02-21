import tkinter as tk
from tkinter import filedialog
from tkinter import Scrollbar
from tkinter import PanedWindow
from tkinter import ttk  # Import ttk module for Separator
import os

from tkinter.filedialog import askopenfilename, asksaveasfilename


def open_file():
    filepath = askopenfilename(
        filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")]
    )
    if not filepath:
        return
    user_input.delete("1.0", tk.END)
    with open(filepath, mode="r", encoding="utf-8") as input_file:
        code = input_file.read()
        user_input.insert(tk.END, code)
    window.title(f"AMIR - {filepath}")

def save_file():
    filepath = asksaveasfilename(
        defaultextension=".txt",
        filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")],
    )
    if not filepath:
        return
    with open(filepath, mode="w", encoding="utf-8") as output_file:
        code = user_input.get("1.0", tk.END)
        output_file.write(code)
    window.title(f"AMIR - {filepath}")

def run_translation():
    # Save current input to 'example.txt'
    with open('example.txt', 'w', encoding='utf-8') as file:
        input_text = user_input.get("1.0", tk.END)
        file.write(input_text)

    # Enclose the executable path in double quotes
    c_program_path = '"C:\\Study\\SEGP\\amir gui 3\\operation.exe"'

    # Call C backend to process 'example.txt'
    os.system(c_program_path)

    # Check if the output file exists before trying to open it
    if os.path.exists('output.txt'):
        with open('output.txt', 'r', encoding='utf-8') as file:
            output_text = file.read()
            output_widget.config(state=tk.NORMAL)
            output_widget.delete("1.0", tk.END)
            output_widget.insert(tk.END, output_text)
            output_widget.config(state=tk.DISABLED)
    else:
        print("Error: 'output.txt' not found. Check if the C program ran correctly.")

current_line_index = 0  # Global variable to keep track of the current line

def run_by_line():
    global current_line_index
    current_line_index += 1  # Process the next line
    
    input_text = user_input.get("1.0", tk.END)  # Get text from the input widget
    input_lines = input_text.split('\n')[:current_line_index]  # Get up to current line
    
    # Write the currently processed lines to a temporary file
    with open('temp_input.txt', 'w') as temp_input_file:
        temp_input_file.write("\n".join(input_lines))
    
    # Call your C program with the line count
    c_program_path = '"C:\\Study\\SEGP\\amir gui 3\\stepBystep.exe"'
    input_file_path = 'example.txt'
    output_file_path = 'output.txt'
    # Ensure the entire command is constructed properly, especially with paths that contain spaces
    command = f'"{c_program_path}" "{input_file_path}" "{output_file_path}" {current_line_index}'

    # Use the constructed command in os.system()
    os.system(command)
    
    # Read and display the output
    if os.path.exists(output_file_path):
        with open(output_file_path, 'r') as output_file:
            output_text = output_file.read()
            output_widget.config(state=tk.NORMAL)
            output_widget.delete("1.0", tk.END)
            output_widget.insert(tk.END, output_text)
            output_widget.config(state=tk.DISABLED)


def toggle_theme():
    global current_theme
    current_theme = "dark" if current_theme == "light" else "light"
    theme = themes[current_theme]

    # Update widget colors
    user_input.config(bg=theme["bg"], fg=theme["fg"], insertbackground=theme["fg"])
    output_widget.config(bg=theme["bg"], fg=theme["fg"], insertbackground=theme["fg"])
    new_text1.config(bg=theme["bg"], fg=theme["fg"], insertbackground=theme["fg"])
    new_text2.config(bg=theme["bg"], fg=theme["fg"], insertbackground=theme["fg"])
    
    dark_mode_menu_index = menu.index(tk.END)  # Get the index of the last item
    new_label = "Light Mode" if current_theme == "dark" else "Dark Mode"

    # Update the 'Dark Mode' menu item label based on the current theme
    menu.entryconfig(dark_mode_menu_index, label=new_label)

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

def show_about():
    pass

window = tk.Tk()
window.title("AMIR")

top_frame = tk.Frame(window)
top_frame.pack(side=tk.TOP, fill=tk.X)

# Create a Menu
menu = tk.Menu(window)
window.config(menu=menu)

# Add menu items
file_menu = tk.Menu(menu, tearoff=0)
menu.add_cascade(label="File", menu=file_menu)
file_menu.add_command(label="Open", command=open_file)
file_menu.add_command(label="Save As...", command=save_file)

# Add a 'Run' button and a 'Toggle Theme' button directly to the main menu bar
menu.add_command(label="Run All", command=run_translation)
menu.add_command(label="Run By Line", command=run_by_line)
menu.add_command(label="Dark Mode", command=toggle_theme)

paned_window = tk.PanedWindow(window, orient=tk.HORIZONTAL)
paned_window.pack(expand=True, fill=tk.BOTH)

left_frame = tk.Frame(paned_window)
paned_window.add(left_frame)

output_widget = tk.Text(left_frame, wrap="word", width=60, height=20)
output_widget.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

output_scrollbar = Scrollbar(left_frame, command=output_widget.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
output_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
output_widget.config(yscrollcommand=output_scrollbar.set)

separator = ttk.Separator(paned_window, orient="vertical")
paned_window.add(separator)

right_frame = tk.Frame(paned_window)
paned_window.add(right_frame)

user_input = tk.Text(right_frame, wrap="word", height=20, width=60)
user_input.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

input_scrollbar = Scrollbar(right_frame, command=user_input.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
input_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
user_input.config(yscrollcommand=output_scrollbar.set)

bottom_frame = tk.Frame(window)
bottom_frame.pack(side=tk.BOTTOM, fill=tk.X)

new_text1 = tk.Text(bottom_frame, wrap="word", height=20, width=60)
new_text1.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

new_scrollbar1 = Scrollbar(bottom_frame, command=new_text1.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
new_scrollbar1.pack(side=tk.RIGHT, fill=tk.Y)
new_text1.config(yscrollcommand=new_scrollbar1.set)

new_text2 = tk.Text(bottom_frame, wrap="word", height=20, width=60)
new_text2.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

new_scrollbar2 = Scrollbar(bottom_frame, command=new_text2.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
new_scrollbar2.pack(side=tk.RIGHT, fill=tk.Y)
new_text2.config(yscrollcommand=new_scrollbar2.set)


window.mainloop()
