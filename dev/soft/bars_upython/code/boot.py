# This file is executed on every boot (including wake-boot from deepsleep)
# import esp
# esp.osdebug(None)
# import uos, machine
# uos.dupterm(None, 1)
# disable REPL on UART(0)
import gc
import webrepl
import json
import network
import time


webrepl.start()
gc.collect()


def try2connect(wlan_obj, aps):
    if not aps:
        print("There are no known networks!")
        return False
    while not wlan_obj.isconnected():
        try:
            ap = next(aps)
            print("Trying to connect to known networks...")
            wlan_obj.connect(ap["ESSID"], ap["Password"])
            time.sleep(1)
        except StopIteration:
            print("Known networks unavailable!")
            return False
    return True


with open("network_conf.json", "r") as f:
    access_points = json.load(f)
    wlan = network.WLAN(network.STA_IF)
    connected = try2connect(wlan, access_points)
if not connected:
    pass
