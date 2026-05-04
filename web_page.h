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
      --yellow:#ffd600;--r:14px;
    }
    html,body{height:100%;overflow-x:hidden}
    body{font-family:'Segoe UI',Roboto,sans-serif;background:var(--bg);color:var(--txt)}
    body::before{content:'';position:fixed;inset:0;
      background:radial-gradient(ellipse at 20% 0%,rgba(0,200,255,0.06),transparent 55%),
                 radial-gradient(ellipse at 80% 100%,rgba(123,97,255,0.04),transparent 55%);
      pointer-events:none}

    .app{display:flex;flex-direction:column;min-height:100vh;max-width:520px;margin:0 auto;padding:6px 10px;position:relative;z-index:1}

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

    /* MODE SWITCH */
    .mode-switch{display:flex;gap:0;margin:8px 0;border-radius:10px;overflow:hidden;border:1px solid var(--border)}
    .mode-btn{flex:1;padding:10px 0;text-align:center;font-size:.7rem;font-weight:700;letter-spacing:2px;
      text-transform:uppercase;cursor:pointer;transition:.25s;background:var(--card);color:var(--dim);border:none}
    .mode-btn.active-manual{background:linear-gradient(135deg,rgba(0,212,255,0.2),rgba(0,212,255,0.05));
      color:var(--accent);box-shadow:inset 0 -2px 0 var(--accent)}
    .mode-btn.active-auto{background:linear-gradient(135deg,rgba(255,214,0,0.2),rgba(255,214,0,0.05));
      color:var(--yellow);box-shadow:inset 0 -2px 0 var(--yellow)}

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

    /* AUTO PANEL */
    .auto-panel{background:var(--card);border:1px solid rgba(255,214,0,0.15);border-radius:var(--r);
      padding:12px;margin:6px 0;display:none}
    .auto-panel.show{display:block}
    .auto-panel h3{font-size:.65rem;color:var(--yellow);letter-spacing:2px;text-transform:uppercase;margin-bottom:8px}
    .color-row{display:flex;gap:8px;margin-bottom:10px}
    .color-btn{width:36px;height:36px;border-radius:50%;border:2px solid rgba(255,255,255,0.1);
      cursor:pointer;transition:.2s;position:relative}
    .color-btn.sel{border-color:#fff;transform:scale(1.15);box-shadow:0 0 12px rgba(255,255,255,0.3)}
    .color-btn[data-c="0"]{background:#ff1744}
    .color-btn[data-c="1"]{background:#2979ff}
    .color-btn[data-c="2"]{background:#ffd600}
    .auto-status{display:flex;gap:12px;flex-wrap:wrap}
    .auto-stat{flex:1;min-width:60px;background:rgba(255,255,255,0.03);border-radius:8px;padding:6px 8px;text-align:center}
    .auto-stat .label{font-size:.5rem;color:var(--dim);letter-spacing:1px;text-transform:uppercase}
    .auto-stat .value{font-size:.85rem;font-weight:700;color:var(--yellow);margin-top:2px}
    .target-indicator{position:absolute;top:50%;left:50%;width:40px;height:40px;
      border:2px solid var(--yellow);border-radius:50%;transform:translate(-50%,-50%);
      display:none;pointer-events:none;box-shadow:0 0 15px rgba(255,214,0,0.4)}
    .target-indicator.show{display:block}
    .crosshair{position:absolute;top:50%;left:50%;width:12px;height:12px;transform:translate(-50%,-50%)}
    .crosshair::before,.crosshair::after{content:'';position:absolute;background:var(--yellow)}
    .crosshair::before{width:12px;height:1px;top:50%;left:0;transform:translateY(-50%)}
    .crosshair::after{width:1px;height:12px;left:50%;top:0;transform:translateX(-50%)}

    /* MANUAL CONTROLS */
    .manual-panel{margin:4px 0}
    .manual-panel.hide{display:none}
    .ctrl{display:flex;align-items:center;justify-content:center;gap:12px;padding:4px 0}
    .rot-btn{width:56px;height:56px;border-radius:50%;
      background:rgba(123,97,255,0.08);border:1.5px solid rgba(123,97,255,0.25);
      color:var(--accent2);font-size:1.3rem;display:flex;align-items:center;justify-content:center;
      cursor:pointer;user-select:none;-webkit-user-select:none;touch-action:manipulation;
      transition:.15s;flex-shrink:0}
    .rot-btn:active,.rot-btn.active{background:rgba(123,97,255,0.25);
      border-color:var(--accent2);box-shadow:0 0 20px rgba(123,97,255,0.3);transform:scale(.92)}
    .rot-label{font-size:.45rem;color:var(--dim);text-align:center;margin-top:2px;
      letter-spacing:1px;text-transform:uppercase}
    .joy-wrap{position:relative;flex-shrink:0}
    .joy-canvas{display:block;touch-action:none}
    .joy-info{position:absolute;bottom:-14px;left:50%;transform:translateX(-50%);
      font-size:.5rem;color:var(--dim);letter-spacing:1px;white-space:nowrap}

    .speed-row{display:flex;align-items:center;gap:10px;padding:6px 16px;
      background:var(--card);border:1px solid var(--border);border-radius:10px;margin:4px 0;flex-shrink:0}
    .speed-row label{font-size:.6rem;color:var(--dim);letter-spacing:2px;text-transform:uppercase;flex-shrink:0}
    .speed-row input[type=range]{flex:1;-webkit-appearance:none;height:4px;border-radius:2px;
      background:rgba(255,255,255,0.06);outline:none}
    .speed-row input::-webkit-slider-thumb{-webkit-appearance:none;width:18px;height:18px;
      border-radius:50%;background:linear-gradient(135deg,var(--accent),var(--accent2));
      box-shadow:0 0 8px var(--glow);border:2px solid rgba(255,255,255,0.15);cursor:pointer}
    .speed-row .val{font-size:.75rem;font-weight:700;color:var(--accent);min-width:28px;text-align:right}

    .dir-display{text-align:center;padding:2px 0;flex-shrink:0}
    .dir-display .dir-text{font-size:.65rem;font-weight:700;color:var(--accent);
      letter-spacing:3px;text-transform:uppercase;min-height:1em}

    .horn-btn{width:100%;height:44px;border-radius:10px;margin-bottom:8px;
      background:rgba(255,23,68,0.1);border:1px solid rgba(255,23,68,0.3);
      color:var(--danger);font-size:1rem;font-weight:700;letter-spacing:2px;
      display:flex;align-items:center;justify-content:center;gap:8px;
      cursor:pointer;user-select:none;-webkit-user-select:none;touch-action:manipulation;
      transition:.15s}
    .horn-btn:active,.horn-btn.active{background:rgba(255,23,68,0.25);
      border-color:var(--danger);box-shadow:0 0 15px rgba(255,23,68,0.3);transform:scale(.98)}

    /* PAN-TILT */
    .pt-panel{background:var(--card);border:1px solid var(--border);border-radius:var(--r);
      padding:12px;margin:6px 0}
    .pt-panel h3{font-size:.6rem;color:var(--accent);letter-spacing:2px;text-transform:uppercase;
      margin-bottom:10px;text-align:center}
    .pt-slider-row{display:flex;align-items:center;gap:10px;margin-bottom:8px}
    .pt-slider-row:last-child{margin-bottom:0}
    .pt-slider-row label{font-size:.6rem;color:var(--dim);letter-spacing:2px;text-transform:uppercase;
      min-width:32px;flex-shrink:0}
    .pt-slider-row input[type=range]{flex:1;-webkit-appearance:none;height:6px;border-radius:3px;
      background:rgba(0,212,255,0.1);outline:none}
    .pt-slider-row input::-webkit-slider-thumb{-webkit-appearance:none;width:22px;height:22px;
      border-radius:50%;background:linear-gradient(135deg,var(--accent),#0090b0);
      box-shadow:0 0 10px rgba(0,212,255,0.4);border:2px solid rgba(255,255,255,0.2);cursor:pointer}
    .pt-slider-row .pt-val{font-size:.75rem;font-weight:700;color:var(--accent);min-width:32px;text-align:right}
    .pt-center-btn{display:block;width:100%;padding:8px;margin-top:6px;border-radius:8px;
      background:rgba(123,97,255,0.1);border:1px solid rgba(123,97,255,0.25);
      color:var(--accent2);font-size:.6rem;font-weight:700;letter-spacing:2px;
      text-transform:uppercase;cursor:pointer;text-align:center;transition:.15s}
    .pt-center-btn:active{background:rgba(123,97,255,0.25);transform:scale(.98)}

    .ftr{text-align:center;font-size:.5rem;color:var(--dim);letter-spacing:1px;padding:4px 0;flex-shrink:0}
  </style>
</head>
<body>
<div class="app">
  <div class="hdr">
    <h1>Kesis-Robot</h1>
    <div class="sub">Mecanum Kontrol Merkezi</div>
    <div class="badges">
      <span class="badge ok"><span class="dot"></span>WiFi</span>
      <span class="badge esp"><span class="dot"></span>ESP-NOW</span>
    </div>
  </div>

  <!-- MODE SWITCH -->
  <div class="mode-switch">
    <div class="mode-btn active-manual" id="btnManual" onclick="setMode('manual')">&#9881; Manuel</div>
    <div class="mode-btn" id="btnAuto" onclick="setMode('auto')">&#9678; Otonom</div>
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
    <div class="target-indicator" id="targetInd"><div class="crosshair"></div></div>
  </div>

  <!-- AUTO PANEL -->
  <div class="auto-panel" id="autoPanel">
    <h3>&#127919; Hedef Renk Sec</h3>
    <div class="color-row">
      <div class="color-btn sel" data-c="0" onclick="pickColor(0,this)" title="Kirmizi"></div>
      <div class="color-btn" data-c="1" onclick="pickColor(1,this)" title="Mavi"></div>
      <div class="color-btn" data-c="2" onclick="pickColor(2,this)" title="Sari"></div>
    </div>
    <div class="auto-status">
      <div class="auto-stat"><div class="label">Durum</div><div class="value" id="aState">ARANIYOR</div></div>
      <div class="auto-stat"><div class="label">Alan</div><div class="value" id="aArea">0</div></div>
      <div class="auto-stat"><div class="label">FPS</div><div class="value" id="aFps">0</div></div>
      <div class="auto-stat"><div class="label">Konum</div><div class="value" id="aPos">-</div></div>
    </div>
  </div>

  <!-- MANUAL PANEL -->
  <div class="manual-panel" id="manualPanel">
    <div class="dir-display"><div class="dir-text" id="dirText">DUR</div></div>
    <div class="horn-btn" id="hornBtn"
        ontouchstart="setHorn(1);event.preventDefault()" onmousedown="setHorn(1)"
        ontouchend="setHorn(0)" onmouseup="setHorn(0)" onmouseleave="setHorn(0)">&#128227; KORNA</div>
    <div class="ctrl">
      <div style="text-align:center">
        <div class="rot-btn" id="rotL"
          ontouchstart="startRot(-1,this);event.preventDefault()"
          onmousedown="startRot(-1,this)"
          ontouchend="stopRot(this)" onmouseup="stopRot(this)" onmouseleave="stopRot(this)">&#8634;</div>
        <div class="rot-label">Sol Don</div>
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
        <div class="rot-label">Sag Don</div>
      </div>
    </div>
    <div class="speed-row">
      <label>Hiz</label>
      <input type="range" id="speedSlider" min="80" max="255" value="200" oninput="onSpeed(this.value)">
      <span class="val" id="speedVal">200</span>
    </div>
  </div>

  <!-- PAN-TILT -->
  <div class="pt-panel">
    <h3>&#128249; Kamera Pan-Tilt</h3>
    <div class="pt-slider-row">
      <label>Pan</label>
      <input type="range" id="panSlider" min="0" max="180" value="90" oninput="ptSlide()">
      <span class="pt-val" id="panVal">90&deg;</span>
    </div>
    <div class="pt-slider-row">
      <label>Tilt</label>
      <input type="range" id="tiltSlider" min="0" max="180" value="90" oninput="ptSlide()">
      <span class="pt-val" id="tiltVal">90&deg;</span>
    </div>
    <div class="pt-center-btn" onclick="ptCenter()">&#8634; Ortala</div>
  </div>

  <div class="ftr">KESIS-ROBOT &copy; 2026 &mdash; ESP32-CAM MECANUM KONTROL</div>
</div>

<script>
// ===== CONFIG =====
const SEND_INTERVAL = 80;
const DEADZONE = 8;

// ===== STATE =====
let joyX=0,joyY=0,rot=0,speed=200,hornVal=0;
let joyActive=false,flashOn=false;
let sendTimer=null;
let currentMode='manual'; // 'manual' or 'auto'
let selectedColor=0; // default red
let autoPolling=null;
let panPos=90,tiltPos=90;

// ===== MODE SWITCH =====
function setMode(mode){
  currentMode=mode;
  const btnM=document.getElementById('btnManual');
  const btnA=document.getElementById('btnAuto');
  const manP=document.getElementById('manualPanel');
  const autP=document.getElementById('autoPanel');

  if(mode==='auto'){
    btnM.className='mode-btn';
    btnA.className='mode-btn active-auto';
    manP.classList.add('hide');
    autP.classList.add('show');
    // Stop manual controls
    joyX=0;joyY=0;rot=0;
    sendControl();
    // Enable auto mode on ESP32
    fetch('/automode?mode=1&color='+selectedColor).catch(()=>{});
    startAutoPolling();
  } else {
    btnA.className='mode-btn';
    btnM.className='mode-btn active-manual';
    autP.classList.remove('show');
    manP.classList.remove('hide');
    // Disable auto mode
    fetch('/automode?mode=0').catch(()=>{});
    stopAutoPolling();
    document.getElementById('targetInd').classList.remove('show');
  }
}

// ===== COLOR PICK =====
function pickColor(c,el){
  selectedColor=c;
  document.querySelectorAll('.color-btn').forEach(b=>b.classList.remove('sel'));
  el.classList.add('sel');
  if(currentMode==='auto'){
    fetch('/automode?mode=1&color='+c).catch(()=>{});
  }
}

// ===== AUTO POLLING =====
function startAutoPolling(){
  if(autoPolling) return;
  autoPolling=setInterval(pollTarget,300);
}
function stopAutoPolling(){
  if(autoPolling){clearInterval(autoPolling);autoPolling=null;}
}
function pollTarget(){
  fetch('/target').then(r=>r.json()).then(d=>{
    const ind=document.getElementById('targetInd');
    const cam=document.getElementById('camWrap');
    document.getElementById('aFps').textContent=d.fps||0;
    document.getElementById('aArea').textContent=d.area||0;
    if(d.found){
      document.getElementById('aState').textContent='BULUNDU';
      document.getElementById('aState').style.color='#00e676';
      document.getElementById('aPos').textContent=d.cx+','+d.cy;
      // Position target indicator on camera view
      const camR=cam.getBoundingClientRect();
      const px=(d.cx/320)*100;
      const py=(d.cy/240)*100;
      ind.style.left=px+'%';
      ind.style.top=py+'%';
      ind.classList.add('show');
    } else {
      document.getElementById('aState').textContent='ARANIYOR';
      document.getElementById('aState').style.color='var(--yellow)';
      document.getElementById('aPos').textContent='-';
      ind.classList.remove('show');
    }
  }).catch(()=>{});
}

// ===== CAMERA =====
const camImg=document.getElementById('camImg');
const camOff=document.getElementById('camOff');
const streamUrl='http://'+location.hostname+':81/stream';
camImg.onload=function(){camOff.style.display='none';camImg.style.display='block';};
camImg.onerror=function(){setTimeout(()=>{camImg.src=streamUrl+'?t='+Date.now();},2000);};
setTimeout(()=>{camImg.src=streamUrl;},500);

// ===== FLASH =====
function toggleFlash(){
  flashOn=!flashOn;
  document.getElementById('flashBtn').classList.toggle('on',flashOn);
  fetch('/flash?v='+(flashOn?1:0)).catch(()=>{});
}

// ===== JOYSTICK =====
const canvas=document.getElementById('joyCanvas');
const ctx=canvas.getContext('2d');
const W=canvas.width,H=canvas.height;
const CX=W/2,CY=H/2;
const BASE_R=68,KNOB_R=24;
let knobX=CX,knobY=CY;

function drawJoy(){
  ctx.clearRect(0,0,W,H);
  ctx.beginPath();ctx.arc(CX,CY,BASE_R,0,Math.PI*2);
  ctx.strokeStyle='rgba(0,200,255,0.15)';ctx.lineWidth=2;ctx.stroke();
  ctx.strokeStyle='rgba(0,200,255,0.06)';ctx.lineWidth=1;
  ctx.beginPath();ctx.moveTo(CX,CY-BASE_R+10);ctx.lineTo(CX,CY+BASE_R-10);ctx.stroke();
  ctx.beginPath();ctx.moveTo(CX-BASE_R+10,CY);ctx.lineTo(CX+BASE_R-10,CY);ctx.stroke();
  ctx.fillStyle='rgba(0,200,255,0.25)';ctx.font='10px sans-serif';ctx.textAlign='center';
  ctx.fillText('\u25B2',CX,CY-BASE_R+14);ctx.fillText('\u25BC',CX,CY+BASE_R-8);
  ctx.fillText('\u25C0',CX-BASE_R+12,CY+4);ctx.fillText('\u25B6',CX+BASE_R-12,CY+4);
  if(joyActive){
    let g=ctx.createRadialGradient(knobX,knobY,0,knobX,knobY,KNOB_R*2.5);
    g.addColorStop(0,'rgba(0,200,255,0.2)');g.addColorStop(1,'transparent');
    ctx.fillStyle=g;ctx.fillRect(0,0,W,H);
  }
  let grad=ctx.createRadialGradient(knobX-4,knobY-4,0,knobX,knobY,KNOB_R);
  grad.addColorStop(0,joyActive?'#33e0ff':'#1a8fa8');
  grad.addColorStop(1,joyActive?'#0090b0':'#0d5060');
  ctx.beginPath();ctx.arc(knobX,knobY,KNOB_R,0,Math.PI*2);
  ctx.fillStyle=grad;ctx.fill();
  ctx.strokeStyle=joyActive?'rgba(0,220,255,0.6)':'rgba(0,200,255,0.25)';
  ctx.lineWidth=2;ctx.stroke();
}

function getJoyPos(e){
  let rect=canvas.getBoundingClientRect();
  let t=e.touches?e.touches[0]:e;
  let x=t.clientX-rect.left,y=t.clientY-rect.top;
  let dx=x-CX,dy=y-CY;
  let dist=Math.sqrt(dx*dx+dy*dy);
  let maxD=BASE_R-KNOB_R;
  if(dist>maxD){dx=dx/dist*maxD;dy=dy/dist*maxD;}
  return{kx:CX+dx,ky:CY+dy,nx:Math.round(dx/maxD*100),ny:Math.round(-dy/maxD*100)};
}

function joyDown(e){e.preventDefault();joyActive=true;joyMove(e);}
function joyMove(e){
  if(!joyActive)return;e.preventDefault();
  let p=getJoyPos(e);knobX=p.kx;knobY=p.ky;
  joyX=Math.abs(p.nx)<DEADZONE?0:p.nx;
  joyY=Math.abs(p.ny)<DEADZONE?0:p.ny;
  document.getElementById('joyInfo').textContent='X:'+joyX+' Y:'+joyY;
  updateDir();drawJoy();
}
function joyUp(e){
  joyActive=false;knobX=CX;knobY=CY;joyX=0;joyY=0;
  document.getElementById('joyInfo').textContent='X:0 Y:0';
  updateDir();drawJoy();sendControl();
}
canvas.addEventListener('mousedown',joyDown);
canvas.addEventListener('mousemove',joyMove);
canvas.addEventListener('mouseup',joyUp);
canvas.addEventListener('mouseleave',joyUp);
canvas.addEventListener('touchstart',joyDown,{passive:false});
canvas.addEventListener('touchmove',joyMove,{passive:false});
canvas.addEventListener('touchend',joyUp);
drawJoy();

// ===== ROTATION =====
function startRot(dir,btn){rot=dir*100;btn.classList.add('active');updateDir();}
function stopRot(btn){rot=0;btn.classList.remove('active');updateDir();}

// ===== HORN =====
function setHorn(v){
  hornVal=v;
  if(v) document.getElementById('hornBtn').classList.add('active');
  else document.getElementById('hornBtn').classList.remove('active');
  sendControl();
}

// ===== SPEED =====
function onSpeed(v){speed=parseInt(v);document.getElementById('speedVal').textContent=v;}

// ===== DIRECTION =====
function updateDir(){
  let t='';
  if(joyY>20)t='ILERI';else if(joyY<-20)t='GERI';
  if(joyX>20)t+=(t?' + ':'')+'SAGA KAY';else if(joyX<-20)t+=(t?' + ':'')+'SOLA KAY';
  if(rot>0)t+=(t?' + ':'')+'SAGA DON';else if(rot<0)t+=(t?' + ':'')+'SOLA DON';
  if(!t)t='DUR';
  document.getElementById('dirText').textContent=t;
}

// ===== SEND =====
function sendControl(){
  if(currentMode==='auto')return;
  fetch('/control?x='+joyX+'&y='+joyY+'&r='+rot+'&s='+speed+'&h='+hornVal).catch(()=>{});
}
sendTimer=setInterval(()=>{if((joyActive||rot!==0||hornVal!==0)&&currentMode==='manual')sendControl();},SEND_INTERVAL);
setInterval(()=>{if(!joyActive&&rot===0&&hornVal===0&&currentMode==='manual')sendControl();},500);

// ===== PAN-TILT =====
function ptSlide(){
  panPos=parseInt(document.getElementById('panSlider').value);
  tiltPos=parseInt(document.getElementById('tiltSlider').value);
  document.getElementById('panVal').innerHTML=panPos+'&deg;';
  document.getElementById('tiltVal').innerHTML=tiltPos+'&deg;';
  fetch('/servo?pan='+panPos+'&tilt='+tiltPos).catch(()=>{});
}
function ptCenter(){
  panPos=90; tiltPos=90;
  document.getElementById('panSlider').value=90;
  document.getElementById('tiltSlider').value=90;
  document.getElementById('panVal').innerHTML='90&deg;';
  document.getElementById('tiltVal').innerHTML='90&deg;';
  fetch('/servo?pan=90&tilt=90').catch(()=>{});
}
</script>
</body>
</html>
)rawliteral";

#endif
