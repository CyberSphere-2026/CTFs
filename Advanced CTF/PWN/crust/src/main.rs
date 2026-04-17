use std::io::{self, BufRead, Write};
use std::mem;
use std::process;

const MAX_NODES: usize = 8;
const MAX_SHADOWS: usize = 4;
const MAX_CMDS: usize = 50;
const NODE_SIZE: usize = 144;

#[inline(never)]
#[no_mangle]
pub extern "C" fn nexus_override() {
    let flag = std::fs::read_to_string("./flag").unwrap();
    println!("flag: {}", flag.trim());
    process::exit(0);
}

trait Node {
    fn inspect(&self);
    fn activate(&self);
    fn write_data(&mut self, offset: usize, data: &[u8]) -> bool;
    fn typename(&self) -> &'static str;
}

#[repr(C)]
struct DataNode {
    buf: [u8; 128],
    len: u64,
    _pad: u64,
}

#[repr(C)]
struct ExecNode {
    handler: u64,
    config: u64,
    name: [u8; 112],
    name_len: u64,
    run_count: u64,
}

#[repr(C)]
struct SentryNode {
    key: [u8; 32],
    token: u64,
    log: [u8; 96],
    log_len: u64,
}

const _: () = assert!(mem::size_of::<DataNode>() == NODE_SIZE);
const _: () = assert!(mem::size_of::<ExecNode>() == NODE_SIZE);
const _: () = assert!(mem::size_of::<SentryNode>() == NODE_SIZE);

const CONFIG_MAGIC: u64 = 0xC0DECA11;

impl Node for DataNode {
    fn inspect(&self) {
        println!("  data: {} bytes", self.len);
        let show = std::cmp::min(self.len as usize, 32);
        if show > 0 {
            print!("  hex: ");
            for b in &self.buf[..show] {
                print!("{:02x}", b);
            }
            println!();
        }
    }

    fn activate(&self) {
        println!("  echo {} bytes", self.len);
    }

    fn write_data(&mut self, offset: usize, data: &[u8]) -> bool {
        if offset.checked_add(data.len()).map_or(true, |end| end > 128) {
            println!("  write out of bounds");
            return false;
        }
        self.buf[offset..offset + data.len()].copy_from_slice(data);
        let end = (offset + data.len()) as u64;
        if end > self.len {
            self.len = end;
        }
        true
    }

    fn typename(&self) -> &'static str {
        "data"
    }
}

impl Node for ExecNode {
    fn inspect(&self) {
        let nlen = std::cmp::min(self.name_len as usize, 112);
        println!(
            "  handler={:#018x} config={:#018x}",
            self.handler, self.config
        );
        println!(
            "  name={} runs={}",
            String::from_utf8_lossy(&self.name[..nlen]),
            self.run_count
        );
    }

    fn activate(&self) {
        if self.config != CONFIG_MAGIC {
            println!("  bad config {:#018x}, denied", self.config);
            return;
        }
        if self.handler == 0 {
            println!("  null handler, denied");
            return;
        }
        println!("  calling {:#018x}", self.handler);
        unsafe {
            let f: fn() = mem::transmute::<u64, fn()>(self.handler);
            f();
        }
    }

    fn write_data(&mut self, _offset: usize, _data: &[u8]) -> bool {
        println!("  exec nodes don't support writes");
        false
    }

    fn typename(&self) -> &'static str {
        "exec"
    }
}

impl Node for SentryNode {
    fn inspect(&self) {
        print!("  key=");
        for b in &self.key[..8] {
            print!("{:02x}", b);
        }
        println!(".. token={:#018x}", self.token);
    }

    fn activate(&self) {
        let checksum: u64 = self.key.iter().map(|&b| b as u64).sum();
        if checksum == self.token {
            println!("  auth ok");
        } else {
            println!("  auth fail (got {:#x}, want {:#x})", checksum, self.token);
        }
    }

    fn write_data(&mut self, offset: usize, data: &[u8]) -> bool {
        if offset.checked_add(data.len()).map_or(true, |end| end > 32) {
            println!("  key is only 32 bytes");
            return false;
        }
        self.key[offset..offset + data.len()].copy_from_slice(data);
        true
    }

    fn typename(&self) -> &'static str {
        "sentry"
    }
}

fn handler_echo() {
    println!("  pong");
}
fn handler_status() {
    println!("  status: ok");
}
fn handler_diag() {
    println!("  diag: ok");
}

struct ShadowEntry {
    ptr: *const dyn Node,
    nonce: u64,
    type_name: &'static str,
}

fn hex_decode(s: &str) -> Vec<u8> {
    let s = s.trim();
    let s = s.strip_prefix("0x").or_else(|| s.strip_prefix("0X")).unwrap_or(s);
    (0..s.len() / 2)
        .filter_map(|i| u8::from_str_radix(&s[2 * i..2 * i + 2], 16).ok())
        .collect()
}

fn parse_usize(raw: &[u8]) -> Option<usize> {
    String::from_utf8_lossy(raw).trim().parse().ok()
}

fn parse_u64_hex(raw: &[u8]) -> Option<u64> {
    let s = String::from_utf8_lossy(raw);
    let s = s.trim();
    if let Some(hex) = s.strip_prefix("0x").or_else(|| s.strip_prefix("0X")) {
        u64::from_str_radix(hex, 16).ok()
    } else {
        s.parse().ok()
    }
}

fn main() {
    println!("main     @ {:#018x}", main as *const () as usize);
    println!("override @ {:#018x}", nexus_override as *const () as usize);
    println!();
    println!("c)reate f)ree i)nspect w)rite");
    println!("r)egister a)ctivate l)ist q)uit");

    let mut stdin = io::stdin().lock();
    let mut stdout = io::stdout().lock();

    let mut read_line = |prompt: &str| -> Vec<u8> {
        stdout.write_all(prompt.as_bytes()).unwrap();
        stdout.flush().unwrap();
        let mut buf = vec![];
        stdin.read_until(b'\n', &mut buf).unwrap();
        buf
    };

    let mut nodes: Vec<Option<Box<dyn Node>>> = (0..MAX_NODES).map(|_| None).collect();
    let mut shadows: Vec<Option<ShadowEntry>> = (0..MAX_SHADOWS).map(|_| None).collect();
    let mut nonce_state: u64 = 0x1337;
    let mut cmd_count: usize = 0;

    let mut next_nonce = || -> u64 {
        let n = nonce_state;
        nonce_state = nonce_state
            .wrapping_mul(6364136223846793005)
            .wrapping_add(1442695040888963407);
        n
    };

    loop {
        if cmd_count >= MAX_CMDS {
            println!("cmd limit hit, bye");
            break;
        }

        let input = read_line("\n> ");
        cmd_count += 1;

        match input.first().copied() {
            Some(b'C') | Some(b'c') => {
                let slot = match nodes.iter().position(|n| n.is_none()) {
                    Some(s) => s,
                    None => {
                        println!("  full");
                        continue;
                    }
                };

                let t = read_line("  type (d/e/s): ");
                match t.first().copied() {
                    Some(b'D') | Some(b'd') => {
                        nodes[slot] = Some(Box::new(DataNode {
                            buf: [0u8; 128],
                            len: 0,
                            _pad: 0,
                        }));
                        println!("  data -> {}", slot);
                    }
                    Some(b'E') | Some(b'e') => {
                        println!("  1) echo   {:#018x}", handler_echo as *const () as usize);
                        println!("  2) status {:#018x}", handler_status as *const () as usize);
                        println!("  3) diag   {:#018x}", handler_diag as *const () as usize);
                        let h = read_line("  handler: ");
                        let handler = match h.first().copied() {
                            Some(b'1') => handler_echo as *const () as u64,
                            Some(b'2') => handler_status as *const () as u64,
                            Some(b'3') => handler_diag as *const () as u64,
                            _ => {
                                println!("  bad handler");
                                continue;
                            }
                        };
                        let raw_name = read_line("  name: ");
                        let nlen = std::cmp::min(
                            raw_name.len().saturating_sub(1),
                            112,
                        );
                        let mut name = [0u8; 112];
                        name[..nlen].copy_from_slice(&raw_name[..nlen]);

                        nodes[slot] = Some(Box::new(ExecNode {
                            handler,
                            config: CONFIG_MAGIC,
                            name,
                            name_len: nlen as u64,
                            run_count: 0,
                        }));
                        println!("  exec -> {}", slot);
                    }
                    Some(b'S') | Some(b's') => {
                        let raw_key = read_line("  key (hex, 32B max): ");
                        let kb = hex_decode(&String::from_utf8_lossy(&raw_key));
                        let mut key = [0u8; 32];
                        let klen = std::cmp::min(kb.len(), 32);
                        key[..klen].copy_from_slice(&kb[..klen]);
                        let token: u64 = key.iter().map(|&b| b as u64).sum();

                        nodes[slot] = Some(Box::new(SentryNode {
                            key,
                            token,
                            log: [0u8; 96],
                            log_len: 0,
                        }));
                        println!("  sentry -> {}", slot);
                    }
                    _ => println!("  what?"),
                }
            }

            Some(b'F') | Some(b'f') => {
                let raw = read_line("  slot: ");
                let idx = match parse_usize(&raw) {
                    Some(i) if i < MAX_NODES => i,
                    _ => {
                        println!("  bad slot");
                        continue;
                    }
                };
                if nodes[idx].is_some() {
                    nodes[idx] = None;
                    println!("  freed {}", idx);
                } else {
                    println!("  already empty");
                }
            }

            Some(b'R') | Some(b'r') => {
                let raw = read_line("  slot: ");
                let idx = match parse_usize(&raw) {
                    Some(i) if i < MAX_NODES => i,
                    _ => {
                        println!("  bad slot");
                        continue;
                    }
                };
                let sidx = match shadows.iter().position(|s| s.is_none()) {
                    Some(s) => s,
                    None => {
                        println!("  shadow registry full");
                        continue;
                    }
                };
                if let Some(ref node) = nodes[idx] {
                    let raw_ptr: *const dyn Node = &**node;
                    let nonce = next_nonce();
                    let tname = node.typename();

                    shadows[sidx] = Some(ShadowEntry {
                        ptr: raw_ptr,
                        nonce,
                        type_name: tname,
                    });

                    println!("  {} -> shadow[{}] nonce={:#018x}", idx, sidx, nonce);
                } else {
                    println!("  empty slot");
                }
            }

            Some(b'A') | Some(b'a') => {
                let raw = read_line("  shadow: ");
                let sidx = match parse_usize(&raw) {
                    Some(i) if i < MAX_SHADOWS => i,
                    _ => {
                        println!("  bad idx");
                        continue;
                    }
                };
                let raw_nonce = read_line("  nonce: ");
                let nonce = match parse_u64_hex(&raw_nonce) {
                    Some(n) => n,
                    None => {
                        println!("  bad nonce");
                        continue;
                    }
                };

                if let Some(ref entry) = shadows[sidx] {
                    if entry.nonce != nonce {
                        println!("  wrong nonce");
                        continue;
                    }
                    println!("  activating shadow[{}] ({})", sidx, entry.type_name);
                    unsafe {
                        (*entry.ptr).activate();
                    }
                } else {
                    println!("  empty shadow");
                }
            }

            Some(b'I') | Some(b'i') => {
                let raw = read_line("  slot: ");
                let idx = match parse_usize(&raw) {
                    Some(i) if i < MAX_NODES => i,
                    _ => {
                        println!("  bad slot");
                        continue;
                    }
                };
                if let Some(ref node) = nodes[idx] {
                    println!("  [{}] {}", idx, node.typename());
                    node.inspect();
                } else {
                    println!("  empty");
                }
            }

            Some(b'W') | Some(b'w') => {
                let raw = read_line("  slot: ");
                let idx = match parse_usize(&raw) {
                    Some(i) if i < MAX_NODES => i,
                    _ => {
                        println!("  bad slot");
                        continue;
                    }
                };
                let raw_off = read_line("  off: ");
                let offset: usize = parse_usize(&raw_off).unwrap_or(0);
                let raw_data = read_line("  hex: ");
                let data = hex_decode(&String::from_utf8_lossy(&raw_data));

                if let Some(ref mut node) = nodes[idx] {
                    if node.write_data(offset, &data) {
                        println!("  ok, {} bytes @ {}", data.len(), offset);
                    }
                } else {
                    println!("  empty");
                }
            }

            Some(b'L') | Some(b'l') => {
                for (i, n) in nodes.iter().enumerate() {
                    match n {
                        Some(ref node) => println!("  [{}] {}", i, node.typename()),
                        None => println!("  [{}] -", i),
                    }
                }
                for (i, s) in shadows.iter().enumerate() {
                    match s {
                        Some(ref e) => {
                            println!("  s[{}] {} nonce={:#018x}", i, e.type_name, e.nonce)
                        }
                        None => println!("  s[{}] -", i),
                    }
                }
            }

            Some(b'Q') | Some(b'q') => {
                println!("bye");
                break;
            }

            _ => {
                println!("  ?");
            }
        }
    }
}
