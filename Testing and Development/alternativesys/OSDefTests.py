import platform

os_name = platform.system()

if os_name == "Windows":
    print("Running on Windows")
elif os_name == "Darwin": # macOS is reported as 'Darwin' by platform.system()
    print("Running on macOS")
elif os_name == "Linux":
    print("Running on Linux")
else:
    print(f"Unknown operating system: {os_name}")