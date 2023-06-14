Import("env")

def after_upload(source, target, env):
    port = env.GetProjectOption("monitor_port")
    print("waiting for " + port + " ...")
    import serial
    while True:
        try:
            s = serial.Serial(port)
            break
        except:
            pass

env.AddPostAction("upload", after_upload)


# Custom HEX from ELF
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", "-R", ".eeprom", 
        '"$BUILD_DIR/${PROGNAME}.elf"', '"$BUILD_DIR/${PROGNAME}.hex"'
    ]), "Building $BUILD_DIR/${PROGNAME}.hex")
)