# Pysomware CTF Challenge – Write-up

## Challenge Overview

**Name:** Pysomware
**Category:** Forensics
**Author** : sn0_0wyy(shoot me a dm if you're stuck on some part)

### Description

Our friend `z3d_r34p3r` got his files encrypted after trusting a “Windows Activator” from Discord. He tried multiple decryption tools with no success. He claims that if you manage to decrypt his files, you’ll find what you’re looking for.

---

## 🧪 Initial Analysis

After extracting the provided archive and performing basic triage, we find a suspicious executable:

```
Windows_Activator.exe
```

located in the victim’s **Downloads** folder.

Running `strings` on the binary reveals indicators that it is a **Python-based executable** (presence of `.pyz`, Python DLLs, etc.).

---

## 🐍 Extracting the Malware

Since the binary is built with PyInstaller, we can extract it using:

* **pyinstxtractor**: https://github.com/extremecoders-re/pyinstxtractor
* Then decompile using **PyLingual**: https://pylingual.io/

You can follow a similar approach from this write-up:
https://ctftime.org/writeup/29575

The relevant archive is typically named:

```
locker.pyz
```

---

## 🔐 Understanding the Encryption Logic

After reversing the code, we determine that the malware is a **ransomware** that generates its encryption key dynamically at runtime.

### Key derivation depends on:

* UID (activation code)
* Computer Name
* Execution timestamp (epoch)

---

## 💬 Recovering the UID (Discord Cache)

The challenge description hints at Discord, so we investigate cached data.

### Tool:

* ChromeCacheView: https://www.nirsoft.net/utils/chromecacheview.zip

### Guide:

https://abrignoni.blogspot.com/2018/03/finding-discord-app-chats-in-windows.html

From the cache, we recover a conversation between:

* Attacker: `zyyz_godzilla_enjoyer`
* Victim: **BAMBO**

The attacker instructs:

> “Copy it fast, I’m going to delete it.”

This confirms the presence of a **temporary activation code (UID)**.

---

## 📋 Recovering Clipboard Data

Since the victim copied the code, we can retrieve it from Windows activity artifacts.

### Reference:

https://www.inversecos.com/2022/05/how-to-perform-clipboard-forensics.html

### Steps:

1. Locate `ActivitiesCache.db`
2. Open with SQLite viewer
3. Navigate to:

   ```
   Browse Data → SmartLookup
   ```
4. Filter by `ClipboardPayload`

### Result:

```
FJH38-9JK23-3T3YT-3R4FJ-BJQBF
```

---

## 🖥️ Extracting Computer Name

From the SYSTEM registry hive using **Registry Explorer**:

```
HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\ComputerName\ComputerName
```

### Result:

```
DESKTOP-6F1LGKO
```

---

## ⏱️ Determining Execution Time

We analyze Prefetch files:

```
prefetcher.py -f WINDOWS_ACTIVATOR.EXE-5F35ECB2.pf
```

### Output:

```
Run Time: 2026-03-06 21:34:00
Run Time: 2026-03-06 21:34:01
```

Converted to epoch:

```
1772829241
```

---

## ⚠️ Important Insight

The malware **does not encrypt immediately**.
It waits for the user to input the UID.

➡️ Therefore, we must brute-force a small time window.

Correct timestamp:

```
1772829242
```

---

## 🔓 Decryption

Encrypted file:

```
flag.png.enc
```

We brute-force the timestamp within a small range and check for PNG magic bytes:

```
89 50 4E 47 0D 0A 1A 0A
```

---

## 🧾 Decryption Script

```python
import hashlib
import pathlib
import re

UID_PATTERN = re.compile(r"^[A-Z0-9]{5}(?:-[A-Z0-9]{5}){4}$")

def normalize_uid(uid: str) -> str:
    return uid.strip().upper().replace(" ", "")

def derive_key(uid: str, exec_time: str, computer: str) -> bytes:
    material = f"{uid}|{exec_time}|{computer.upper()}"
    return hashlib.sha256(material.encode()).digest()

def keystream(key: bytes):
    counter = 0
    while True:
        ctr_bytes = counter.to_bytes(8, "little", signed=False)
        yield hashlib.sha256(key + ctr_bytes).digest()
        counter += 1

def rotate_left(b: int, r: int) -> int:
    r &= 7
    return ((b << r) | (b >> (8 - r))) & 0xFF

def rotate_right(b: int, r: int) -> int:
    r &= 7
    return ((b >> r) | (b << (8 - r))) & 0xFF

def decrypt(data: bytes, key: bytes) -> bytes:
    out = bytearray()
    ks = keystream(key)
    offset = 0
    block_index = 0
    while offset < len(data):
        stream = next(ks)
        rot = (block_index % 7) + 1
        take = min(len(stream), len(data) - offset)
        chunk = data[offset:offset + take]
        for i, b in enumerate(chunk):
            if block_index % 2 == 0:
                out.append(rotate_left(b, rot) ^ stream[i])
            else:
                out.append(rotate_right(b, rot) ^ stream[i])
        offset += take
        block_index += 1
    return bytes(out)


cwd = pathlib.Path.cwd()
enc_file = cwd / "flag.png.enc"
output_file = cwd / "flag.png"

if not enc_file.exists():
    print("[!] flag.png.enc not found in current directory")
    exit()



uid = "FJH38-9JK23-3T3YT-3R4FJ-BJQBF"
exec_time = 1772832841
computer = "DESKTOP-6F1LGKO"

PNG_SIGNATURE = b'\x89PNG\r\n\x1a\n'

cipher = enc_file.read_bytes()

for i in range(1,10):
    key = derive_key(uid, str(exec_time + i), computer)
    plain = decrypt(cipher, key)

    if plain.startswith(PNG_SIGNATURE):
        print(f"[+] Correct exec_time: {exec_time + i}")
        output_file.write_bytes(plain)
        print(f"[+] Decrypted → {output_file}")
        break
else:
    print("[!] No valid key found")
```
Enc flag is inside Desktop\realdeal\flag.png.enc

---

## 🏁 Final Result

After successful decryption:

```
Flag: C3rt1fied_Pyth0n_Mal_An4lyst
```

### ✅ Final Flag:

```
CyberSphere{C3rt1fied_Pyth0n_Mal_An4lyst}
```

