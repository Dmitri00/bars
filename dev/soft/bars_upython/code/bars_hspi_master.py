from machine import Pin, SPI


class HSPIMaster:

    def __init__(self, baud=115200, noc=2):
        self.num_of_counters = noc
        self.hspi = SPI(1, baudrate=baud, polarity=0, phase=0)
        self.buf = bytearray(1 + 4 * self.num_of_counters)

    def receive_counters(self):

        print("Reading data from SPI...")

        self.hspi.readinto(self.buf, 0xff)

        print("Processing received data...")

        stm_data = []
        counter_value = 0
        i = 0
        j = 0
        while i <= self.num_of_counters:
            while j < 1 + 4 * i:
                counter_value <<= 8
                counter_value |= self.buf[j]
                j += 1
            stm_data.append(counter_value)
            counter_value = 0
            i += 1

        print("SPI data processed, info received successfully")

        return stm_data
