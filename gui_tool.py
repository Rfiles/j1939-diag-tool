#!/usr/bin/env python3

import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import serial, serial.tools.list_ports
import threading
import queue
import re
import base64
import os
import json

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("J1939 Diagnostic Tool")
        self.geometry("800x600")

        self.serial_connection = None
        self.serial_queue = queue.Queue()
        self.serial_thread = None
        self.thread_running = False

        self.transfer_state = "idle" # idle, downloading, uploading_wait_ready, uploading_send_data
        self.upload_content = []
        self.upload_index = 0
        self.download_file_path = ""
        self.download_data = ""

        # Create Notebook (for tabs)
        self.notebook = ttk.Notebook(self)
        self.notebook.pack(expand=True, fill="both", padx=5, pady=5)

        # Create Terminal Tab
        self.create_terminal_tab()
        
        # Create File Manager Tab
        self.create_file_manager_tab()
        
        # Create Configuration Tab
        self.create_configuration_tab()

        self.update_gui()

    def create_terminal_tab(self):
        self.terminal_frame = ttk.Frame(self.notebook)
        self.notebook.add(self.terminal_frame, text="Terminal")

        # Serial connection frame
        connection_frame = ttk.LabelFrame(self.terminal_frame, text="Connection")
        connection_frame.pack(side="top", fill="x", padx=5, pady=5)

        self.port_label = ttk.Label(connection_frame, text="Port:")
        self.port_label.pack(side="left", padx=5, pady=5)

        self.port_combobox = ttk.Combobox(connection_frame, values=self.get_serial_ports())
        self.port_combobox.pack(side="left", padx=5, pady=5)

        self.connect_button = ttk.Button(connection_frame, text="Connect", command=self.toggle_connection)
        self.connect_button.pack(side="left", padx=5, pady=5)
        
        self.refresh_button = ttk.Button(connection_frame, text="Refresh", command=self.refresh_ports)
        self.refresh_button.pack(side="left", padx=5, pady=5)

        # Terminal view
        terminal_view_frame = ttk.LabelFrame(self.terminal_frame, text="Terminal")
        terminal_view_frame.pack(expand=True, fill="both", padx=5, pady=5)

        self.terminal_text = tk.Text(terminal_view_frame, wrap="word", state="disabled", height=15)
        self.terminal_text.pack(side="left", expand=True, fill="both", padx=5, pady=5)

        terminal_scrollbar = ttk.Scrollbar(terminal_view_frame, orient="vertical", command=self.terminal_text.yview)
        terminal_scrollbar.pack(side="right", fill="y")
        self.terminal_text.config(yscrollcommand=terminal_scrollbar.set)
        
        # Command input
        command_frame = ttk.LabelFrame(self.terminal_frame, text="Command")
        command_frame.pack(side="bottom", fill="x", padx=5, pady=5)
        
        self.command_entry = ttk.Entry(command_frame)
        self.command_entry.pack(side="left", expand=True, fill="x", padx=5, pady=5)
        self.command_entry.bind("<Return>", self.send_command)

        self.send_button = ttk.Button(command_frame, text="Send", command=self.send_command)
        self.send_button.pack(side="right", padx=5, pady=5)

    def create_file_manager_tab(self):
        self.file_manager_frame = ttk.Frame(self.notebook)
        self.notebook.add(self.file_manager_frame, text="File Manager")

        # --- Controls ---
        fm_controls_frame = ttk.Frame(self.file_manager_frame)
        fm_controls_frame.pack(side="top", fill="x", padx=5, pady=5)

        self.list_files_button = ttk.Button(fm_controls_frame, text="List Files", command=self.list_remote_files)
        self.list_files_button.pack(side="left", padx=5, pady=5)

        self.upload_button = ttk.Button(fm_controls_frame, text="Upload File", command=self.upload_file_gui)
        self.upload_button.pack(side="left", padx=5, pady=5)

        self.download_button = ttk.Button(fm_controls_frame, text="Download File", command=self.download_file_gui)
        self.download_button.pack(side="left", padx=5, pady=5)

        # --- File Tree ---
        fm_tree_frame = ttk.Frame(self.file_manager_frame)
        fm_tree_frame.pack(expand=True, fill="both", padx=5, pady=5)
        
        self.file_tree = ttk.Treeview(fm_tree_frame, columns=("size",), show="tree headings")
        self.file_tree.heading("#0", text="Filename")
        self.file_tree.heading("size", text="Size")
        self.file_tree.pack(side="left", expand=True, fill="both")
        
        tree_scrollbar = ttk.Scrollbar(fm_tree_frame, orient="vertical", command=self.file_tree.yview)
        tree_scrollbar.pack(side="right", fill="y")
        self.file_tree.config(yscrollcommand=tree_scrollbar.set)
        
        # --- Progress Bar ---
        self.progress_bar = ttk.Progressbar(self.file_manager_frame, orient="horizontal", mode="determinate")
        self.progress_bar.pack(side="bottom", fill="x", padx=5, pady=5)
        
    def create_configuration_tab(self):
        self.configuration_frame = ttk.Frame(self.notebook)
        self.notebook.add(self.configuration_frame, text="Configuration")
        
        self.config_vars = {}
        
        # --- WiFi ---
        wifi_frame = ttk.LabelFrame(self.configuration_frame, text="WiFi")
        wifi_frame.pack(fill="x", padx=5, pady=5)
        self.create_config_entry(wifi_frame, "wifi.ssid", "SSID:")
        self.create_config_entry(wifi_frame, "wifi.password", "Password:")

        # --- MQTT ---
        mqtt_frame = ttk.LabelFrame(self.configuration_frame, text="MQTT")
        mqtt_frame.pack(fill="x", padx=5, pady=5)
        self.create_config_entry(mqtt_frame, "mqtt.broker", "Broker:")
        self.create_config_entry(mqtt_frame, "mqtt.port", "Port:")
        self.create_config_entry(mqtt_frame, "mqtt.topic", "Topic:")

        # --- OTA ---
        ota_frame = ttk.LabelFrame(self.configuration_frame, text="OTA")
        ota_frame.pack(fill="x", padx=5, pady=5)
        self.create_config_entry(ota_frame, "ota.hostname", "Hostname:")
        self.create_config_entry(ota_frame, "ota.password", "Password:")

        # --- Features ---
        features_frame = ttk.LabelFrame(self.configuration_frame, text="Features")
        features_frame.pack(fill="x", padx=5, pady=5)
        self.create_config_checkbox(features_frame, "features.wifi_enabled", "WiFi Enabled")
        self.create_config_checkbox(features_frame, "features.mqtt_enabled", "MQTT Enabled")
        self.create_config_checkbox(features_frame, "features.gps_enabled", "GPS Enabled")

        # --- Controls ---
        config_controls_frame = ttk.Frame(self.configuration_frame)
        config_controls_frame.pack(side="bottom", fill="x", padx=5, pady=5)

        self.load_config_button = ttk.Button(config_controls_frame, text="Load from device", command=self.load_config_from_device)
        self.load_config_button.pack(side="left", padx=5, pady=5)
        
        self.save_config_button = ttk.Button(config_controls_frame, text="Save to device", command=self.save_config_to_device)
        self.save_config_button.pack(side="left", padx=5, pady=5)

    def create_config_entry(self, parent, key, label_text):
        frame = ttk.Frame(parent)
        frame.pack(fill="x", padx=5, pady=2)
        label = ttk.Label(frame, text=label_text, width=15)
        label.pack(side="left")
        var = tk.StringVar()
        entry = ttk.Entry(frame, textvariable=var)
        entry.pack(side="left", fill="x", expand=True)
        self.config_vars[key] = var
        
    def create_config_checkbox(self, parent, key, label_text):
        frame = ttk.Frame(parent)
        frame.pack(fill="x", padx=5, pady=2)
        var = tk.BooleanVar()
        checkbox = ttk.Checkbutton(frame, text=label_text, variable=var)
        checkbox.pack(side="left")
        self.config_vars[key] = var

    def get_serial_ports(self):
        return [port.device for port in serial.tools.list_ports.comports()]

    def refresh_ports(self):
        self.port_combobox['values'] = self.get_serial_ports()

    def toggle_connection(self):
        if self.serial_connection and self.serial_connection.is_open:
            self.disconnect()
        else:
            self.connect()
            
    def connect(self):
        port = self.port_combobox.get()
        if not port:
            messagebox.showerror("Error", "Please select a port.")
            return

        try:
            self.serial_connection = serial.Serial(port, 115200, timeout=1)
            self.thread_running = True
            self.serial_thread = threading.Thread(target=self.read_from_serial, daemon=True)
            self.serial_thread.start()
            self.connect_button.config(text="Disconnect")
            self.log_to_terminal(f"Connected to {port}\n")
        except serial.SerialException as e:
            messagebox.showerror("Error", f"Failed to connect: {e}")

    def disconnect(self):
        if self.serial_connection and self.serial_connection.is_open:
            self.thread_running = False
            # Wait for the thread to finish
            if self.serial_thread and self.serial_thread.is_alive():
                self.serial_thread.join()
            self.serial_connection.close()
            self.connect_button.config(text="Connect")
            self.log_to_terminal("Disconnected\n")

    def read_from_serial(self):
        while self.thread_running:
            try:
                if self.serial_connection and self.serial_connection.is_open and self.serial_connection.in_waiting:
                    line = self.serial_connection.readline().decode('utf-8', errors='replace').strip()
                    if line:
                        self.serial_queue.put(line)
            except (serial.SerialException, TypeError):
                # This can happen when disconnecting
                break
            except Exception as e:
                # Catch other potential exceptions to prevent thread from crashing
                print(f"Error in serial read thread: {e}")
                break
        
    def send_command(self, event=None):
        command = self.command_entry.get()
        if command and self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.write(f"{command}\n".encode('utf-8'))
            self.log_to_terminal(f"> {command}\n")
            self.command_entry.delete(0, "end")

    def log_to_terminal(self, message):
        self.terminal_text.config(state="normal")
        self.terminal_text.insert("end", message)
        self.terminal_text.see("end")
        self.terminal_text.config(state="disabled")

    def update_gui(self):
        try:
            while not self.serial_queue.empty():
                line = self.serial_queue.get_nowait()
                self.handle_serial_line(line)
        finally:
            if self.transfer_state == "uploading_send_data":
                self.send_upload_chunk()
            self.after(100, self.update_gui)
            
    def handle_serial_line(self, line):
        # Handle file listing
        match = re.match(r"FILE: (.+), SIZE: (\d+)", line)
        if match:
            filename, size = match.groups()
            self.file_tree.insert("", "end", text=filename, values=(size,))
            return

        # Handle upload
        if self.transfer_state == "uploading_wait_ready" and "READY_FOR_UPLOAD" in line:
            self.transfer_state = "uploading_send_data"
            return
            
        if self.transfer_state.startswith("uploading") and "UPLOAD_SUCCESS" in line:
            messagebox.showinfo("Success", "File uploaded successfully.")
            self.transfer_state = "idle"
            self.progress_bar['value'] = 0
            self.list_remote_files()
            return

        if self.transfer_state.startswith("uploading") and "UPLOAD_ERROR" in line:
            messagebox.showerror("Error", "File upload failed on device.")
            self.transfer_state = "idle"
            self.progress_bar['value'] = 0
            return

        # Handle download
        if self.transfer_state == "downloading" and "DOWNLOAD_START" in line:
            self.download_data = ""
            return
        
        if self.transfer_state == "downloading" and "DOWNLOAD_END" in line:
            try:
                decoded_content = base64.b64decode(self.download_data)
                with open(self.download_file_path, 'wb') as f:
                    f.write(decoded_content)
                messagebox.showinfo("Success", f"File downloaded to {self.download_file_path}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to decode or save file: {e}")
            self.transfer_state = "idle"
            self.progress_bar['value'] = 0
            return
            
        if self.transfer_state == "downloading":
            self.download_data += line
            # Update progress bar if you can get file size
            return

        # Handle config loading
        if self.transfer_state == "loading_config" and "{" in line:
            try:
                config_data = json.loads(line)
                for key, value in config_data.items():
                    if key in self.config_vars:
                        self.config_vars[key].set(value)
                self.transfer_state = "idle"
                self.log_to_terminal("Configuration loaded.\n")
            except json.JSONDecodeError:
                self.log_to_terminal("Error decoding config JSON\n")
            return


        self.log_to_terminal(line + "\n")

    def list_remote_files(self):
        if not (self.serial_connection and self.serial_connection.is_open):
            messagebox.showerror("Error", "Not connected to a device.")
            return
        
        # Clear the treeview
        for item in self.file_tree.get_children():
            self.file_tree.delete(item)
            
        self.serial_connection.write("ls\n".encode('utf-8'))

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
            
            self.progress_bar['maximum'] = len(self.upload_content)
            self.progress_bar['value'] = 0

            self.transfer_state = "uploading_wait_ready"
            self.serial_connection.write(f"upload {remote_path}\n".encode('utf-8'))
            self.log_to_terminal(f"--- Starting upload of {local_path} to {remote_path} ---\n")

        except Exception as e:
            messagebox.showerror("Error", f"Could not read file: {e}")

    def send_upload_chunk(self):
        if self.upload_index < len(self.upload_content):
            chunk = self.upload_content[self.upload_index]
            self.serial_connection.write(chunk + b'\n')
            self.upload_index += 1
            self.progress_bar['value'] = self.upload_index
        else:
            self.serial_connection.write(b"UPLOAD_END\n")
            self.transfer_state = "idle" # Wait for confirmation
            self.log_to_terminal("--- File content sent, waiting for device confirmation... ---\n")

    def download_file_gui(self):
        selected_item = self.file_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No file selected.")
            return
            
        remote_filename = self.file_tree.item(selected_item, "text")
        
        if not (self.serial_connection and self.serial_connection.is_open):
            messagebox.showerror("Error", "Not connected to a device.")
            return

        save_path = filedialog.asksaveasfilename(initialfile=remote_filename)
        if not save_path:
            return

        self.download_file_path = save_path
        self.transfer_state = "downloading"
        self.serial_connection.write(f"download {remote_filename}\n".encode('utf-8'))
        self.log_to_terminal(f"Requesting download of {remote_filename}...\n")

    def load_config_from_device(self):
        if not (self.serial_connection and self.serial_connection.is_open):
            messagebox.showerror("Error", "Not connected to a device.")
            return
        self.transfer_state = "loading_config"
        self.serial_connection.write("get_config\n".encode('utf-8'))

    def save_config_to_device(self):
        if not (self.serial_connection and self.serial_connection.is_open):
            messagebox.showerror("Error", "Not connected to a device.")
            return
        
        try:
            for key, var in self.config_vars.items():
                command = f"set {key} {var.get()}\n"
                self.serial_connection.write(command.encode('utf-8'))
                self.log_to_terminal(f"> {command}")
            
            # After setting all values, send the `save` command
            self.serial_connection.write("save\n".encode('utf-8'))
            self.log_to_terminal("> save\n")
            messagebox.showinfo("Success", "Configuration sent to device.")

        except Exception as e:
            messagebox.showerror("Error", f"Failed to save configuration: {e}")

if __name__ == "__main__":
    app = App()
    app.mainloop()