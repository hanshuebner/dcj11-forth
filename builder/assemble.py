import pexpect
import time
import re
import sys

# Start simh PDP-11 emulator
child = pexpect.spawn('pdp11', encoding='utf-8', timeout=10)

# Create a buffer to store output
capturing = False
current_line = ""

# Custom logfile class to handle output
class OutputHandler:
    def write(self, data):
        global capturing, current_line
        # Remove CR characters
        data = data.replace('\r', '')
        
        # Accumulate characters into current_line
        current_line += data
        
        # Check for newline to process complete lines
        if '\n' in current_line:
            lines = current_line.split('\n')
            # Process all complete lines except the last one
            for line in lines[:-1]:
                # Check for start marker
                if "RY: buffering file in memory" in line:
                    capturing = True
                    continue
                    
                # Check for end marker
                if "Simulation stopped" in line:
                    capturing = False
                    continue
                    
                # If we're capturing, print immediately
                if capturing:
                    print(line)
            
            # Keep the last incomplete line
            current_line = lines[-1]
            
    def flush(self):
        pass

# Set up the custom output handler
child.logfile = OutputHandler()

# Wait for SIMH prompt
child.expect("sim>")

# Load the ini file explicitly
child.sendline("do pdp11.ini")

# Function to wait for a dot prompt followed by ~300ms of inactivity
def wait_for_dot_idle_prompt():
    buffer = ""
    dot_seen = False
    last_output_time = time.time()
    while True:
        try:
            chunk = child.read_nonblocking(size=1, timeout=0.3)
            buffer += chunk
            if chunk == '.':
                dot_seen = True
                last_output_time = time.time()
            else:
                # If we see any output after the dot, reset both flags
                dot_seen = False
                last_output_time = time.time()
        except pexpect.exceptions.TIMEOUT:
            # Only return true if we've seen a dot and no output for 0.3 seconds
            if dot_seen and time.time() - last_output_time > 0.3:
                return True
        except pexpect.exceptions.EOF:
            print("Simulator exited unexpectedly")
            return False

# --- First prompt ---
wait_for_dot_idle_prompt()

# --- Assemble forth
child.sendline("@DY:FORTH.COM")

# --- Wait for second prompt ---
wait_for_dot_idle_prompt()

# --- Send Ctrl-E (to exit the simulation) ---
child.send('\x05')  # Ctrl-E

# Wait for SIMH to cleanly exit
child.expect(pexpect.EOF)
