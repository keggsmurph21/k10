import socket
import struct

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("localhost", 3088))

def request(req: bytes) -> bytes:
    print()
    print(f"    /sending {len(req)} bytes ({req})")
    sock.send(req)
    print(f"    | ...")
    res = sock.recv(4096)
    print(f"    \\received {len(res)} bytes ({res})")
    print()
    return res


# Register a user
res = request(b"\0\7player1\x08password")
res_type, is_present, p1, p1_secret = struct.unpack(">B?QQ", res)
print(f"Registered player id {p1} (secret={hex(p1_secret)})")

# Register a user
res = request(b"\0\7player2\x08password")
res_type, is_present, p2, p2_secret = struct.unpack(">B?QQ", res)
print(f"Registered player id {p2} (secret={hex(p2_secret)})")

# Create a game (as p1)
res = request(struct.pack(">BQQBBBBBQQ", 1, p1, p1_secret, 2, 0, 0, 0, 0, 2, 10))
res_type, game_id = struct.unpack(">BQ", res)
print(f"Created game with id {game_id}")

# Join the game (as p2)
res = request(struct.pack(">BQQQ", 2, p2, p2_secret, game_id))
print(f"Joined game")

# Leave the game (as p2)
res = request(struct.pack(">BQQQ", 3, p2, p2_secret, game_id))
print(f"Left game")

# Join the game (as p2)
res = request(struct.pack(">BQQQ", 2, p2, p2_secret, game_id))
print(f"Joined game")

# Start the game (as p2)
res = request(struct.pack(">BQQQ", 4, p2, p2_secret, game_id))
print(f"Started game")
