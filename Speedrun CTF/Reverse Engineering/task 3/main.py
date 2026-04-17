import curses
import time
import sys
from random import randint
import hashlib
import base64


def sha256_key(passphrase):
    return hashlib.sha256(passphrase.encode()).digest()

def rc4(key, data):
    S = list(range(256))
    j = 0
    for i in range(256):
        j = (j + S[i] + key[i % len(key)]) % 256
        S[i], S[j] = S[j], S[i]
    i = j = 0
    out = bytearray()
    for byte in data:
        i = (i + 1) % 256
        j = (j + S[i]) % 256
        S[i], S[j] = S[j], S[i]
        out.append(byte ^ S[(S[i] + S[j]) % 256])
    return bytes(out)

def rc4_keystream_byte_at(key, index):
    if index < 0:
        raise ValueError("index must be >= 0")

    S = list(range(256))
    j = 0
    for i in range(256):
        j = (j + S[i] + key[i % len(key)]) % 256
        S[i], S[j] = S[j], S[i]

    i = 0
    j = 0
    ks_byte = 0
    for _ in range(index + 1):
        i = (i + 1) % 256
        j = (j + S[i]) % 256
        S[i], S[j] = S[j], S[i]
        ks_byte = S[(S[i] + S[j]) % 256]

    return ks_byte

def decrypt_char(cipher_raw, index, passphrase="7anach"):
    key = sha256_key(passphrase)
    if index < 0 or index >= len(cipher_raw):
        return ''

    byte = cipher_raw[index] ^ key[index % 32]
    stream_byte = rc4_keystream_byte_at(key[:16], index)
    return chr(byte ^ stream_byte)

ENCRYPTED_FLAG = "p77/jK4EXzWUyuxdwiRY1BHAYsLK+4kx/bO3U6zPKbLjLvQL4P8Rs4q4SsHQxGN/eDYydgFdPCBkkmqoMcyxAqvdIWnuudCIa1fcyLvoEjzJPg=="
ENCRYPTED_RAW = base64.b64decode(ENCRYPTED_FLAG)

PREFIX = "Cybersphere{"


def safe_addstr(screen, y, x, text):
    max_y, max_x = screen.getmaxyx()
    if y >= max_y or x >= max_x:
        return
    allowed = max_x - x - 1
    if allowed <= 0:
        return
    screen.addstr(y, x, text[:allowed])


class Field:
    def __init__(self, size):
        self.size = size
        self.icons = {0: ' . ', 1: ' * ', 2: ' # ', 3: ' & '}
        self.snake_coords = []
        self._generate_field()
        self.add_entity()

    def add_entity(self):
        while True:
            i, j = randint(0, self.size-1), randint(0, self.size-1)
            if [i, j] not in self.snake_coords:
                self.field[i][j] = 3
                break

    def _generate_field(self):
        self.field = [[0]*self.size for _ in range(self.size)]

    def _clear_field(self):
        self.field = [[c if c not in (1,2) else 0 for c in row] for row in self.field]

    def render(self, screen, collected_flag):
        self._clear_field()
        for i, j in self.snake_coords:
            self.field[i][j] = 1
        head = self.snake_coords[-1]
        self.field[head[0]][head[1]] = 2

        for i in range(self.size):
            row = ''.join(self.icons[self.field[i][j]] for j in range(self.size))
            safe_addstr(screen, i, 0, row)

        safe_addstr(screen, self.size+1, 0, "FLAG: " + collected_flag)

    def get_entity_pos(self):
        for i in range(self.size):
            for j in range(self.size):
                if self.field[i][j] == 3:
                    return [i, j]
        return [-1, -1]

    def is_snake_eat_entity(self):
        return self.get_entity_pos() == self.snake_coords[-1]


class Snake:
    def __init__(self):
        self.direction = curses.KEY_RIGHT
        self.coords = [[0,0],[0,1],[0,2],[0,3]]
        self.collected_flag = ""
        self.char_index = 0

    def set_direction(self, ch):
        opp = {curses.KEY_LEFT: curses.KEY_RIGHT, curses.KEY_RIGHT: curses.KEY_LEFT,
               curses.KEY_UP: curses.KEY_DOWN, curses.KEY_DOWN: curses.KEY_UP}
        if opp.get(ch) != self.direction:
            self.direction = ch

    def level_up(self):
        a, b = self.coords[0][:], self.coords[1]
        if a[0] < b[0]:   a[0] -= 1
        elif a[1] < b[1]: a[1] -= 1
        elif a[0] > b[0]: a[0] += 1
        elif a[1] > b[1]: a[1] += 1
        a = self._wrap(a)
        self.coords.insert(0, a)

    def _wrap(self, p):
        s = self.field.size
        p[0] = p[0] % s
        p[1] = p[1] % s
        return p

    def move(self):
        head = self.coords[-1][:]
        if   self.direction == curses.KEY_UP:    head[0] -= 1
        elif self.direction == curses.KEY_DOWN:  head[0] += 1
        elif self.direction == curses.KEY_RIGHT: head[1] += 1
        elif self.direction == curses.KEY_LEFT:  head[1] -= 1
        head = self._wrap(head)

        del self.coords[0]
        self.coords.append(head)
        self.field.snake_coords = self.coords

        if head in self.coords[:-1]:
            return "DEAD"

        if self.field.is_snake_eat_entity():
            curses.beep()
            self.level_up()
            if self.char_index < len(ENCRYPTED_RAW):
                self.collected_flag += decrypt_char(ENCRYPTED_RAW, self.char_index)
                self.char_index += 1
            if self.collected_flag == PREFIX:
                return "EXIT"
            self.field.add_entity()
        return "OK"

    def set_field(self, field):
        self.field = field


def main(screen):
    screen.timeout(0)
    curses.curs_set(0)

    max_y, max_x = screen.getmaxyx()
    grid = 10
    field = Field(grid)
    snake = Snake()
    snake.set_field(field)

    while True:
        ch = screen.getch()
        if ch != -1:
            snake.set_direction(ch)

        result = snake.move()

        if result == "DEAD":
            screen.clear()
            safe_addstr(screen, 0, 0, "GAME OVER!")
            safe_addstr(screen, 1, 0, f"Flag: {snake.collected_flag}")
            screen.refresh(); time.sleep(3); sys.exit()

        if result == "EXIT":
            screen.clear()
            safe_addstr(screen, 0, 0, "REALLY BRO?")
            safe_addstr(screen, 1, 0, f"Flag: {snake.collected_flag}")
            screen.refresh(); time.sleep(4); sys.exit()

        field.render(screen, snake.collected_flag)
        screen.refresh()
        time.sleep(0.35)


if __name__ == '__main__':
    curses.wrapper(main)