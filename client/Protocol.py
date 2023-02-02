import os
from Configs import ProtocolConfigs
from CRC import CRC


COMMAND_CODES = {
    'connect': 0xAF,
    'hash': 0xBF,
    'genk': 0xCF,
    'enc' : 0xDF,
    'getk' : 0x2F,
}

class Protocol():
    
    def __init__(self):
        self.payload = bytes()
        self.__init_response_parse_handlers()
        self.__init_request_parse_handlers()

    def __init_response_parse_handlers(self):
        self.response_parse_handlers = {
            COMMAND_CODES['connect']:self.__parse_connect_response_payload
        }

    def __init_request_parse_handlers(self):
        self.request_parse_handlers = {
            COMMAND_CODES['connect']:self.__parse_connect_request_payload
        }

    def __clean_payload(self):
        self.payload = bytes()
    
    def connect_payload(self, login, password) -> bytes:

        self.__clean_payload()
        self.__add_command("connect")
        self.__add_nonce()
        self.__add_random_token()
        self.__add_length(len(login) + 1 + len(password) + 1)
        self.__add_param(login.encode() + b"\00")
        self.__add_param(password)
        self.__add_crc()

        return self.payload

    def get_hash_payload(self, token, payload) -> bytes:

        self.__clean_payload()
        self.__add_command("hash")
        self.__add_nonce()
        self.__add_param(token)
        self.__add_length(len(payload) + 1)
        self.__add_param(payload)
        self.__add_crc()

        return self.payload

    def gen_key_payload(self, token, payload) -> bytes:

        self.__clean_payload()
        self.__add_command("genk")
        self.__add_nonce()
        self.__add_param(token)
        self.__add_length(len(payload) + 1)
        self.__add_param(payload)
        self.__add_crc()

        return self.payload

    def enc_payload(self, token, key_id, data) -> bytes:

        key_id = key_id.to_bytes(4, byteorder = 'little')
        self.__clean_payload()
        self.__add_command("enc")
        self.__add_nonce()
        self.__add_param(token)
        self.__add_length(len(key_id) + len(data) + 1)
        self.__add_param(key_id)
        self.__add_param(data)
        self.__add_crc()

        return self.payload

    def get_key_payload(self, token, key_id) -> bytes:

        key_id = key_id.to_bytes(4, byteorder = 'little')
        self.__clean_payload()
        self.__add_command("getk")
        self.__add_nonce()
        self.__add_param(token)
        self.__add_length(len(key_id) + 1)
        self.__add_param(key_id)
        self.__add_crc()

        return self.payload


    def __parse_connect_response_payload(self, payload):
        print("Parse connect response")
        return ()


    def __parse_connect_request_payload(self, payload):
        print("Parse connect request")
        return ()


    def parse_response_payload(self, payload):
        pass

    def parse_request_payload(self, payload):
        command_code, payload = self.__get_command(payload)
        parse_handler = self.request_parse_handlers[command_code]
        return (command_code, parse_handler(payload))


    def __add_command(self, command_name):
        self.payload += COMMAND_CODES[command_name].to_bytes(ProtocolConfigs.COMMAND_SIZE, ProtocolConfigs.BYTE_ORDER)

    def __add_nonce(self):
        self.payload += os.urandom(ProtocolConfigs.NONCE_SIZE)
    
    def __add_random_token(self):
        self.payload += os.urandom(32)
    
    def __add_length(self, length):
        self.payload += length.to_bytes(ProtocolConfigs.LENGTH_SIZE, ProtocolConfigs.BYTE_ORDER)

    def __add_param(self, param):
        self.payload += param
    
    def __add_crc(self):
        counted_payload_crc = CRC.crc(self.payload)
        self.payload += counted_payload_crc.to_bytes(ProtocolConfigs.CRC_SIZE, ProtocolConfigs.BYTE_ORDER)


    def __get_command(self, payload : bytes):
        # Returns (int, bytes)
        return (int.from_bytes(payload[:ProtocolConfigs.COMMAND_SIZE]), payload[ProtocolConfigs.COMMAND_SIZE:])
    
    def __get_nonce(self, payload : bytes):
        # Returns (int, bytes)
        return (int.from_bytes(payload[:ProtocolConfigs.NONCE_SIZE]), payload[ProtocolConfigs.NONCE_SIZE:])
