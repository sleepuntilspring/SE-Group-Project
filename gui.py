import tkinter as tk
from tkinter import filedialog
from tkinter import Scrollbar
from tkinter import PanedWindow
from tkinter import ttk  # Import ttk module for Separator

from tkinter.filedialog import askopenfilename, asksaveasfilename

translation_dict = {
    "MOV": "1010",
    "ADD": "1100",
    "SUB": "0011",
    "nop": "00",
    "Reset1 code": "01",
    "Reset2 code": "02",
    "mil": "03",
    "mih": "04",
    "mi": "05",
    "m": "06",
    "m": "07",
    "m": "08",
    "m": "09",
    "m": "0A",
    "m": "0B",
    "m": "0C",
    "m": "0D",
    "and": "0E",
    "or": "0F",
    "xor": "10",
    "not": "11",
    "add": "12",
    "sub": "13",
    "sl": "14",
    "sr": "15",
    "call": "16",
    "ret": "17",
    "push": "18",
    "pop": "19",
    "j": "1A",
    "x=": "1B",
    "x<": "1C",
    "x>": "1D",
    "x<=": "1E",
    "x>=": "1F",
    "r": "",
}

def open_file():
    filepath = askopenfilename(
        filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")]
    )
    if not filepath:
        return
    text.delete("1.0", tk.END)
    with open(filepath, mode="r", encoding="utf-8") as input_file:
        code = input_file.read()
        text.insert(tk.END, code)
    window.title(f"AMIR - {filepath}")

def save_file():
    filepath = asksaveasfilename(
        defaultextension=".txt",
        filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")],
    )
    if not filepath:
        return
    with open(filepath, mode="w", encoding="utf-8") as output_file:
        code = text.get("1.0", tk.END)
        output_file.write(code)
    window.title(f"AMIR - {filepath}")

def add_text():
    text_to_add = text.get("1.0", tk.END)
    text.delete("1.0", tk.END)
    output_text = translate(text_to_add)
    output_widget.delete("1.0", tk.END)
    output_widget.insert(tk.END, output_text)

def run_translation(event=None): 
    input_text = text.get("1.0", tk.END)
    output_text = translate(input_text)
    output_widget.delete("1.0", tk.END)
    output_widget.insert(tk.END, output_text)

    text.tag_delete("00")
    text.tag_delete("01")
    text.tag_delete("02")
    text.tag_delete("03")
    text.tag_delete("04")

    text.tag_configure("00", foreground="blue")
    text.tag_configure("01", foreground="blue")
    text.tag_configure("02", foreground="blue")
    text.tag_configure("03", foreground="blue")
    text.tag_configure("04", foreground="blue")
    text.tag_configure("05", foreground="blue")
    text.tag_configure("06", foreground="blue")
    text.tag_configure("07", foreground="blue")
    text.tag_configure("08", foreground="blue")
    text.tag_configure("09", foreground="blue")
    text.tag_configure("0A", foreground="blue")
    text.tag_configure("0B", foreground="blue")
    text.tag_configure("0C", foreground="blue")
    text.tag_configure("0D", foreground="blue")
    text.tag_configure("0E", foreground="blue")
    text.tag_configure("0F", foreground="blue")
    text.tag_configure("10", foreground="blue")
    text.tag_configure("11", foreground="blue")
    text.tag_configure("12", foreground="blue")
    text.tag_configure("13", foreground="blue")
    text.tag_configure("14", foreground="blue")
    text.tag_configure("15", foreground="blue")
    text.tag_configure("16", foreground="blue")
    text.tag_configure("17", foreground="blue")
    text.tag_configure("18", foreground="blue")
    text.tag_configure("19", foreground="blue")
    text.tag_configure("1A", foreground="blue")
    text.tag_configure("1B", foreground="blue")
    text.tag_configure("1C", foreground="blue")
    text.tag_configure("1D", foreground="blue")
    text.tag_configure("1E", foreground="blue")
    text.tag_configure("1F", foreground="blue")
    text.tag_configure("", foreground="blue")

    text.tag_configure("grey", foreground="grey")

    for code in input_text.split():
        translated = translation_dict.get(code, "UNKNOWN")
        start_index = "1.0"
        while True:
            start_index = text.search(code, start_index, stopindex=tk.END, exact=True)
            if not start_index:
                break
            end_index = f"{start_index}+{len(code)}c"
            text.tag_add(translated, start_index, end_index)
            start_index = end_index

def translate(machine_code):
    assembly_code = []
    for code in machine_code.split():
        translated = translation_dict.get(code, "UNKNOWN")
        assembly_code.append(translated)
    return " ".join(assembly_code)

def toggle_dark_mode():
    bg_color = "black"
    text_color = "white"
    scrollbar_color = "grey"
    frame_color = "grey"
    button_color = "lightgrey"
    separator_color = "white"
    
    

    if window.cget("bg") == "black":
        bg_color = "white"
        text_color = "black"
        scrollbar_color = "lightgrey"
        frame_color = "white"
        button_color = "lightgrey"
        separator_color = "black"

        
    window.config(bg=bg_color)
    
    text.config(bg=bg_color, fg=text_color)
    output_widget.config(bg=bg_color, fg=text_color)
    text_scrollbar.config(troughcolor=scrollbar_color, highlightcolor=scrollbar_color, activebackground=scrollbar_color)
    output_scrollbar.config(troughcolor=scrollbar_color, highlightcolor=scrollbar_color, activebackground=scrollbar_color)

    for widget in paned_window.winfo_children():
        widget.config(bg=bg_color, fg=text_color)
        if isinstance(widget, tk.Text):
            widget.config(selectbackground=text_color, selectforeground=bg_color)

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
    tk.Button(button_frame, text="Toggle Dark Mode / Light Mode", command=toggle_dark_mode),

]

for btn in buttons:
    btn.pack(side=tk.LEFT)

paned_window = tk.PanedWindow(window, orient=tk.HORIZONTAL)
paned_window.pack(expand=True, fill=tk.BOTH)

left_frame = tk.Frame(paned_window)
paned_window.add(left_frame)

text = tk.Text(left_frame, wrap="word", width=40, height=20)
text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

text_scrollbar = Scrollbar(left_frame, command=text.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
text_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
text.config(yscrollcommand=text_scrollbar.set)

separator = ttk.Separator(paned_window, orient="vertical")
paned_window.add(separator)

output_frame = tk.Frame(paned_window)
paned_window.add(output_frame)

output_widget = tk.Text(output_frame, wrap="word", height=20, width=40)
output_widget.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

output_scrollbar = Scrollbar(output_frame, command=output_widget.yview, troughcolor="lightgrey", highlightcolor="grey", activebackground="grey")
output_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
output_widget.config(yscrollcommand=output_scrollbar.set)

text.bind("<KeyRelease>", run_translation)

window.mainloop()
