Import("env")


def find_jumperless_port_upload(source, target, env):
    import serial
    import serial.tools.list_ports

    autodetected = -1
    ports = serial.tools.list_ports.comports()
    i = 0
    for port, desc, hwid in ports:
        
        print("{}: {} [{}]".format(i, port, desc))
        
        if desc == "Jumperless":
            autodetected = i
        i = i + 1

    if autodetected != -1:

        selection = autodetected
        env.Replace(MONITOR_PORT=ports[selection][0])
        env.Replace(UPLOAD_PORT=ports[selection][0])
        #(env, "monitor_port", ports[selection][0])
       # ConfigEnvOption(env, "upload_port", ports[selection][0])
        print("Autodetected jumperless port: " + ports[selection][0])



env.AddPreAction("upload", find_jumperless_port_upload)