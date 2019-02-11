from umqtt.simple import MQTTClient
import utime as time


class BarsClient:

    def __init__(self, name, ip):
        self.c = MQTTClient(name, ip)
        self.c.connect(lambda topic, msg: print((topic, msg)))
        self.c.subscribe(b"int_resp")

    def __wait(self):
        resp = self.c.check_msg()
        if resp is None:
            pass
        else:
            return resp
        time.sleep(5)
        resp = self.c.check_msg()
        if resp is None:
            raise TimeoutError
        else:
            return resp

    def __send_msg(self, topic, msg):
        self.c.publish(topic, msg)

    def req_interval(self, batt):
        print("Requesting interval...")
        self.c.publish(b"interval", b"pls")
        self.c.publish(b"batt", batt)
        i = 0
        print("Attempting to receive response...")
        while i < 3:
            try:
                print("Checking response message, attempt No." + str(i+1) + "...")
                resp = self.__wait()
                print("Response received")
                return int(resp[1].decode('utf-8'))
            except TimeoutError:
                i += 1
                continue
        return None

    def send_counters(self, counters, batt):
        print("Sending counters...")
        for topic, msg in counters:
            self.__send_msg(topic, msg)
        self.c.publish(b"batt", batt)
        print("Waiting for response...")
        resp = self.__wait()
        print("Response received:")
        print(resp)
        return int(resp[1].decode('utf-8'))
