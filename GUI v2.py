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
    c_program_path = '"C:\\Study\\SEGP\\amir gui 4\\segp_operation.exe"'

    # Call C backend to process 'example.txt'
    os.system(c_program_path)

    # Check if the output file exists before trying to open it
    if os.path.exists('registers_output.txt'):
        with open('registers_output.txt', 'r', encoding='utf-8') as file:
            r_output_text = file.read()
            output_widget.config(state=tk.NORMAL)
            output_widget.delete("1.0", tk.END)
            output_widget.insert(tk.END, r_output_text)
            output_widget.config(state=tk.DISABLED)
    else:
        print("Error: 'registers_output.txt' not found. Check if the C program ran correctly.")
    
    if os.path.exists('memory_output.txt'):
        with open('memory_output.txt', 'r', encoding='utf-8') as file:
            m_output_text = file.read()
            new_text1.config(state=tk.NORMAL)
            new_text1.delete("1.0", tk.END)
            new_text1.insert(tk.END, m_output_text)
            new_text1.config(state=tk.DISABLED)
    else:
        print("Error: 'memory_output.txt' not found. Check if the C program ran correctly.")

    if os.path.exists('io_output.txt'):
        with open('io_output.txt', 'r', encoding='utf-8') as file:
            i_output_text = file.read()
            new_text2.config(state=tk.NORMAL)
            new_text2.delete("1.0", tk.END)
            new_text2.insert(tk.END, i_output_text)
            new_text2.config(state=tk.DISABLED)
    else:
        print("Error: 'io_output.txt' not found. Check if the C program ran correctly.")

"""
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
"""

def show_about():
    pass

window = tk.Tk()
window.title("AMIR")

top_frame = tk.Frame(window)
top_frame.pack(side=tk.TOP, fill=tk.X)

button_frame = tk.Frame(top_frame)
button_frame.pack(side=tk.LEFT, padx=5)

buttons = [
    tk.Button(button_frame, text="Open", command=open_file),
    tk.Button(button_frame, text="Save As", command=save_file),
    tk.Button(button_frame, text="Run", command=run_translation),
    tk.Button(button_frame, text="About", command=show_about),
]

for btn in buttons:
    btn.pack(side=tk.LEFT)


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

# Create frames for each text widget and scrollbar pair
bottom_left_frame = tk.Frame(bottom_frame)
bottom_left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

bottom_right_frame = tk.Frame(bottom_frame)
bottom_right_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

# Bottom left text widget and scrollbar
new_text1 = tk.Text(bottom_left_frame, wrap="word", height=20, width=60)
new_text1.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

new_scrollbar1 = Scrollbar(bottom_left_frame, command=new_text1.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
new_scrollbar1.pack(side=tk.RIGHT, fill=tk.Y)
new_text1.config(yscrollcommand=new_scrollbar1.set)

# Bottom right text widget and scrollbar
new_text2 = tk.Text(bottom_right_frame, wrap="word", height=20, width=60)
new_text2.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

new_scrollbar2 = Scrollbar(bottom_right_frame, command=new_text2.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
new_scrollbar2.pack(side=tk.RIGHT, fill=tk.Y)
new_text2.config(yscrollcommand=new_scrollbar2.set)


window.mainloop()
