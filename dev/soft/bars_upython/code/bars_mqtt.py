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
        if resp is None:
            raise TimeoutError
        else:
            return resp

    def __send_msg(self, topic, msg):
        self.c.publish(topic, msg)

    def req_interval(self, batt):
        self.c.publish(b"interval", b"pls")
        self.c.publish(b"batt", batt)
        i = 0
        while i < 3:
            try:
                resp = self.__wait()
                return int(resp[1].decode('utf-8'))
            except TimeoutError:
                continue
        return None

    def send_counters(self, counters, batt):
        for topic, msg in counters:
            self.__send_msg(topic, msg)
        self.c.publish(b"batt", batt)
        resp = self.c.wait_msg()
        return int(resp[1].decode('utf-8'))
