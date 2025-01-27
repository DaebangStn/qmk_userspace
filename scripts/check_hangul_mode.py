import hid

def find_keyboard():
    # Corne V4 keyboard vendor ID and product ID
    VENDOR_ID = 0x4653
    PRODUCT_ID = 0x0004

    try:
        # Try to open the device directly
        device = hid.Device(VENDOR_ID, PRODUCT_ID)
        print(f'Connected to: {device.manufacturer} {device.product}')
        return device
    except IOError:
        print("Keyboard not found!")
        return None

def send_keyboard_state(state):
    device = find_keyboard()
    if not device:
        return

    try:
        # Send the data (first byte is report ID, following bytes are data)
        device.write([0x00, state])  # 0x00 is report ID, state is 0 or 1
        print(f"Sent state: {'Hangul' if state == 1 else 'English'}")
    except Exception as e:
        print(f"Error sending data: {e}")
    finally:
        device.close()

if __name__ == "__main__":
    # Example usage
    # Send 1 for Hangul mode
    send_keyboard_state(1)

    # Send 0 for English mode
    # send_keyboard_state(0)
