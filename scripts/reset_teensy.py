Import("env")
import os
import subprocess
import time

TEENSY_EXE = os.path.join(
    os.path.expanduser("~"),
    ".platformio", "packages", "tool-teensy", "teensy.exe"
)

def before_upload(source, target, env):
    # teensy_post_compile (the gui uploader) IPC-messages a running teensy.exe.
    # If that process isn't up yet there's a race: post_compile fires before the
    # GUI is ready and the upload silently fails. We pre-launch it here and give
    # it 3 s to initialize so the race is gone.
    try:
        result = subprocess.run(
            ["tasklist", "/FI", "IMAGENAME eq teensy.exe", "/NH"],
            capture_output=True, text=True
        )
        if "teensy.exe" not in result.stdout:
            if os.path.isfile(TEENSY_EXE):
                print("Pre-launching Teensy Loader GUI...")
                subprocess.Popen([TEENSY_EXE])
                time.sleep(3.0)
            else:
                print(f"Warning: teensy.exe not found at {TEENSY_EXE}")
        else:
            print("Teensy Loader GUI already running")
    except Exception as e:
        print(f"Warning: could not ensure Teensy Loader is running: {e}")

env.AddPreAction("upload", before_upload)
