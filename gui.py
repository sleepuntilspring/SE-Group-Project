import tkinter as tk
from tkinter import filedialog

from tkinter.filedialog import askopenfilename, asksaveasfilename

translation_dict = {
    "1010": "MOV",
    "1100": "ADD",
    "0011": "SUB",
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

def run_translation():
    input_text = text.get("1.0", tk.END)
    output_text = translate(input_text)
    output_widget.delete("1.0", tk.END)
    output_widget.insert(tk.END, output_text)

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

paned_window = tk.PanedWindow(window, orient=tk.HORIZONTAL)
paned_window.pack(expand=True, fill=tk.BOTH)

top_frame = tk.Frame(window)

buttons = [
    tk.Button(top_frame, text="Open", command=open_file),
    tk.Button(top_frame, text="Save As", command=save_file),
    tk.Button(top_frame, text="Run", command=run_translation),
    tk.Button(top_frame, text="About", command=show_about),
]

for btn in buttons:
    btn.pack(pady=5)

paned_window.add(top_frame)

right_frame = tk.Frame(paned_window, relief=tk.RAISED, bd=2)

text = tk.Text(right_frame, wrap="none", width=40, height=10)
text.grid(row=0, column=0, sticky="nsew")

output_widget = tk.Text(right_frame, wrap="word", height=2)  # Set height to 2 lines
output_widget.grid(row=0, column=1, sticky="nsew")

right_frame.grid_columnconfigure(0, weight=1, uniform="equal")
right_frame.grid_columnconfigure(1, weight=1, uniform="equal")
right_frame.grid_rowconfigure(0, weight=1)

paned_window.add(right_frame)

paned_window.paneconfig(top_frame, minsize=50, stretch="always")
paned_window.paneconfig(right_frame, minsize=700, stretch="always")

window.mainloop()
