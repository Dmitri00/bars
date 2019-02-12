from bars_hspi_master import HSPIMaster
from bars_mqtt import BarsClient


hspi = HSPIMaster()
mqtt = BarsClient("ESP", "192.168.1.100")
