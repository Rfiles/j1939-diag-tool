#!/usr/bin/env python3

"""
License Key Generator for the J1939 Diagnostic Tool

This script replicates the license key generation logic from the firmware
to create valid, signed license keys for a specific hardware ID.

✦ Vou executar o script.
 ╭─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────╮
 │ ✓  Shell python3 /home/rrfiles/j1939_rfiles/j1939_diagnostic_tool/generate_license.py --hw-id "A8:03:2A:B4:5C:6D" --uses 500 --features 255             │
 │                                                                                                                                                         │
 │    --- J1939 Diagnostic Tool License Generator ---                                                                                                      │
 │      Hardware ID:  A8:03:2A:B4:5C:6D                                                                                                                    │
 │      Uses:         500                                                                                                                                  │
 │      Features:     255                                                                                                                                  │
 │      Issue Date:   649 (days since 2024-01-01)                                                                                                          │
 │                                                                                                                                                         │
 │    --------------------------------------------------                                                                                                   │
 │      Generated License Key (12 chars): 9AH/iQIZIUmC                                                                                                     │
 │    --------------------------------------------------                                                                                                   │
 ╰─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────╯
✦ Gerei a chave de licença com sucesso.

"""

import hmac
import hashlib
import datetime
import base64
import argparse
import random

# IMPORTANT: This secret key MUST match the one in `src/core/license_handler.cpp`
HMAC_SECRET_KEY = b"A_DIFFERENT_SECRET_KEY_FOR_V2.7_LICENSES"

# The epoch start date must also match the firmware
EPOCH_START = datetime.datetime(2024, 1, 1, tzinfo=datetime.timezone.utc)

def generate_truncated_signature(hw_id: str, uses: int, features: int, issue_date: int) -> int:
    """Generates the 21-bit HMAC-SHA256 signature."""
    data_to_sign = f"{hw_id}:{uses}:{features}:{issue_date}".encode('utf-8')
    
    # Generate the full signature
    h = hmac.new(HMAC_SECRET_KEY, data_to_sign, hashlib.sha256)
    full_signature = h.digest() # 32 bytes
    
    # Truncate to 21 bits, matching the C++ logic
    # (full_signature[0] << 13) | (full_signature[1] << 5) | (full_signature[2] >> 3)
    truncated_sig = (full_signature[0] << 13) | (full_signature[1] << 5) | (full_signature[2] >> 3)
    return truncated_sig

def pack_data(uses: int, features: int, issue_date: int, padding: int, signature: int) -> bytearray:
    """Packs the license data into a 9-byte array using bitwise operations."""
    buffer = bytearray(9)
    
    # Pack uses (16 bits)
    buffer[0] = uses & 0xFF
    buffer[1] = (uses >> 8) & 0xFF
    
    # Pack features (8 bits)
    buffer[2] = features
    
    # Pack issue_date (15 bits)
    buffer[3] = issue_date & 0xFF
    buffer[4] = (issue_date >> 8) & 0x7F

    # Pack signature (21 bits) and padding (11 bits) into the remaining 4 bytes
    combined = (signature << 11) | (padding & 0x7FF)
    buffer[5] = combined & 0xFF
    buffer[6] = (combined >> 8) & 0xFF
    buffer[7] = (combined >> 16) & 0xFF
    buffer[8] = (combined >> 24) & 0xFF

    return buffer

def main():
    epilog_text = """
    **License Key Structure Example**

    The 12-character license key is a Base64 encoding of a 9-byte (72-bit) data structure.
    Here is the bit-level layout:

    | Bytes | Bits (Size) | Field        | Description                                  |
    |-------|-------------|--------------|----------------------------------------------|
    | 0-1   | 0-15 (16)   | Uses         | Number of uses granted (0-65535).            |
    | 2     | 16-23 (8)   | Features     | Bitmask of enabled features.                 |
    | 3-4   | 24-38 (15)  | Issue Date   | Days since 2024-01-01. Used for activation deadline. |
    | 5-8   | 39-71 (33)  | Combined     | A combination of signature and padding.      |
    |       |  - 39-49 (11)|   - Padding  | Random data to obfuscate the key.            |
    |       |  - 50-71 (21)|   - Signature| HMAC-SHA256 signature (truncated).           |

    The signature is calculated based on the Hardware ID, Uses, Features, and Issue Date,
    ensuring the key is tied to a specific device and its licensed content.
    """
    parser = argparse.ArgumentParser(
        description="Generate a license key for the J1939 Diagnostic Tool.",
        epilog=epilog_text,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("--hw-id", required=True, help="The hardware ID (MAC Address) of the device.")
    parser.add_argument("--uses", type=int, required=True, help="Number of uses to grant (e.g., 1000).")
    parser.add_argument("--features", type=int, default=1, help="Feature bitmask (e.g., 1 for Telemetry, 3 for Telemetry + J1939_PRO).")
    
    args = parser.parse_args()

    # 1. Calculate issue date
    now = datetime.datetime.now(datetime.timezone.utc)
    issue_date = (now - EPOCH_START).days

    # 2. Generate signature
    signature = generate_truncated_signature(args.hw_id, args.uses, args.features, issue_date)

    # 3. Generate random padding
    padding = random.randint(0, 0x0FFF)

    # 4. Pack all data into a 9-byte buffer
    packed_buffer = pack_data(args.uses, args.features, issue_date, padding, signature)

    # 5. Base64 encode the buffer
    license_key = base64.b64encode(packed_buffer).decode('utf-8')

    print("--- J1939 Diagnostic Tool License Generator ---")
    print(f"  Hardware ID:  {args.hw_id}")
    print(f"  Uses:         {args.uses}")
    print(f"  Features:     {args.features}")
    print(f"  Issue Date:   {issue_date} (days since {EPOCH_START.date()})")
    print("\n--------------------------------------------------")
    print(f"  Generated License Key (12 chars): {license_key}")
    print("--------------------------------------------------")

if __name__ == "__main__":
    main()
