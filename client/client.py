## Generate key pair 
## Sign with key id
## Get key by ID : pub and priv
## encrypt/decrypt file with key

from Connector import Connector
from Protocol import Protocol
from CRC import CRC

__TOOL_VERSION__ = '0.1.0'
__TOOL_NAME__ = 'pHSM'
__TOOL_DATE__ = '21.12.2022'


protocol_manager = Protocol()
connector = Connector()

secure_token = None

def get_response():
    data = connector.listen(5)
    if not data:
        print("Time passed")
    return data

def __connection_handler(token, user_tokens):
    global secure_token
    payload = protocol_manager.connect_payload(user_tokens[0], user_tokens[1].encode())
    connector.send(payload)

    resp = get_response()
    if resp == None:
        return
    secure_token = resp[5:32+5]
    if CRC.crc(resp[:len(resp)-1]) != resp[-1]:
        print("CRC wrong")
    
def __get_hash_handler(token, user_tokens):
    if user_tokens[0] == '-f':
        print(user_tokens[1])
        data = open(user_tokens[1], "r").read(-1)
    else:
        data = user_tokens[0]
    payload = protocol_manager.get_hash_payload(token, data.encode())
    connector.send(payload)

    resp = get_response()
    if resp == None:
        return
    print(resp[41:-1].hex())
    if CRC.crc(resp[:len(resp)-1]) != resp[-1]:
        print("CRC wrong")

def __gen_key_handler(token, user_tokens):
    payload = protocol_manager.gen_key_payload(token, b'')
    connector.send(payload)

    resp = get_response()
    if resp == None:
        return
    print(resp[41:-1].hex())
    if CRC.crc(resp[:len(resp)-1]) != resp[-1]:
        print("CRC wrong")

def __enc_handler(token, user_tokens):
    key_id = user_tokens[0]
    if user_tokens[1] == '-f':
        data = open(user_tokens[2], "r").read(-1)
    else:
        data = user_tokens[1]
    payload = protocol_manager.enc_payload(token, int(key_id), data.encode())
    connector.send(payload)

    resp = get_response()
    if resp == None:
        return
    print(resp[41:].hex())
    if CRC.crc(resp[:len(resp)-1]) != resp[-1]:
        print("CRC wrong")

def __get_key_handler(token, user_tokens):
    key_id = user_tokens[0]
    payload = protocol_manager.get_key_payload(token, int(key_id))
    connector.send(payload)

    resp = get_response()
    if resp == None:
        return
    print(resp[41:-1].hex())
    if CRC.crc(resp[:len(resp)-1]) != resp[-1]:
        print("CRC wrong")

__input_handlers = {
    "con": __connection_handler,
    "hash": __get_hash_handler,
    "genk": __gen_key_handler,
    "enc": __enc_handler,
    "getk": __get_key_handler,
}

def main():
    print(f"{__TOOL_NAME__} version {__TOOL_VERSION__} - {__TOOL_DATE__}")


    while (True):
        
        user_input = input(">>")
        user_input_tokens = user_input.split(" ")

        if user_input_tokens[0] == 'exit':
            break

        if user_input_tokens[0] not in __input_handlers:
            print("No such command found!")
            continue

        __input_handlers[user_input_tokens[0]](secure_token, user_input_tokens[1:])

       

if __name__ == "__main__":
    main()