import gzip
import re
import os

def main():
    print("Compressing html_page.h...")
    
    file_path = "html_page.h"
    if not os.path.exists(file_path):
        print(f"Error: {file_path} not found!")
        return

    with open(file_path, "r", encoding="utf-8") as f:
        content = f.read()

    # Search for all const char Name[] PROGMEM = R"rawliteral( ... )rawliteral";
    matches = re.findall(r'const char (\w+)\[\] PROGMEM = R"rawliteral\((.*?)\)rawliteral";', content, re.DOTALL)
    if not matches:
        print("Error: Could not find any rawliteral blocks in html_page.h!")
        return

    header_content = """#pragma once
#include <Arduino.h>

"""
    
    for name, html_content in matches:
        html_content = html_content.strip()
        html_bytes = html_content.encode("utf-8")
        compressed = gzip.compress(html_bytes, compresslevel=9)
        length = len(compressed)
        
        # Format as C++ hex byte array
        hex_bytes = []
        for b in compressed:
            hex_bytes.append(f"0x{b:02x}")
        
        # Format with beautiful line wraps (16 bytes per line)
        formatted_lines = []
        current_line = []
        for idx, hb in enumerate(hex_bytes):
            current_line.append(hb)
            if len(current_line) == 16 or idx == len(hex_bytes) - 1:
                formatted_lines.append("    " + ", ".join(current_line) + ("," if idx < len(hex_bytes) - 1 else ""))
                current_line = []

        array_content = "\n".join(formatted_lines)
        header_content += f"const uint32_t {name}_gz_len = {length};\n"
        header_content += f"const uint8_t {name}_gz[] PROGMEM = {{\n{array_content}\n}};\n\n"
        print(f"Compressed {name}: Original: {len(html_bytes)} bytes, Compressed: {length} bytes.")

    with open("html_page_gz.h", "w", encoding="utf-8") as f:
        f.write(header_content)

    print("Successfully generated html_page_gz.h!")

if __name__ == "__main__":
    main()
