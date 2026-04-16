const express = require('express');
const http    = require('http');
const { Server } = require('socket.io');
const jwt     = require('jsonwebtoken');
const path    = require('path');

const app    = express();
const server = http.createServer(app);
const io     = new Server(server);

const JWT_SECRET = 'shadow';
const FLAG       = 'Cybersphere{sh4d0w_s0ck3t_jwt_4nd_1nt3l_4p1_pwn3d}';

app.use(express.static(path.join(__dirname, 'public')));

app.get('/api/token/operative', (req, res) => {
  const token = jwt.sign(
    { uid: 'op-7734', handle: 'SPECTRE', role: 'operative', clearance: 1 },
    JWT_SECRET,
    { algorithm: 'HS256', expiresIn: '2h' }
  );
  res.json({
    token,
    issued_at: new Date().toISOString(),
    note: 'Operative-level access only. Escalation requires handler clearance.'
  });
});

app.get('/api/intel', (req, res) => {
  const authHeader = req.headers.authorization;
  if (!authHeader || !authHeader.startsWith('Bearer ')) {
    return res.status(401).json({ error: 'Bearer token required' });
  }

  const rawToken = authHeader.slice(7);

  let decoded;
  try {
    decoded = jwt.verify(rawToken, JWT_SECRET, { algorithms: ['HS256'] });
  } catch (err) {
    if (err.name === 'TokenExpiredError') {
      return res.status(401).json({ error: 'Token expired — re-issue required' });
    }
    return res.status(401).json({ error: 'Invalid token signature' });
  }

  if (decoded.role !== 'handler') {
    return res.status(403).json({
      error: 'Clearance denied',
      required_role: 'handler',
      your_role: decoded.role,
      hint: 'You need handler-level clearance to access this archive.'
    });
  }

  res.json({
    classification: 'TOP SECRET // NOFORN',
    archive: 'SHADOW DESK INTEL — HANDLER EYES ONLY',
    retrieved_at: new Date().toISOString(),
    reports: [
      {
        ref: 'SD-001',
        title: 'OPERATION NIGHTFALL — Status',
        body: 'Asset CARDINAL successfully exfiltrated. Berlin safe house confirmed. Cover intact.'
      },
      {
        ref: 'SD-002',
        title: 'Network Penetration — Eastern Sector',
        body: 'Twelve endpoints compromised via supply-chain vector. Lateral movement ongoing. ETA exfil window: 72 hrs.'
      },
      {
        ref: 'SD-003',
        title: 'DIRECTOR\'S EYES ONLY — Final Extraction Code',
        body: FLAG
      }
    ]
  });
});

const PUBLIC_MSGS = [
  { from: 'DISPATCH', text: 'Comms check active. All operatives report in.' },
  { from: 'SYS',      text: 'Secure channel initialized. Encryption layer: AES-256-GCM.' },
  { from: 'DISPATCH', text: 'No field updates from sectors 4-9. Maintaining standby.' },
  { from: 'RELAY',    text: 'Uplink stable. Satellite window opens in T-minus 04:12.' },
];

io.on('connection', (socket) => {
  socket.on('join', (room) => {
    socket.join(room);

    if (room === 'public_feed') {
      PUBLIC_MSGS.forEach((msg, i) => {
        setTimeout(() => socket.emit('message', msg), i * 900);
      });
    }

    if (room === 'shadow_ops') {
      setTimeout(() => {
        socket.emit('message', {
          from: 'SYS_INTERNAL',
          text: '[WARN] Operative provisioning endpoint still exposed: GET /api/token/operative — rotate credentials immediately'
        });
      }, 400);

      setTimeout(() => {
        socket.emit('message', {
          from: 'SYS_INTERNAL',
          text: '[INFO] Classified intel archive mounted at /api/intel — handler-clearance bearer token required for access'
        });
      }, 1200);
    }
  });
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`[ShadowDesk] Argon Systems internal platform running → http://localhost:${PORT}`);
});
