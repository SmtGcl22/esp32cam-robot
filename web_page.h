#ifndef WEB_PAGE_H
#define WEB_PAGE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
  <title>KesisRobot Kontrol</title>
  <style>
    *,*::before,*::after{margin:0;padding:0;box-sizing:border-box}
    :root{
      --bg:#080b12;--card:rgba(12,18,30,0.82);--border:rgba(0,200,255,0.12);
      --accent:#00d4ff;--accent2:#7b61ff;--glow:rgba(0,200,255,0.3);
      --danger:#ff1744;--success:#00e676;--txt:#e0e4f0;--dim:#5a6580;
      --r:14px;
    }
    html,body{height:100%;overflow:hidden}
    body{font-family:'Segoe UI',Roboto,sans-serif;background:var(--bg);color:var(--txt)}
    body::before{content:'';position:fixed;inset:0;
      background:radial-gradient(ellipse at 20% 0%,rgba(0,200,255,0.06),transparent 55%),
                 radial-gradient(ellipse at 80% 100%,rgba(123,97,255,0.04),transparent 55%);
      pointer-events:none}

    .app{display:flex;flex-direction:column;height:100vh;max-width:520px;margin:0 auto;padding:6px 10px;position:relative;z-index:1}

    /* HEADER */
    .hdr{text-align:center;padding:8px 0 4px}
    .hdr h1{font-size:1.1rem;font-weight:800;letter-spacing:3px;
      background:linear-gradient(135deg,var(--accent),var(--accent2));
      -webkit-background-clip:text;-webkit-text-fill-color:transparent;text-transform:uppercase}
    .hdr .sub{font-size:.58rem;color:var(--dim);letter-spacing:4px;text-transform:uppercase}
    .badges{display:flex;justify-content:center;gap:8px;margin-top:6px}
    .badge{display:inline-flex;align-items:center;gap:4px;padding:2px 10px;border-radius:12px;
      font-size:.55rem;font-weight:600;letter-spacing:1px}
    .badge.ok{background:rgba(0,230,118,0.1);border:1px solid rgba(0,230,118,0.25);color:var(--success)}
    .badge.esp{background:rgba(0,200,255,0.08);border:1px solid rgba(0,200,255,0.2);color:var(--accent)}
    .dot{width:5px;height:5px;border-radius:50%;background:currentColor;animation:pulse 2s infinite}
    @keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}

    /* CAMERA */
    .cam-wrap{position:relative;width:100%;aspect-ratio:4/3;background:#0a0f1a;
      border-radius:var(--r);overflow:hidden;border:1px solid var(--border);margin:6px 0;flex-shrink:0}
    .cam-wrap img{width:100%;height:100%;object-fit:cover;display:block}
    .cam-off{position:absolute;inset:0;display:flex;flex-direction:column;align-items:center;
      justify-content:center;gap:6px;color:var(--dim);font-size:.7rem;letter-spacing:2px}
    .cam-off span:first-child{font-size:1.8rem;opacity:.4}
    .cam-hud{position:absolute;top:6px;left:8px;font-size:.5rem;color:var(--accent);
      letter-spacing:2px;opacity:.7;text-transform:uppercase}
    .cam-hud.r{left:auto;right:8px}
    .flash-btn{position:absolute;bottom:8px;right:8px;width:32px;height:32px;border-radius:50%;
      background:rgba(0,0,0,0.5);border:1px solid rgba(255,255,255,0.15);color:#fff;
      font-size:.9rem;display:flex;align-items:center;justify-content:center;cursor:pointer;
      backdrop-filter:blur(4px);transition:.2s}
    .flash-btn.on{background:rgba(255,200,0,0.3);border-color:rgba(255,200,0,0.5);color:#ffd600}

    /* CONTROLS ROW */
    .ctrl{display:flex;align-items:center;justify-content:center;gap:12px;
      flex:1;min-height:0;padding:4px 0}

    /* ROTATION BTN */
    .rot-btn{width:56px;height:56px;border-radius:50%;
      background:rgba(123,97,255,0.08);border:1.5px solid rgba(123,97,255,0.25);
      color:var(--accent2);font-size:1.3rem;display:flex;align-items:center;justify-content:center;
      cursor:pointer;user-select:none;-webkit-user-select:none;touch-action:manipulation;
      transition:.15s;flex-shrink:0}
    .rot-btn:active,.rot-btn.active{background:rgba(123,97,255,0.25);
      border-color:var(--accent2);box-shadow:0 0 20px rgba(123,97,255,0.3);transform:scale(.92)}
    .rot-label{font-size:.45rem;color:var(--dim);text-align:center;margin-top:2px;
      letter-spacing:1px;text-transform:uppercase}

    /* JOYSTICK */
    .joy-wrap{position:relative;flex-shrink:0}
    .joy-canvas{display:block;touch-action:none}
    .joy-info{position:absolute;bottom:-14px;left:50%;transform:translateX(-50%);
      font-size:.5rem;color:var(--dim);letter-spacing:1px;white-space:nowrap}

    /* SPEED */
    .speed-row{display:flex;align-items:center;gap:10px;padding:6px 16px;
      background:var(--card);border:1px solid var(--border);border-radius:10px;margin:4px 0;flex-shrink:0}
    .speed-row label{font-size:.6rem;color:var(--dim);letter-spacing:2px;text-transform:uppercase;flex-shrink:0}
    .speed-row input[type=range]{flex:1;-webkit-appearance:none;height:4px;border-radius:2px;
      background:rgba(255,255,255,0.06);outline:none}
    .speed-row input::-webkit-slider-thumb{-webkit-appearance:none;width:18px;height:18px;
      border-radius:50%;background:linear-gradient(135deg,var(--accent),var(--accent2));
      box-shadow:0 0 8px var(--glow);border:2px solid rgba(255,255,255,0.15);cursor:pointer}
    .speed-row .val{font-size:.75rem;font-weight:700;color:var(--accent);min-width:28px;text-align:right}

    /* DIRECTION DISPLAY */
    .dir-display{text-align:center;padding:2px 0;flex-shrink:0}
    .dir-display .dir-text{font-size:.65rem;font-weight:700;color:var(--accent);
      letter-spacing:3px;text-transform:uppercase;min-height:1em}

    /* FOOTER */
    .ftr{text-align:center;font-size:.5rem;color:var(--dim);letter-spacing:1px;padding:4px 0;flex-shrink:0}
  </style>
</head>
<body>
<div class="app">
  <!-- HEADER -->
  <div class="hdr">
    <h1>Kesis-Robot</h1>
    <div class="sub">Mecanum Kontrol Merkezi</div>
    <div class="badges">
      <span class="badge ok"><span class="dot"></span>WiFi</span>
      <span class="badge esp"><span class="dot"></span>ESP-NOW</span>
    </div>
  </div>

  <!-- CAMERA -->
  <div class="cam-wrap" id="camWrap">
    <img id="camImg" src="" style="display:none" alt="Canli Yayin">
    <div class="cam-off" id="camOff">
      <span>&#128247;</span><span>Kamera Yükleniyor...</span>
    </div>
    <div class="cam-hud">&#9679; REC</div>
    <div class="cam-hud r" id="fpsHud">--FPS</div>
    <div class="flash-btn" id="flashBtn" onclick="toggleFlash()">&#128294;</div>
  </div>

  <!-- DIRECTION TEXT -->
  <div class="dir-display"><div class="dir-text" id="dirText">DUR</div></div>

  <!-- CONTROLS: Rot Left + Joystick + Rot Right -->
  <div class="ctrl">
    <div style="text-align:center">
      <div class="rot-btn" id="rotL"
        ontouchstart="startRot(-1,this);event.preventDefault()"
        onmousedown="startRot(-1,this)"
        ontouchend="stopRot(this)" onmouseup="stopRot(this)" onmouseleave="stopRot(this)">&#8634;</div>
      <div class="rot-label">Sol Dön</div>
    </div>

    <div class="joy-wrap">
      <canvas class="joy-canvas" id="joyCanvas" width="160" height="160"></canvas>
      <div class="joy-info" id="joyInfo">X:0 Y:0</div>
    </div>

    <div style="text-align:center">
      <div class="rot-btn" id="rotR"
        ontouchstart="startRot(1,this);event.preventDefault()"
        onmousedown="startRot(1,this)"
        ontouchend="stopRot(this)" onmouseup="stopRot(this)" onmouseleave="stopRot(this)">&#8635;</div>
      <div class="rot-label">Sağ Dön</div>
    </div>
  </div>

  <!-- SPEED SLIDER -->
  <div class="speed-row">
    <label>Hız</label>
    <input type="range" id="speedSlider" min="80" max="255" value="200" oninput="onSpeed(this.value)">
    <span class="val" id="speedVal">200</span>
  </div>

  <div class="ftr">KESIS-ROBOT &copy; 2026 &mdash; ESP32-CAM MECANUM KONTROL</div>
</div>

<script>
// ===== CONFIG =====
const SEND_INTERVAL = 80; // ms
const DEADZONE = 8;

// ===== STATE =====
let joyX = 0, joyY = 0, rot = 0, speed = 200;
let joyActive = false, flashOn = false;
let sendTimer = null;

// ===== CAMERA =====
const camImg = document.getElementById('camImg');
const camOff = document.getElementById('camOff');
const streamUrl = 'http://' + location.hostname + ':81/stream';
camImg.onload = function(){ camOff.style.display='none'; camImg.style.display='block'; };
camImg.onerror = function(){
  setTimeout(()=>{ camImg.src = streamUrl + '?t=' + Date.now(); }, 2000);
};
// Start loading
setTimeout(()=>{ camImg.src = streamUrl; }, 500);

// ===== FLASH =====
function toggleFlash(){
  flashOn = !flashOn;
  document.getElementById('flashBtn').classList.toggle('on', flashOn);
  fetch('/flash?v=' + (flashOn?1:0)).catch(()=>{});
}

// ===== JOYSTICK =====
const canvas = document.getElementById('joyCanvas');
const ctx = canvas.getContext('2d');
const W = canvas.width, H = canvas.height;
const CX = W/2, CY = H/2;
const BASE_R = 68, KNOB_R = 24;
let knobX = CX, knobY = CY;

function drawJoy(){
  ctx.clearRect(0,0,W,H);

  // Outer ring
  ctx.beginPath();
  ctx.arc(CX,CY,BASE_R,0,Math.PI*2);
  ctx.strokeStyle = 'rgba(0,200,255,0.15)';
  ctx.lineWidth = 2;
  ctx.stroke();

  // Cross lines
  ctx.strokeStyle = 'rgba(0,200,255,0.06)';
  ctx.lineWidth = 1;
  ctx.beginPath(); ctx.moveTo(CX,CY-BASE_R+10); ctx.lineTo(CX,CY+BASE_R-10); ctx.stroke();
  ctx.beginPath(); ctx.moveTo(CX-BASE_R+10,CY); ctx.lineTo(CX+BASE_R-10,CY); ctx.stroke();

  // Direction indicators
  ctx.fillStyle = 'rgba(0,200,255,0.25)';
  ctx.font = '10px sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText('▲', CX, CY-BASE_R+14);
  ctx.fillText('▼', CX, CY+BASE_R-8);
  ctx.fillText('◀', CX-BASE_R+12, CY+4);
  ctx.fillText('▶', CX+BASE_R-12, CY+4);

  // Knob shadow
  if(joyActive){
    let g = ctx.createRadialGradient(knobX,knobY,0,knobX,knobY,KNOB_R*2.5);
    g.addColorStop(0,'rgba(0,200,255,0.2)');
    g.addColorStop(1,'transparent');
    ctx.fillStyle = g;
    ctx.fillRect(0,0,W,H);
  }

  // Knob
  let grad = ctx.createRadialGradient(knobX-4,knobY-4,0,knobX,knobY,KNOB_R);
  grad.addColorStop(0, joyActive?'#33e0ff':'#1a8fa8');
  grad.addColorStop(1, joyActive?'#0090b0':'#0d5060');
  ctx.beginPath();
  ctx.arc(knobX,knobY,KNOB_R,0,Math.PI*2);
  ctx.fillStyle = grad;
  ctx.fill();
  ctx.strokeStyle = joyActive?'rgba(0,220,255,0.6)':'rgba(0,200,255,0.25)';
  ctx.lineWidth = 2;
  ctx.stroke();
}

function getJoyPos(e){
  let rect = canvas.getBoundingClientRect();
  let t = e.touches ? e.touches[0] : e;
  let x = t.clientX - rect.left;
  let y = t.clientY - rect.top;
  let dx = x - CX, dy = y - CY;
  let dist = Math.sqrt(dx*dx + dy*dy);
  let maxD = BASE_R - KNOB_R;
  if(dist > maxD){ dx = dx/dist*maxD; dy = dy/dist*maxD; }
  return {kx: CX+dx, ky: CY+dy, nx: Math.round(dx/maxD*100), ny: Math.round(-dy/maxD*100)};
}

function joyDown(e){
  e.preventDefault();
  joyActive = true;
  joyMove(e);
}
function joyMove(e){
  if(!joyActive) return;
  e.preventDefault();
  let p = getJoyPos(e);
  knobX = p.kx; knobY = p.ky;
  joyX = Math.abs(p.nx)<DEADZONE ? 0 : p.nx;
  joyY = Math.abs(p.ny)<DEADZONE ? 0 : p.ny;
  document.getElementById('joyInfo').textContent = 'X:'+joyX+' Y:'+joyY;
  updateDir();
  drawJoy();
}
function joyUp(e){
  joyActive = false;
  knobX = CX; knobY = CY;
  joyX = 0; joyY = 0;
  document.getElementById('joyInfo').textContent = 'X:0 Y:0';
  updateDir();
  drawJoy();
  sendControl();
}

canvas.addEventListener('mousedown', joyDown);
canvas.addEventListener('mousemove', joyMove);
canvas.addEventListener('mouseup', joyUp);
canvas.addEventListener('mouseleave', joyUp);
canvas.addEventListener('touchstart', joyDown, {passive:false});
canvas.addEventListener('touchmove', joyMove, {passive:false});
canvas.addEventListener('touchend', joyUp);

drawJoy();

// ===== ROTATION =====
function startRot(dir, btn){
  rot = dir * 100;
  btn.classList.add('active');
  updateDir();
}
function stopRot(btn){
  rot = 0;
  btn.classList.remove('active');
  updateDir();
}

// ===== SPEED =====
function onSpeed(v){
  speed = parseInt(v);
  document.getElementById('speedVal').textContent = v;
}

// ===== DIRECTION DISPLAY =====
function updateDir(){
  let t = '';
  if(joyY > 20) t = 'ILERI';
  else if(joyY < -20) t = 'GERI';

  if(joyX > 20) t += (t?' + ':'') + 'SAGA KAY';
  else if(joyX < -20) t += (t?' + ':'') + 'SOLA KAY';

  if(rot > 0) t += (t?' + ':'') + 'SAGA DON';
  else if(rot < 0) t += (t?' + ':'') + 'SOLA DON';

  if(!t) t = 'DUR';
  document.getElementById('dirText').textContent = t;
}

// ===== SEND CONTROL DATA =====
function sendControl(){
  fetch('/control?x='+joyX+'&y='+joyY+'&r='+rot+'&s='+speed).catch(()=>{});
}

// Periodic sender
sendTimer = setInterval(()=>{
  if(joyActive || rot !== 0){
    sendControl();
  }
}, SEND_INTERVAL);

// Also send stop when everything is idle (heartbeat)
setInterval(()=>{
  if(!joyActive && rot === 0){
    sendControl();
  }
}, 500);
</script>
</body>
</html>
)rawliteral";

#endif
