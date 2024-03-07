import tkinter as tk
from tkinter import filedialog, Scrollbar, ttk, messagebox
import os
import re
import tkinter.messagebox
import webbrowser
from PIL import Image, ImageTk

import tkinter as tk
from idlelib.tooltip import Hovertip

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
    paned_window.add(right_frame)
    change_text_color() 
    window.title(f"AMIR - {filepath}")
    
    # Add line numbering to the window
    add_line_numbering(window)
    user_input.bind("<KeyRelease>", change_text_color, update_line_numbers)
       
def new_project():
    user_input.delete("1.0", tk.END)
    window.title("AMIR - New Project")
    paned_window.add(right_frame)
    add_line_numbering(window)
    user_input.bind("<KeyRelease>", change_text_color, update_line_numbers)

def save_file():
    current_filepath = window.title().replace("AMIR - ", "")
    
    if current_filepath and "New Project" not in current_filepath:
        # If the file was opened before and it's not a new project, save to the existing file
        filepath = current_filepath
    else:
        # If it's a new project or not opened before, prompt the user for a file path
        filepath = filedialog.asksaveasfilename(
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

def compile_Program(): #compile the file first
    
    with open('temp_input2.txt', 'w', encoding='utf-8') as file:
        input_text = user_input.get("1.0", tk.END)
        file.write(input_text)

    c_program_path2 = '"C:\\Users\\leeye\\A_LYY\\1_Sem1\\AMIR-1\\AMIR-1_hello.exe"'

    os.system(c_program_path2)
    
def run_translation(): #for STEP IN BUTTON

    # Enclose the executable path in double quotes
    c_program_path = '"C:\\Users\\leeye\\A_LYY\\1_Sem1\\AMIR-1\\segp_operation.exe"'
    c_program_path2 = '"C:\\Users\\leeye\\A_LYY\\1_Sem1\\AMIR-1\\AMIR-1_hello.exe"'
    
    # Call C backend to process 'example.txt'
    os.system(c_program_path)
    os.system(c_program_path2)

    # Check if the output file exists before trying to open it
    if os.path.exists('registers_output.txt'):
        with open('registers_output.txt', 'r', encoding='utf-8') as file:
            r_output_text = file.read()

            # Clear existing entries in the tree
            for item in register_tree.get_children():
                register_tree.delete(item)

            # Insert new data into the tree
            data = parse_registers_output(r_output_text)
            for row in data:
                register_tree.insert("", "end", values=row)

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
        
def run_Program(): #when RUN BUTTON is clicked
    
    # Save current input to 'example.txt'
    with open('temp_input2.txt', 'w', encoding='utf-8') as file:
        input_text = user_input.get("1.0", tk.END)
        file.write(input_text)
        # Enclose the executable path in double quotes
    c_program_path = '"C:\\Users\\leeye\\A_LYY\\1_Sem1\\AMIR-1\\segp_operation.exe"'
    c_program_path2 = '"C:\\Users\\leeye\\A_LYY\\1_Sem1\\AMIR-1\\AMIR-1_hello.exe"'
    
    # Call C backend to process 'example.txt'
    os.system(c_program_path)
    os.system(c_program_path2)


    # Check if the error.log file exists and is not empty
    if os.path.exists('error.log') and os.path.getsize('error.log') > 0:
        with open('error.log', 'r', encoding='utf-8') as error_file:
            error_message = error_file.read()
            tkinter.messagebox.showerror("Error", "Please check the debug corner for more details.")
    else: 

        # Check if the output file exists before trying to open it
        if os.path.exists('registers_output.txt'):
            with open('registers_output.txt', 'r', encoding='utf-8') as file:
                r_output_text = file.read()

                for item in register_tree.get_children():
                    register_tree.delete(item)

                # Insert new data into the tree
                data = parse_registers_output(r_output_text)
                for row in data:
                    register_tree.insert("", "end", values=row)
                    
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

output_file_path = 'C:\\Users\\leeye\\A_LYY\\1_Sem1\\AMIR-1\\getLines.txt'

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
landed_lines = []
results = []
global i
i = 0
global flag
flag = False

def step_in():

        global current_line
        global jumped
        global landed_lines
        lines_away_value = 1
        global flag
        global i

        if flag:
             i = 0
        else: 
            
            # Remove any existing tags
            user_input.tag_remove("highlight", "1.0", tk.END)

            # If current_line is not set, start from the beginning
            if current_line is None:
                current_line = "1.0"
                lines_away_value = 0

            line_text = user_input.get(f"{current_line} linestart", f"{current_line} lineend")

            if not line_text.startswith('--') and (
                "x=" in line_text or
                "x<=" in line_text or
                "x>=" in line_text or
                "x<" in line_text or
                "x>" in line_text or
                "j" in line_text or
                "call" in line_text) and not jumped:

                # Run the program and get lines away
                run_Program()
                keyword = "found"
                lines_away_values = get_lines_away(keyword)

                if lines_away_values is None:
                    lines_away_value = 1

                
                while lines_away_values and i < len(lines_away_values):
                    lines_away_value = lines_away_values.pop(i)

                    # print(f"{keyword} {lines_away_value} lines away.")

                    # Update the landed_lines list
                    landed_lines.append(current_line)

                    # Jump to the calculated position in the text widget
                    current_line = user_input.index(f"{current_line} + {lines_away_value} lines linestart")
                
                    # Reset jumped to False
                    jumped = False

                    i += 1  # increment index

            else:
                # Update the landed_lines list
                landed_lines.append(current_line)

            
                # Move to the next line
                current_line = user_input.index(f"{current_line} + {lines_away_value} lines linestart")
                lines_away_value = 1
                jumped = False

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
        flag = False
   
def get_lines_away(keyword):
    results = []
    run_Program()

    with open(output_file_path, 'r') as file:
        for line in file:
            match = re.match(fr'{keyword} (\d+)', line)
            if match:
                results.append(int(match.group(1)))

    return results

def step_up():
    global current_line
    global landed_lines
    global jumped

    global flag
    flag = True

    # Remove any existing tags
    user_input.tag_remove("highlight", "1.0", tk.END)

    # If current_line is not set, start from the beginning
    if current_line is None:
        current_line = "1.0"

    # Move to the previous landed line or the previous line
    if landed_lines:
        current_line = landed_lines.pop()
        jumped = False  # Set jumped to False when moving to the previous landed line
    else:
        # If no previous landed lines or labels, move to the previous line
        current_line = user_input.index(f"{current_line} - 1 lines linestart")
        jumped = False  # Set jumped to False when moving to the previous line

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
    message = "About Amir\n\n"
    message += "For more information, visit our GitHub:\n"
    message += "https://github.com/sleepuntilspring/SE-Group-Project/tree/main/amir%20gui"

    webbrowser.open("https://github.com/sleepuntilspring/SE-Group-Project/tree/main/amir%20gui")
    
    tkinter.messagebox.showinfo("About", message)

# Variable to track the mode
is_dark_mode = False

# Function to toggle Dark/Light Mode
def toggle_theme():
    global is_dark_mode
    is_dark_mode = not is_dark_mode
    
    if is_dark_mode:
        # Set Dark Mode Styles
        user_input.config(bg='#363636', fg='white')  # Example colors for dark mode
        for tree in [memory_tree, register_tree, io_tree]:
            tree.tag_configure('evenrow', background='#363636', foreground='white')  # Adjust the row colors
            tree.tag_configure('oddrow', background='#363636', foreground='white')  # Adjust the row colors
        settings_menu.entryconfig("Dark Mode", label="Light Mode", image=sun_icon, compound='left')
    else:
        # Set Light Mode Styles
        user_input.config(bg='white', fg='black')  # Revert to default colors for light mode
        for tree in [memory_tree, register_tree, io_tree]:
            tree.tag_configure('evenrow', background='white', foreground='black')  # Revert the row colors
            tree.tag_configure('oddrow', background='white', foreground='black')  # Revert the row colors
        settings_menu.entryconfig("Light Mode", label="Dark Mode", image=moon_icon, compound='left')

    # Force the Treeview update
    for tree in [memory_tree, register_tree, io_tree]:
        for i, item in enumerate(tree.get_children()):
            tree.item(item, tags=('evenrow' if i % 2 == 0 else 'oddrow',))

window = tk.Tk()
window.title("AMIR")

menu_bar = tk.Menu(window)
window.config(menu=menu_bar)

top_frame = tk.Frame(window)
top_frame.pack(side=tk.TOP, fill=tk.X)

button_frame = tk.Frame(top_frame)
button_frame.pack(side=tk.LEFT, padx=5)

# Get the current script's directory
script_dir = os.path.dirname(os.path.abspath(__file__))

# Function to create an image and return a PhotoImage object
def create_image_icon(image_name, width, height):
    image_path = os.path.join(script_dir, "icons", image_name)
    image = Image.open(image_path)
    image = image.resize((width, height), 3)
    return ImageTk.PhotoImage(image)


open_icon = create_image_icon("open-.png", 15, 15)  # Replace "open_icon.png" with your actual icon file
save_icon = create_image_icon("save.png", 18, 18)  # Replace "save_icon.png" with your actual icon file
run_icon = create_image_icon("run.png", 12, 12)
step_in_icon= create_image_icon ("step_in.png", 15, 15)
step_up_icon= create_image_icon ("step_up.png", 15, 15)
step_out_icon= create_image_icon ("stepOut.png", 15, 15)
about_icon= create_image_icon("about.png",15,15)
moon_icon = create_image_icon("moon.png", 15, 15)
sun_icon = create_image_icon("sun.png", 15, 15)
github_icon = create_image_icon("github.png", 15, 15)
new_icon = create_image_icon("new.png",15,15)
compile_icon = create_image_icon("compile.png",15,15)

file_menu = tk.Menu(menu_bar, tearoff=0)
menu_bar.add_cascade(label="File", menu=file_menu)

file_menu.add_command(label="Open", image=open_icon, compound='left', command=open_file)
file_menu.add_command(label="Save", image=save_icon, compound='left', command=save_file)
file_menu.add_command(label="New Project", image=new_icon, compound='left', command=new_project)

debug_menu = tk.Menu(menu_bar, tearoff=0)
menu_bar.add_cascade(label="Debug", menu=debug_menu)

debug_menu.add_command(label="Step In", image=step_in_icon, compound='left', command=step_in)
debug_menu.add_command(label="Step Up", image=step_up_icon, compound='left', command=step_up)
debug_menu.add_command(label="Step Out", image=step_out_icon, compound='left', command=step_out)
debug_menu.add_command(label="Run", image=run_icon, compound='left', command=run_Program)

about_menu = tk.Menu(menu_bar, tearoff=0)
menu_bar.add_cascade(label="About", menu=about_menu)
about_menu.add_command(label="Github", image=github_icon, compound='left', command=show_about)

settings_menu = tk.Menu(menu_bar, tearoff=0)
menu_bar.add_cascade(label="Settings", menu=settings_menu)
settings_menu.add_command(label="Dark Mode", image=moon_icon, compound='left', command=toggle_theme)

paned_window = tk.PanedWindow(window, orient=tk.HORIZONTAL)
paned_window.pack(expand=True, fill=tk.BOTH)

class CreateToolTip(object):
    """
    create a tooltip for a given widget
    """
    def __init__(self, widget, text='widget info'):
        self.waittime = 500     # miliseconds
        self.wraplength = 180   # pixels
        self.widget = widget
        self.text = text
        self.default_bg = self.widget.cget("background")  # Store the default background color
        self.widget.bind("<Enter>", self.enter)
        self.widget.bind("<Leave>", self.leave)
        self.widget.bind("<ButtonPress>", self.leave)
        self.id = None
        self.tw = None

    def enter(self, event=None):
        self.widget.config(background="light blue")  # Change the background color
        self.schedule()

    def leave(self, event=None):
        self.unschedule()
        self.widget.config(background=self.default_bg)  # Restore the default background color
        self.hidetip()

    def schedule(self):
        self.unschedule()
        self.id = self.widget.after(self.waittime, self.showtip)

    def unschedule(self):
        id = self.id
        self.id = None
        if id:
            self.widget.after_cancel(id)

    def showtip(self, event=None):
        x = y = 0
        x, y, cx, cy = self.widget.bbox("insert")
        x += self.widget.winfo_rootx() + 25
        y += self.widget.winfo_rooty() + 20
        # creates a toplevel window
        self.tw = tk.Toplevel(self.widget)
        # Leaves only the label and removes the app window
        self.tw.wm_overrideredirect(True)
        self.tw.wm_geometry("+%d+%d" % (x, y))
        label = tk.Label(self.tw, text=self.text, justify='left',
                       background="#ffffff", relief='solid', borderwidth=1,
                       wraplength = self.wraplength)
        label.pack(ipadx=1)

    def hidetip(self):
        tw = self.tw
        self.tw= None
        if tw:
            tw.destroy()

open_button = tk.Button(button_frame ,text="    ", image=open_icon, compound=tk.LEFT, command=open_file)
open_button.pack(side=tk.LEFT)
tooltip = CreateToolTip(open_button, "Open")

compile_button = tk.Button(button_frame,text="    ",  image=compile_icon, compound=tk.LEFT, command=compile_Program)
compile_button.pack(side=tk.LEFT)
tooltip = CreateToolTip(compile_button, "Compile")

run_button = tk.Button(button_frame,text="    ",  image=run_icon, compound=tk.LEFT, command=run_Program)
run_button.pack(side=tk.LEFT)
tooltip = CreateToolTip(run_button, "Run")

step_in_button = tk.Button(button_frame,text="    ",  image=step_in_icon, compound=tk.LEFT, command=step_in)
step_in_button.pack(side=tk.LEFT)
tooltip = CreateToolTip(step_in_button, "Step In")

step_up_button = tk.Button(button_frame, text="    ", image=step_up_icon, compound=tk.LEFT, command=step_up)
step_up_button.pack(side=tk.LEFT)
tooltip = CreateToolTip(step_up_button, "Step Up")

step_out_button = tk.Button(button_frame, text="    ",  image=step_out_icon, compound=tk.LEFT, command=step_out)
step_out_button.pack(side=tk.LEFT)
tooltip = CreateToolTip(step_out_button, "Step Out")


# Left Frame for New Box (Table Display)
left_frame = tk.Frame(paned_window)
paned_window.add(left_frame)
search_frame = tk.Frame(left_frame)
search_frame.pack(side=tk.BOTTOM, fill=tk.X)

search_entry = tk.Entry(search_frame)
search_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)

search_button = tk.Button(search_frame, text="Search", command=lambda: search_tree(register_tree, search_entry.get()))
search_button.pack(side=tk.LEFT, padx=5)
treeview_container = tk.Frame(left_frame)
treeview_container.pack(fill=tk.BOTH, expand=True)

# Create a Treeview widget for displaying data in a table
columns = ("Registers", "Content")  # Adjust the column names accordingly
register_tree = ttk.Treeview(treeview_container, columns=columns, show="headings")

# Set column headings
for col in columns:
    register_tree.heading(col, text=col)
    register_tree.column(col, width=100) 

register_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

# Scrollbar for the Treeview
tree_scrollbar = Scrollbar(treeview_container, command=register_tree.yview)
tree_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
register_tree.config(yscrollcommand=tree_scrollbar.set)


separator = ttk.Separator(paned_window, orient="vertical")
paned_window.add(separator)

right_frame = tk.Frame(paned_window)

def load_data(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        # Assuming your data is organized, parse it accordingly
        data = [line.strip().split('\t') for line in lines]
    return data

# Line Numbers
line_numbers = tk.Text(right_frame, width=4, padx=5, pady=5, wrap=tk.NONE)
line_numbers.pack(side=tk.LEFT, fill=tk.Y)

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
memory_tree_container = tk.Frame(bottom_left_frame)
memory_tree_container.pack(fill=tk.BOTH, expand=True)

bottom_right_frame = tk.Frame(bottom_frame)
bottom_right_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
io_tree_container = tk.Frame(bottom_right_frame)
io_tree_container.pack(fill=tk.BOTH, expand=True)

# Memory Window Treeview Setup
memory_columns = ("Address", "Content")  # Define columns for memory data
memory_tree = ttk.Treeview(memory_tree_container, columns=memory_columns, show="headings")
memory_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

# Configure the column headings
for col in memory_columns:
    memory_tree.heading(col, text=col)
    memory_tree.column(col, width=100)

memory_scrollbar = Scrollbar(memory_tree_container, command=memory_tree.yview)
memory_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
memory_tree.config(yscrollcommand=memory_scrollbar.set)
memory_search_frame = tk.Frame(bottom_left_frame)
memory_search_frame.pack(side=tk.BOTTOM, fill=tk.X)

memory_search_entry = tk.Entry(memory_search_frame)
memory_search_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)

memory_search_button = tk.Button(memory_search_frame, text="Search", command=lambda: search_tree(memory_tree, memory_search_entry.get()))
memory_search_button.pack(side=tk.LEFT, padx=5)

# IO Port Window Treeview Setup
io_columns = ("IO Port", "Value")  # Define columns for IO port data
io_tree = ttk.Treeview(io_tree_container, columns=io_columns, show="headings")
io_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)


# Configure the column headings
for col in io_columns:
    io_tree.heading(col, text=col)
    io_tree.column(col, width=100)

io_scrollbar = Scrollbar(io_tree_container, command=io_tree.yview)
io_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
io_tree.config(yscrollcommand=io_scrollbar.set)
io_search_frame = tk.Frame(bottom_right_frame)
io_search_frame.pack(side=tk.BOTTOM, fill=tk.X)

io_search_entry = tk.Entry(io_search_frame)
io_search_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)

io_search_button = tk.Button(io_search_frame, text="Search", command=lambda: search_tree(io_tree, io_search_entry.get()))
io_search_button.pack(side=tk.LEFT, padx=5)

# After creating each Treeview in your setup
for tree in [memory_tree, register_tree, io_tree]:
    tree.tag_configure('evenrow', background='white')
    tree.tag_configure('oddrow', background='white')


def search_tree(tree, search_query):
    for item in tree.get_children():
        tree.item(item, tags=())  # Clear existing tags
    tree.tag_configure('found', background='yellow')
    
    found = False
    for item in tree.get_children():
        if search_query.lower() in tree.item(item, 'values')[0].lower():
            tree.item(item, tags=('found',))
            tree.see(item)
            found = True
            break  # Remove to search all instances
    
    if not found:
        print("No match found.")


# Bind the text widget to the event
user_input.bind("<KeyRelease>", change_text_color)

def on_closing():
    # Ask the user for confirmation before closing
    answer = tkinter.messagebox.askquestion("Confirmation", "Are you sure you want to exit?\nClick 'Yes' to Save File and Exit")
    if answer == "yes":
        save_file()
        window.destroy()

# Function to read the 'error.log' file and return its content
def read_error_log():
    error_log_path = 'error.log'
    if os.path.exists(error_log_path):
        with open(error_log_path, 'r') as error_file:
            return error_file.read()
    return None

# Function to display the error message in a separate window or dialog box
def display_error_message():
    error_message = read_error_log()
    if error_message:
        messagebox.showerror("Error Message", error_message)

# Create a function to handle the debug corner
def show_debug_corner():
    debug_corner = tk.Toplevel(window)
    debug_corner.title("Debug Corner")

    # Add a label to display the error message
    error_message_label = tk.Label(debug_corner, text="Error Message:")
    error_message_label.pack()

    # Add a text widget to display the error message content
    error_message_text = tk.Text(debug_corner, wrap="word", width=80, height=15)
    error_message_text.pack()

    # Insert the error message content into the text widget
    error_message = read_error_log()
    if error_message:
        error_message_text.config(state=tk.NORMAL)
        error_message_text.insert(tk.END, error_message)
        error_message_text.config(state=tk.DISABLED)
    else:
        error_message_text.insert(tk.END, "No errors.")

    # Update the window to ensure the geometry calculation takes effect
    debug_corner.update_idletasks()

    # Center the debug corner window on the screen
    screen_width = debug_corner.winfo_screenwidth()
    screen_height = debug_corner.winfo_screenheight()
    window_width = debug_corner.winfo_width()
    window_height = debug_corner.winfo_height()
    x_coordinate = int((screen_width - window_width) / 2)
    y_coordinate = int((screen_height - window_height) / 2)
    debug_corner.geometry(f"+{x_coordinate}+{y_coordinate}")

def add_line_numbering(window):

    def on_text_change(event=None):  # Add event=None to handle initial update
        update_line_numbers()

    user_input.bind("<KeyRelease>", on_text_change)  # Bind KeyRelease event

    update_line_numbers()

def update_line_numbers():
        current_lines = user_input.get("1.0", "end").count("\n")
        if current_lines != int(line_numbers.index("end-1c").split(".")[0]):
            line_numbers.config(state=tk.NORMAL)
            line_numbers.delete("1.0", "end")
            for i in range(1, current_lines + 2):
                line_numbers.insert("end", str(i) + "\n")
            line_numbers.config(state=tk.DISABLED)

# User Control Keys
window.bind("<Control-o>", lambda event: open_file())  # Ctrl + O for open
window.bind("<Control-s>", lambda event: save_file())  # Ctrl + S for save
window.bind("<Control-r>", lambda event: run_Program())  # Ctrl + R for run
window.bind("<Control-i>", lambda event: step_in())  # Ctrl + I for step in
window.bind("<Control-u>", lambda event: step_up())  # Ctrl + U for step up
window.bind("<Control-o>", lambda event: step_out())  # Ctrl + O for step out

# Create a button to show the debug corner
debug_button = tk.Button(top_frame, text="Show Debug Corner", command=show_debug_corner)
debug_button.pack(side=tk.RIGHT, padx=5)

# Add this line to bind the closing event to the function
window.protocol("WM_DELETE_WINDOW", on_closing)
window.mainloop()
