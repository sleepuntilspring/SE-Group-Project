import tkinter as tk
from tkinter import filedialog

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

def run_translation(event=None):  # Updated function signature to handle events
    input_text = text.get("1.0", tk.END)
    output_text = translate(input_text)
    output_widget.delete("1.0", tk.END)
    output_widget.insert(tk.END, output_text)

    text.tag_delete("1010")
    text.tag_delete("1100")
    text.tag_delete("0011")

    text.tag_configure("1010", foreground="blue")
    text.tag_configure("1100", foreground="green")
    text.tag_configure("0011", foreground="red")

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

left_frame = tk.Frame(paned_window, relief=tk.RAISED, bd=2)
left_text = tk.Text(left_frame, wrap="none", width=40, height=10)
left_text.grid(row=0, column=0, sticky="nsew")
left_frame.grid_columnconfigure(0, weight=1, uniform="equal")
left_frame.grid_rowconfigure(0, weight=1)
paned_window.add(left_frame)

right_frame = tk.Frame(paned_window, relief=tk.RAISED, bd=2)
text = tk.Text(right_frame, wrap="none", width=40, height=10)
text.grid(row=0, column=0, sticky="nsew")

output_widget = tk.Text(right_frame, wrap="word", height=2)
output_widget.grid(row=0, column=1, sticky="nsew")

right_frame.grid_columnconfigure(0, weight=1, uniform="equal")
right_frame.grid_columnconfigure(1, weight=1, uniform="equal")
right_frame.grid_rowconfigure(0, weight=1)

paned_window.add(right_frame)

paned_window.paneconfig(left_frame, minsize=200, stretch="always")
paned_window.paneconfig(right_frame, minsize=500, stretch="always")

text.bind("<KeyRelease>", run_translation)

window.mainloop()
