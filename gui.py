import tkinter as tk
from tkinter import filedialog

from tkinter.filedialog import askopenfilename, asksaveasfilename

def open_file():
    filepath = askopenfilename(
        filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")]
    )
    if not filepath:
        return
    txt_edit.delete("1.0", tk.END)
    with open(filepath, mode="r", encoding="utf-8") as input_file:
        text = input_file.read()
        txt_edit.insert(tk.END, text)
    window.title(f"AMIR2 - {filepath}")

def save_file():
    filepath = asksaveasfilename(
        defaultextension=".txt",
        filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")],
    )
    if not filepath:
        return
    with open(filepath, mode="w", encoding="utf-8") as output_file:
        text = txt_edit.get("1.0", tk.END)
        output_file.write(text)
    window.title(f"AMIR2 - {filepath}")

def add_text():
    text_to_add = entry.get()
    txt_edit.insert(tk.END, text_to_add + '\n')
    entry.delete(0, tk.END)

window = tk.Tk()
window.title("AMIR2")

paned_window = tk.PanedWindow(window, orient=tk.HORIZONTAL)
paned_window.pack(expand=True, fill=tk.BOTH)

top_frame = tk.Frame(window)
btn_open = tk.Button(top_frame, text="Open", command=open_file)
btn_save = tk.Button(top_frame, text="Save As...", command=save_file)
btn_open.pack(side=tk.TOP, padx=5, pady=5)
btn_save.pack(side=tk.TOP, padx=5, pady=5)

paned_window.add(top_frame)

right_frame = tk.Frame(paned_window, relief=tk.RAISED, bd=2)

txt_edit = tk.Text(right_frame)
txt_edit.grid(row=0, column=0, sticky="nsew")

entry = tk.Entry(right_frame)
entry.grid(row=0, column=1, sticky="nsew")
add_button = tk.Button(right_frame, text="Translate", command=add_text)
add_button.grid(row=1, column=0, columnspan=2, pady=5)

right_frame.grid_columnconfigure(0, weight=1)
right_frame.grid_columnconfigure(1, weight=1)

right_frame.grid_rowconfigure(0, weight=1)

paned_window.add(right_frame)

paned_window.paneconfig(top_frame, minsize=50, stretch="always")
paned_window.paneconfig(right_frame, minsize=700, stretch="always")

window.mainloop()
