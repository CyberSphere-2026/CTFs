import { io } from 'socket.io-client';

const _ROOMS = Object.freeze({
  public:   'public_feed',
  internal: 'shadow_ops',
});

const socket = io(window.location.origin);

socket.on('connect', () => {
  setStatus('CONNECTED', true);
  socket.emit('join', _ROOMS.public);
});

socket.on('disconnect', () => {
  setStatus('DISCONNECTED', false);
});

socket.on('message', (msg) => {
  pushFeedItem(msg);
});

function setStatus(text, online) {
  const dot   = document.getElementById('status-dot');
  const label = document.getElementById('status-label');
  if (dot)   dot.className     = online ? 'dot dot--online' : 'dot dot--offline';
  if (label) label.textContent = text;
}

function pushFeedItem({ from, text }) {
  const feed = document.getElementById('ops-feed');
  if (!feed) return;

  const ts   = new Date().toLocaleTimeString('en-US', { hour12: false });
  const item = document.createElement('div');
  item.className = 'feed-item';
  item.innerHTML =
    `<span class="ts">${ts}</span>` +
    `<span class="origin">${from}</span>` +
    `<span class="body">${text}</span>`;

  feed.insertBefore(item, feed.firstChild);

  while (feed.children.length > 40) {
    feed.removeChild(feed.lastChild);
  }
}

window.addEventListener('DOMContentLoaded', () => {
  const hero = document.getElementById('hero-text');
  if (!hero) return;
  const full = hero.dataset.text || '';
  hero.textContent = '';
  let i = 0;
  const tick = setInterval(() => {
    if (i >= full.length) { clearInterval(tick); return; }
    hero.textContent += full[i++];
  }, 40);
});
