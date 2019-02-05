from machine import Pin, SPI


class HSPIMaster:

    def __init__(self, baud=8000000, noc=2):
        self.no_of_counters = noc
        self.hspi = SPI(1, baudrate=baud, polarity=0, phase=0)
        self.buf = bytearray(1 + self.no_of_counters)

    def receive_counters(self):
        self.hspi.readinto(self.buf, 0xff)
        stm_data = []
        counter_value = 0
        i = 0
        j = 0
        while i <= self.no_of_counters:
            while j < 1 + 4 * i:
                counter_value <<= 8
                counter_value |= self.buf[j]
                j += 1
            stm_data.append(counter_value)
            counter_value = 0
            i += 1
        return stm_data
