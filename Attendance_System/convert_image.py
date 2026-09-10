import sys
import os

def print_help():
    print("""
ESP32 Image Converter Utility
-----------------------------
This script converts and resizes images (PNG, JPEG, BMP, etc.) into the exact 
Standard Baseline JPEG format required by the ESP32 TFT_eSPI (TJpgDec) library.

Usage:
  python convert_image.py <input_image> [output_width] [output_height]

Examples:
  1. For Employee Profile Photo (120x120 center-cropped):
     python convert_image.py photo.png 120 120
     (Saves to: photo_converted.jpg)

  2. For Standby Screen Logo / Custom Image:
     python convert_image.py logo.png 320 200
     (Saves to: logo_converted.jpg)

Requirements:
  pip install Pillow
""")

def main():
    if len(sys.argv) < 2 or sys.argv[1] in ["--help", "-h", "/?"]:
        print_help()
        return

    input_path = sys.argv[1]
    if not os.path.exists(input_path):
        print(f"Error: Input file '{input_path}' not found!")
        return

    # Check for Pillow
    try:
        from PIL import Image, ImageOps
    except ImportError:
        print("\nError: The 'Pillow' library is required to run this script.")
        print("Please install it by running the following command in your terminal:")
        print("   pip install Pillow\n")
        return

    # Default sizes
    width = 120
    height = 120

    if len(sys.argv) >= 3:
        try:
            width = int(sys.argv[2])
        except ValueError:
            print(f"Warning: Invalid width '{sys.argv[2]}'. Using default 120.")
            
    if len(sys.argv) >= 4:
        try:
            height = int(sys.argv[3])
        except ValueError:
            print(f"Warning: Invalid height '{sys.argv[3]}'. Using default 120.")

    # Load and process image
    try:
        img = Image.open(input_path)
        print(f"Loaded: {input_path} ({img.size[0]}x{img.size[1]} | Mode: {img.mode})")
        
        # Convert to RGB if it is RGBA (transparent PNG) or Grayscale
        if img.mode != "RGB":
            print(f"Converting color mode from {img.mode} to RGB...")
            # Create a white background for transparent images
            if img.mode in ("RGBA", "LA") or (img.mode == "P" and "transparency" in img.info):
                background = Image.new("RGB", img.size, (255, 255, 255))
                background.paste(img, mask=img.split()[-1] if img.mode == "RGBA" else None)
                img = background
            else:
                img = img.convert("RGB")

        # Center-crop and resize to the requested dimensions
        print(f"Resizing and cropping to {width}x{height}...")
        img_resized = ImageOps.fit(img, (width, height), Image.Resampling.LANCZOS)

        # Generate output path
        base, ext = os.path.splitext(input_path)
        output_path = f"{base}_converted.jpg"

        # Save as Baseline JPEG (non-progressive)
        # optimize=True reduces file size, progressive=False guarantees baseline
        img_resized.save(output_path, "JPEG", quality=85, progressive=False, optimize=True)
        
        file_size_kb = os.path.getsize(output_path) / 1024
        print(f"\nSuccess! Converted image saved to:")
        print(f"👉 {output_path}")
        print(f"Dimensions: {width}x{height}")
        print(f"File Size: {file_size_kb:.2f} KB (Perfect for ESP32 memory)")

    except Exception as e:
        print(f"Error converting image: {e}")

if __name__ == "__main__":
    main()
