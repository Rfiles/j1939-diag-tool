#!/usr/bin/env python3

"""
File Manager for the J1939 Diagnostic Tool over Serial or WiFi (HTTP)
"""

import argparse
import serial
import time
import base64
import os
import sys

try:
    import requests
except ImportError:
    print("The 'requests' library is required for HTTP mode. Please install it using: pip install requests")
    sys.exit(1)

# --- Serial Functions ---

def serial_upload(serial_port, local_path, remote_path):
    # ... (implementation from before)

def serial_download(serial_port, remote_path, local_path):
    # ... (implementation from before)

# --- HTTP Functions ---

def http_list_files(ip):
    try:
        response = requests.get(f"http://{ip}/files", timeout=5)
        response.raise_for_status()
        print("Files on device:")
        for file_info in response.json()['files']:
            print(f"  - {file_info['name']} ({file_info['size']} bytes)")
    except Exception as e:
        print(f"Error listing files: {e}")

def http_download(ip, remote_path, local_path):
    print(f"Downloading '{remote_path}' from http://{ip} to '{local_path}'...")
    try:
        response = requests.get(f"http://{ip}/download?path={remote_path}", timeout=10, stream=True)
        response.raise_for_status()
        with open(local_path, 'wb') as f:
            for chunk in response.iter_content(chunk_size=8192):
                f.write(chunk)
        print(f"Success: File saved to '{local_path}'.")
    except Exception as e:
        print(f"Error downloading file: {e}")

def http_upload(ip, local_path, remote_path):
    if not os.path.exists(local_path):
        print(f"Error: Local file not found at '{local_path}'")
        return
    print(f"Uploading '{local_path}' to '{remote_path}' on http://{ip}...")
    try:
        with open(local_path, 'rb') as f:
            files = {'upload': (os.path.basename(remote_path), f, 'application/octet-stream')}
            response = requests.post(f"http://{ip}/upload", files=files, timeout=30)
            response.raise_for_status()
            print(f"Success: {response.text}")
    except Exception as e:
        print(f"Error uploading file: {e}")

# --- Main Execution ---

def main():
    parser = argparse.ArgumentParser(description="File manager for J1939 Diag Tool.")
    parser.add_argument("-p", "--protocol", choices=['serial', 'http'], default='serial', help="The protocol to use.")
    parser.add_argument("--port", help="Serial port (if using serial protocol)")
    parser.add_argument("--ip", help="IP address of the device (if using http protocol)")
    subparsers = parser.add_subparsers(dest="command", required=True)

    # List command (HTTP only)
    subparsers.add_parser("list", help="List files on the device (HTTP only)")

    # Upload command
    parser_upload = subparsers.add_parser("upload", help="Upload a file to the device")
    parser_upload.add_argument("local_path", help="Path of the local file to upload")
    parser_upload.add_argument("remote_path", help="Destination path on the device")

    # Download command
    parser_download = subparsers.add_parser("download", help="Download a file from the device")
    parser_download.add_argument("remote_path", help="Path of the file on the device")
    parser_download.add_argument("local_path", help="Local destination path")

    args = parser.parse_args()

    if args.protocol == 'serial':
        if not args.port:
            print("Error: --port is required for serial protocol")
            return
        try:
            with serial.Serial(args.port, 115200, timeout=10) as ser:
                # ... (serial connection logic)
                if args.command == "upload":
                    serial_upload(ser, args.local_path, args.remote_path)
                elif args.command == "download":
                    serial_download(ser, args.remote_path, args.local_path)
        except serial.SerialException as e:
            print(f"Error opening serial port {args.port}: {e}")

    elif args.protocol == 'http':
        if not args.ip:
            print("Error: --ip is required for http protocol")
            return
        if args.command == "list":
            http_list_files(args.ip)
        elif args.command == "upload":
            http_upload(args.ip, args.local_path, args.remote_path)
        elif args.command == "download":
            http_download(args.ip, args.remote_path, args.local_path)

if __name__ == "__main__":
    main()