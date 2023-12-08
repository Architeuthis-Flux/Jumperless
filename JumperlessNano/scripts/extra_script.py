Import("env")
#import serial


#env = DefaultEnvironment()

def find_jumperless_port_monitor(source, target, env):
    
    import serial.tools.list_ports
    #print(env.dump())
    counter = 0
    while True:
        autodetected = -1
        ports = serial.tools.list_ports.comports()
        i = 0

        for port, desc, hwid in ports:
            
            #print("{}: {} [{}]".format(i, port, desc))
            
            if desc == "Jumperless":
                autodetected = i
                print("Autodetected jumperless port: " + ports[autodetected][0])
                
            i = i + 1

        if autodetected != -1:

            selection = autodetected
            env.Replace(MONITOR_PORT=ports[selection][0])
            #env.Replace(UPLOAD_PORT=ports[selection][0])
            #env.Replace("monitor_port", ports[selection][0])
            #env.ConfigEnvOption(env, "monitor_port", ports[selection][0])
            print("Autodetected jumperless port: " + ports[selection][0])

            return ports[selection][0]
        counter = counter + 1
        if counter > 30000:
            #print("timeout")
            return 'timeout'
        
        


def after_upload(source, target, env):
    #port = env.GetProjectOption("monitor_port")
    port = find_jumperless_port_monitor(source, target, env)

    print("waiting for " + port + " ...")
    import serial
    counter = 0
    while True:
        try:
            s = serial.Serial(port)
            #env.dump()
            break
        except:
            pass
        counter = counter + 1
        if counter > 30000:
            print("timeout")
            break


env.AddPostAction("upload", after_upload)


# Custom HEX from ELF
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", "-R", ".eeprom", 
        '"$BUILD_DIR/${PROGNAME}.elf"', '"$BUILD_DIR/${PROGNAME}.hex"'
    ]), "Building $BUILD_DIR/${PROGNAME}.hex")
)