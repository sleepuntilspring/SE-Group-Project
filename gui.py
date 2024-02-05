import tkinter as tk
from tkinter import filedialog
from tkinter import Scrollbar
from tkinter import PanedWindow

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
    "m ra, sp": "06",
    "m sp, ra": "07",
    "m ra, @rb": "08",
    "m @ra, rb": "09",
    "m ra, #rb": "0A",
    "m #ra, rb": "0B",
    "m @ra, #rb": "0C",
    "m #ra, @rb": "0D",
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
    text.tag_configure("01", foreground="green")
    text.tag_configure("02", foreground="red")
    text.tag_configure("03", foreground="yellow")
    text.tag_configure("04", foreground="brown")
    text.tag_configure("05", foreground="pink")
    text.tag_configure("06", foreground="orange")
    text.tag_configure("07", foreground="orange")
    text.tag_configure("08", foreground="orange")
    text.tag_configure("09", foreground="orange")
    text.tag_configure("0A", foreground="orange")
    text.tag_configure("0B", foreground="orange")
    text.tag_configure("0C", foreground="orange")
    text.tag_configure("0D", foreground="orange")
    text.tag_configure("0E", foreground="lavender")
    text.tag_configure("0F", foreground="midnight blue")
    text.tag_configure("10", foreground="maroon")
    text.tag_configure("11", foreground="indigo")
    text.tag_configure("12", foreground="grey")


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

right_frame = tk.Frame(paned_window, relief=tk.RAISED, bd=2)
text = tk.Text(right_frame, wrap="word", width=40, height=10)
text.grid(row=0, column=0, sticky="nsew")

output_widget = tk.Text(right_frame, wrap="word", height=2)
output_widget.grid(row=0, column=1, sticky="nsew")

scrollbar = Scrollbar(right_frame, command=text.yview)
scrollbar.grid(row=0, column=2, sticky="nsew")
text.config(yscrollcommand=scrollbar.set)

scrollbar = Scrollbar(paned_window, command=text.yview)
paned_window.add(right_frame)
paned_window.add(scrollbar)

text.config(yscrollcommand=scrollbar.set)

paned_window.add(right_frame)

paned_window.paneconfig(right_frame, minsize=500, stretch="always")

text.bind("<KeyRelease>", run_translation)

window.mainloop()
