
class CRC:

    def crc(data):
        CRC = 0x00
        for i in data:
            CRC = CRC ^ i
        return CRC