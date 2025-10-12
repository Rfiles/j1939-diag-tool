#!/usr/bin/env python3

import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import serial, serial.tools.list_ports
import threading
import queue
import re
import base64
import os

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        # ... (state variables)
        self.transfer_state = "idle" # idle, downloading, uploading_wait_ready, uploading_send_data
        self.upload_content = []
        self.upload_index = 0
        # ... (rest of init)

    def upload_file_gui(self):
        if not (self.serial_connection and self.serial_connection.is_open):
            messagebox.showerror("Error", "Not connected to a device.")
            return

        local_path = filedialog.askopenfilename()
        if not local_path: return

        remote_filename = os.path.basename(local_path)
        remote_path = f"/{remote_filename}"

        try:
            with open(local_path, 'rb') as f:
                encoded_content = base64.b64encode(f.read())
            
            chunk_size = 256
            self.upload_content = [encoded_content[i:i+chunk_size] for i in range(0, len(encoded_content), chunk_size)]
            self.upload_index = 0

            self.transfer_state = "uploading_wait_ready"
            self.serial_connection.write(f"upload {remote_path}\n".encode('utf-8'))
            self.terminal_text.config(state="normal")
            self.terminal_text.insert("end", f"--- Starting upload of {local_path} ---
")
            self.terminal_text.config(state="disabled")

        except Exception as e:
            messagebox.showerror("Error", f"Could not read file: {e}")

    def update_gui(self):
        try:
            while not self.serial_queue.empty():
                line = self.serial_queue.get_nowait()
                # ...

                if self.transfer_state == "uploading_wait_ready":
                    if "READY_FOR_UPLOAD" in line:
                        self.transfer_state = "uploading_send_data"
                # ... (downloading logic)
                else:
                    # Normal terminal output
                    # ...
        finally:
            # Handle sending data in the main GUI thread to avoid serial conflicts
            if self.transfer_state == "uploading_send_data":
                if self.upload_index < len(self.upload_content):
                    chunk = self.upload_content[self.upload_index]
                    self.serial_connection.write(chunk + b'\n')
                    self.upload_index += 1
                else:
                    self.serial_connection.write(b"UPLOAD_END\n")
                    self.transfer_state = "idle"
                    messagebox.showinfo("Upload", "File sent. Waiting for device confirmation...")

            self.after(100, self.update_gui)

    # ... (rest of App class)

if __name__ == "__main__":
    app = App()
    app.mainloop()
