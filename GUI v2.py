import tkinter as tk
from tkinter import filedialog, Scrollbar, PanedWindow, ttk
import subprocess
import os
import re

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
    change_text_color() 
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

def change_text_color(event=None):
    # Get text from the input widget
    input_text = user_input.get("1.0", tk.END)
    
    # Clear existing tags
    user_input.tag_remove("blue", "1.0", tk.END)
    user_input.tag_remove("green", "1.0", tk.END)
    user_input.tag_remove("grey", "1.0", tk.END)
    
    # Apply blue color to the word "m" and "mil"
    for word in ["m", "mil","mih", "mi", "and", "or", "xor", "sub", "sl", "sr", "not", "add","nop", "push", "pop", "call", "j"]:
        start_index = "1.0"
        while True:
            start_index = user_input.search(word, start_index, stopindex=tk.END)
            if not start_index:
                break
            end_index = f"{start_index}+{len(word)}c"
            user_input.tag_add("blue", start_index, end_index)
            user_input.tag_config("blue", foreground="blue")
            start_index = end_index            
    
    start_index = "1.0"
    while True:
        start_index = user_input.search(r'--.*', start_index, stopindex=tk.END, regexp=True)
        if not start_index:
            break

        # Find the end of the line
        end_index = user_input.search("\n", start_index, stopindex=tk.END)
        if not end_index:
            end_index = tk.END

        user_input.tag_add("grey", start_index, end_index)
        user_input.tag_config("grey", foreground="grey")
        start_index = end_index
    
    start_index = "1.0"
    while True:
        start_index = user_input.search("r", start_index, stopindex=tk.END)
        if not start_index:
            break

        # Find the next space, '=', or ',' symbol after the 'r' symbol
        end_index = start_index

        while True:
            char = user_input.get(end_index, f"{end_index}+1c")
            if char.isspace() or char in "=,":
                break
            end_index = f"{end_index}+1c"

        user_input.tag_add("green", start_index, end_index)
        user_input.tag_config("green", foreground="green")
        start_index = end_index

    start_index = "1.0"
    while True:
        start_index = user_input.search("#", start_index, stopindex=tk.END, regexp=True)
        if not start_index:
            break

        # Find the start of the line
        line_start = user_input.index(f"{start_index} linestart")

        # Find the end of the line
        line_end = user_input.index(f"{start_index} lineend")

        # Check if '--' is present on the entire line
        if not user_input.search(r'--', line_start, line_end, regexp=True):
            # Find the next space after the '#' symbol
            space_index = user_input.search(" ", start_index, stopindex=tk.END)

            # Find the next word after the '#' symbol
            word_match = re.search(r'\S+', user_input.get(start_index, space_index))

            if word_match:
                end_index = user_input.index(start_index + f"+{len(word_match.group())}c")
            else:
                end_index = user_input.index(tk.END)

            user_input.tag_add("red", start_index, end_index)
            user_input.tag_config("red", foreground="red")

        start_index = end_index


    start_index = "1.0"
    while True:
        start_index = user_input.search("@", start_index, stopindex=tk.END, regexp=True)
        if not start_index:
            break

        # Find the start of the line
        line_start = user_input.index(f"{start_index} linestart")

        # Find the end of the line
        line_end = user_input.index(f"{start_index} lineend")

        # Check if '--' is present on the entire line
        if not user_input.search(r'--', line_start, line_end, regexp=True):
            # Find the next space after the '#' symbol
            space_index = user_input.search(" ", start_index, stopindex=tk.END)

            # Find the next word after the '#' symbol
            word_match = re.search(r'\S+', user_input.get(start_index, space_index))

            if word_match:
                end_index = user_input.index(start_index + f"+{len(word_match.group())}c")
            else:
                end_index = user_input.index(tk.END)

            user_input.tag_add("red", start_index, end_index)
            user_input.tag_config("red", foreground="red")

        start_index = end_index

def run_translation():

    # Enclose the executable path in double quotes
    c_program_path = '"C:\\Study\\SEGP\\amir gui 4\\segp_operation.exe"'

    # Call C backend to process 'example.txt'
    os.system(c_program_path)

    # Check if the output file exists before trying to open it
    if os.path.exists('registers_output.txt'):
        with open('registers_output.txt', 'r', encoding='utf-8') as file:
            r_output_text = file.read()

            # Clear existing entries in the tree
            for item in tree.get_children():
                tree.delete(item)

            # Insert new data into the tree
            data = parse_registers_output(r_output_text)
            for row in data:
                tree.insert("", "end", values=row)

    else:
        print("Error: 'registers_output.txt' not found. Check if the C program ran correctly.")
    
    if os.path.exists('memory_output.txt'):
        with open('memory_output.txt', 'r', encoding='utf-8') as file:
            m_output_text = file.read()

            for item in memory_tree.get_children():
                memory_tree.delete(item)
                
            data2 = parse_memory_output(m_output_text)
            for row in data2:
                memory_tree.insert("", "end", values=row)

    else:
        print("Error: 'memory_output.txt' not found. Check if the C program ran correctly.")

    if os.path.exists('io_output.txt'):
        with open('io_output.txt', 'r', encoding='utf-8') as file:
            io_output_text = file.read()

            for item in io_tree.get_children():
                io_tree.delete(item)
                
            data3 = parse_io_output(io_output_text)
            for row in data3:
                io_tree.insert("", "end", values=row)
    else:
        print("Error: 'io_output.txt' not found. Check if the C program ran correctly.")

        
def run_Program():
    # Save current input to 'example.txt'
    with open('temp_input2.txt', 'w', encoding='utf-8') as file:
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

            for item in tree.get_children():
                tree.delete(item)

            # Insert new data into the tree
            data = parse_registers_output(r_output_text)
            for row in data:
                tree.insert("", "end", values=row)
                
    else:
        print("Error: 'registers_output.txt' not found. Check if the C program ran correctly.")
    
    if os.path.exists('memory_output.txt'):
        with open('memory_output.txt', 'r', encoding='utf-8') as file:
            m_output_text = file.read()

            for item in memory_tree.get_children():
                memory_tree.delete(item)
                
            data2 = parse_memory_output(m_output_text)
            for row in data2:
                memory_tree.insert("", "end", values=row)

    else:
        print("Error: 'memory_output.txt' not found. Check if the C program ran correctly.")
    
    if os.path.exists('io_output.txt'):
        with open('io_output.txt', 'r', encoding='utf-8') as file:
            io_output_text = file.read()

            for item in io_tree.get_children():
                io_tree.delete(item)
                
            data3 = parse_io_output(io_output_text)
            for row in data3:
                io_tree.insert("", "end", values=row)
    else:
        print("Error: 'io_output.txt' not found. Check if the C program ran correctly.")


def parse_registers_output(output_text):
     # Split the text into rows
    rows = output_text.strip().split('\n')

    # Assuming each row has two values separated by whitespace (using regex)
    data = [re.split(r'\s+', row.strip()) for row in rows]

    return data   

def parse_memory_output(output_text):
     # Split the text into rows
    rows = output_text.strip().split('\n')

    # Assuming each row has two values separated by whitespace (using regex)
    data = [re.split(r'\s+', row.strip()) for row in rows]

    return data   

def parse_io_output(output_text):
     # Split the text into rows
    rows = output_text.strip().split('\n')

    # Assuming each row has two values separated by whitespace (using regex)
    data = [re.split(r'\s+', row.strip()) for row in rows]

    return data   

current_line = None
def step_in():
    global current_line

    # Remove any existing tags
    user_input.tag_remove("highlight", "1.0", tk.END)
    
    # If current_line is not set, start from the beginning
    if current_line is None:
        current_line = "1.0"
    else:
        # Move to the next line
        current_line = user_input.index(f"{current_line} + 1 lines linestart")
    
    # Highlight the current line
    user_input.tag_configure("highlight", background="yellow")
    user_input.tag_add("highlight", f"{current_line} linestart", f"{current_line} lineend")
    
    # Scroll the highlighted line into view
    user_input.see(current_line)

    # Save current input to 'example.txt'
    with open('temp_input2.txt', 'w', encoding='utf-8') as file:
        input_text = user_input.get("1.0", f"{current_line} lineend")
        file.write(input_text)
        
    run_translation()

def step_up():
    global current_line

    # Remove any existing tags
    user_input.tag_remove("highlight", "1.0", tk.END)
    
    # If current_line is not set, start from the beginning
    if current_line is None:
        current_line = "1.0"
    else:
        # Move to the next line
        current_line = user_input.index(f"{current_line} - 1 lines linestart")
    
    # Highlight the current line
    user_input.tag_configure("highlight", background="yellow")
    user_input.tag_add("highlight", f"{current_line} linestart", f"{current_line} lineend")
    
    # Scroll the highlighted line into view
    user_input.see(current_line)

    # Save current input to 'example.txt'
    with open('temp_input2.txt', 'w', encoding='utf-8') as file:
        input_text = user_input.get("1.0", f"{current_line} lineend")
        file.write(input_text)
        
    run_translation()

def step_out():
    global current_line
    # Remove any existing tags
    user_input.tag_remove("highlight", "1.0", tk.END)
    
    # Reset current_line
    current_line = None

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
    tk.Button(button_frame, text="Run", command=run_Program),
    tk.Button(button_frame, text="Step In", command=step_in),
    tk.Button(button_frame, text="Step Up", command=step_up),
    tk.Button(button_frame, text="Step Out", command=step_out),
    tk.Button(button_frame, text="About", command=show_about),
]

for btn in buttons:
    btn.pack(side=tk.LEFT)

paned_window = tk.PanedWindow(window, orient=tk.HORIZONTAL)
paned_window.pack(expand=True, fill=tk.BOTH)

# Left Frame for New Box (Table Display)
left_frame = tk.Frame(paned_window)
paned_window.add(left_frame)

# Create a Treeview widget for displaying data in a table
columns = ("Registers", "Content")  # Adjust the column names accordingly
tree = ttk.Treeview(left_frame, columns=columns, show="headings")


# Set column headings
for col in columns:
    tree.heading(col, text=col)
    tree.column(col, width=100) 

tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

# Scrollbar for the Treeview
tree_scrollbar = Scrollbar(left_frame, command=tree.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
tree_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
tree.config(yscrollcommand=tree_scrollbar.set)

separator = ttk.Separator(paned_window, orient="vertical")
paned_window.add(separator)

right_frame = tk.Frame(paned_window)
paned_window.add(right_frame)

def load_data(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        # Assuming your data is organized, parse it accordingly
        data = [line.strip().split('\t') for line in lines]
    return data

#User Input
user_input = tk.Text(right_frame, wrap="word", width=60, height=20)
user_input.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

input_scrollbar = Scrollbar(right_frame, command=user_input.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
input_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
user_input.config(yscrollcommand=input_scrollbar.set)

bottom_frame = tk.Frame(window)
bottom_frame.pack(side=tk.BOTTOM, fill=tk.X)

# Create frames for each text widget and scrollbar pair
bottom_left_frame = tk.Frame(bottom_frame)
bottom_left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

bottom_right_frame = tk.Frame(bottom_frame)
bottom_right_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

# Memory Window Treeview Setup
memory_columns = ("Address", "Content")  # Define columns for memory data
memory_tree = ttk.Treeview(bottom_left_frame, columns=memory_columns, show="headings")

# Configure the column headings
for col in memory_columns:
    memory_tree.heading(col, text=col)
    memory_tree.column(col, width=100)

# Add a scrollbar for the memory_tree
memory_scrollbar = Scrollbar(bottom_left_frame, command=memory_tree.yview)
memory_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
memory_tree.config(yscrollcommand=memory_scrollbar.set)

memory_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)


# IO Port Window Treeview Setup
io_columns = ("IO Port", "Value")  # Define columns for IO port data
io_tree = ttk.Treeview(bottom_right_frame, columns=io_columns, show="headings")

# Configure the column headings
for col in io_columns:
    io_tree.heading(col, text=col)
    io_tree.column(col, width=100)

# Add a scrollbar for the io_tree
io_scrollbar = Scrollbar(bottom_right_frame, command=io_tree.yview)
io_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
io_tree.config(yscrollcommand=io_scrollbar.set)

io_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)


# Bind the text widget to the event
user_input.bind("<KeyRelease>", change_text_color)

window.mainloop()
