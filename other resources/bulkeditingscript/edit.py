import tkinter as tk
from tkinter import IntVar

def submit_data():
    checkbox_status = [var.get() for var in checkbox_vars]
    instr_name = entry1.get().strip()
    
    with open("script.txt", "r") as file:
        content = file.read()
    content = content.replace("!inALL", '"' + instr_name + '"' + ", !inALL")
    with open("script.txt", "w") as file:
        file.write(content)
    for i in range(len(instruction_names)):
        if checkbox_status[i]:
            opcode = entry_fields[i].get().strip()
            
            if not opcode:
                status_label.config(text="error")
                return
            
            with open("script.txt", "r") as file:
                content = file.read()
            
            placeholderin = f"!in{instruction_names[i].upper()}"
            placeholderop = f"!op{instruction_names[i].upper()}"
            opcodeString = f'if (in == "{instr_name}") {{\n    inCode = 0x{opcode};\n}} else {placeholderop}'
            content = content.replace(placeholderin, '"' + instr_name + '"' + f", {placeholderin}").replace(placeholderop, opcodeString)
            
            with open("script.txt", "w") as file:
                file.write(content)
            
            print("Instruction Name:", instr_name)
            print("Opcode:", opcode)
    
    status_label.config(text="success")
    
def validate_hex_input(P):
    if all(c in '0123456789ABCDEFabcdef' for c in P):
        return True
    return False
checkbox_vars = []
instr_labels = []
checkboxes = []
entry_fields = []
def create_instruction_row(root):
    global entry1, status_label
    row_frame = tk.Frame(root)
    
    instr_label = tk.Label(row_frame, text="Instruction:")
    instr_label.grid(row=0, column=0, padx=5, pady=1)
    entry1 = tk.Entry(row_frame, validate='key')
    entry1.grid(row=0, column=1, padx=5, pady=1)
    
    status_label = tk.Label(row_frame, text="status")
    status_label.grid(row=0, column=5, padx=5, pady=1)
    
    submit_button = tk.Button(row_frame, text="Submit", command=submit_data)
    submit_button.grid(row=0, column=6, padx=5, pady=1)


    
    for col, instr in enumerate(instruction_names):
        instr_label = tk.Label(row_frame, text=instr)
        instr_label.grid(row=1, column=col, padx=5, pady=1)
        instr_labels.append(instr_label)

        checkbox_var = IntVar()
        checkbox = tk.Checkbutton(row_frame, variable=checkbox_var)
        checkbox.grid(row=2, column=col, padx=5, pady=1)
        checkboxes.append(checkbox)
        checkbox_vars.append(checkbox_var)
        
        entry = tk.Entry(row_frame, validate='key')
        entry['validatecommand'] = (entry.register(validate_hex_input), '%P')
        entry.grid(row=3, column=col, padx=5, pady=1)
        entry_fields.append(entry)
    
    row_frame.pack()

instruction_names = ["vse", "takraz", "tako", "dir", "ind", "raz", "rel"]
def main():
    root = tk.Tk()
    root.title("Instruction App")
    
    
    create_instruction_row(root)
    
    root.mainloop()

if __name__ == "__main__":
    main()
