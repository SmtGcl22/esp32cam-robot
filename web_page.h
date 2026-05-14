#ifndef WEB_PAGE_H
#define WEB_PAGE_H
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="tr"><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>KesisRobot</title>
<style>
*,*::before,*::after{margin:0;padding:0;box-sizing:border-box}
:root{--bg:#0a0a12;--card:rgba(15,10,25,0.92);--border:rgba(255,45,123,0.2);
--neon:#ff2d7b;--cyan:#00f0ff;--yellow:#f7ff00;--orange:#ff6b2b;
--glow:rgba(255,45,123,0.4);--danger:#ff1744;--success:#00e676;
--txt:#e8e0f0;--dim:#6a5a7a;--r:10px}
html,body{height:100%;overflow-x:hidden;touch-action:manipulation}
body{font-family:Consolas,'Courier New',monospace;background:var(--bg);color:var(--txt)}
body::before{content:'';position:fixed;inset:0;background:
repeating-linear-gradient(0deg,transparent,transparent 2px,rgba(255,45,123,0.03) 2px,rgba(255,45,123,0.03) 4px),
radial-gradient(ellipse at 30% 0%,rgba(255,45,123,0.08),transparent 60%),
radial-gradient(ellipse at 70% 100%,rgba(0,240,255,0.06),transparent 60%);pointer-events:none;z-index:0}
.app{display:flex;flex-direction:column;min-height:100vh;max-width:960px;margin:0 auto;padding:6px 10px;position:relative;z-index:1}
.hdr{text-align:center;padding:8px 0 4px}
.hdr h1{font-size:1.3rem;font-weight:900;letter-spacing:6px;color:var(--neon);text-transform:uppercase;
text-shadow:0 0 10px var(--glow),0 0 30px rgba(255,45,123,0.2)}
.hdr .sub{font-size:.55rem;color:var(--cyan);letter-spacing:5px;text-transform:uppercase}
.badges{display:flex;justify-content:center;gap:8px;margin-top:6px}
.badge{display:inline-flex;align-items:center;gap:4px;padding:3px 12px;border-radius:20px;font-size:.55rem;font-weight:700;letter-spacing:2px}
.badge.ok{background:rgba(0,230,118,0.1);border:1px solid rgba(0,230,118,0.3);color:var(--success)}
.badge.esp{background:rgba(0,240,255,0.08);border:1px solid rgba(0,240,255,0.25);color:var(--cyan)}
.dot{width:6px;height:6px;border-radius:50%;background:currentColor;animation:pulse 2s infinite}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.3}}
.toolbar{display:flex;gap:4px;margin:6px 0;flex-wrap:wrap;align-items:center}
.tb-btn{padding:7px 10px;border-radius:8px;font-size:.55rem;font-weight:700;letter-spacing:1px;
cursor:pointer;border:1px solid var(--border);background:var(--card);color:var(--dim);transition:.2s;
font-family:inherit;text-transform:uppercase;user-select:none}
.tb-btn.on{color:var(--cyan);border-color:rgba(0,240,255,0.4);background:rgba(0,240,255,0.08)}
.tb-btn.rst{color:var(--orange);border-color:rgba(255,107,43,0.3)}
.tb-sep{width:1px;height:20px;background:var(--border);margin:0 2px}
.tb-btn.prof{color:var(--yellow);border-color:rgba(247,255,0,0.2)}
.tb-btn.prof.active{border-color:var(--yellow);background:rgba(247,255,0,0.15);box-shadow:0 0 8px rgba(247,255,0,0.2)}
.tb-btn.save{color:var(--success);border-color:rgba(0,230,118,0.3)}
.tb-btn.save.saving{color:#fff;background:rgba(0,230,118,0.3);animation:pulse 1s infinite}
.tb-right{margin-left:auto;display:flex;gap:4px}
.mode-switch{display:flex;gap:0;margin:6px 0;border-radius:var(--r);overflow:hidden;border:1px solid var(--border)}
.mode-btn{flex:1;padding:12px 0;text-align:center;font-size:.7rem;font-weight:700;letter-spacing:3px;
text-transform:uppercase;cursor:pointer;transition:.3s;background:var(--card);color:var(--dim);border:none;font-family:inherit}
.mode-btn.active-manual{background:linear-gradient(135deg,rgba(255,45,123,0.2),rgba(255,45,123,0.05));
color:var(--neon);box-shadow:inset 0 -3px 0 var(--neon)}
.mode-btn.active-auto{background:linear-gradient(135deg,rgba(247,255,0,0.15),rgba(247,255,0,0.03));
color:var(--yellow);box-shadow:inset 0 -3px 0 var(--yellow)}
.panel{background:var(--card);border:1px solid var(--border);border-radius:var(--r);margin:5px 0;
transition:box-shadow .2s;overflow:visible}
.panel.floating{position:fixed;z-index:100;box-shadow:0 0 30px rgba(0,0,0,0.6),0 0 10px var(--glow);min-width:200px}
.panel.hidden{display:none}
.p-hdr{display:flex;align-items:center;padding:8px 12px;cursor:grab;user-select:none;
background:rgba(255,45,123,0.05);border-bottom:1px solid var(--border);border-radius:var(--r) var(--r) 0 0}
.p-hdr:active{cursor:grabbing}
.p-title{flex:1;font-size:.6rem;font-weight:700;letter-spacing:3px;text-transform:uppercase;color:var(--cyan);
text-shadow:0 0 6px rgba(0,240,255,0.3)}
.p-btn{width:26px;height:26px;border-radius:6px;display:flex;align-items:center;justify-content:center;
cursor:pointer;font-size:.75rem;color:var(--dim);transition:.15s;border:none;background:transparent;margin-left:3px}
.p-btn:hover,.p-btn:active{color:var(--neon);background:rgba(255,45,123,0.1)}
.p-body{padding:10px 12px;overflow:hidden;transform-origin:top center;transition:transform .1s}
.p-body.collapsed{display:none}
.p-zoom-hint{position:absolute;bottom:4px;right:8px;font-size:.4rem;color:var(--dim);letter-spacing:1px;pointer-events:none;opacity:.5}
.panels-grid{display:flex;flex-direction:column;gap:6px}
@media(min-width:700px){.panels-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;align-items:start}}
.cam-wrap{position:relative;width:100%;aspect-ratio:4/3;background:#08060f;border-radius:8px;overflow:hidden}
.cam-wrap img{width:100%;height:100%;object-fit:cover;display:block}
.cam-off{position:absolute;inset:0;display:flex;flex-direction:column;align-items:center;justify-content:center;
gap:6px;color:var(--dim);font-size:.7rem;letter-spacing:3px}
.cam-off span:first-child{font-size:2rem;opacity:.4}
.cam-hud{position:absolute;top:6px;left:8px;font-size:.5rem;color:var(--neon);letter-spacing:3px;opacity:.8;text-shadow:0 0 6px var(--glow)}
.cam-hud.r{left:auto;right:8px;color:var(--cyan)}
.flash-btn{position:absolute;bottom:8px;right:8px;width:38px;height:38px;border-radius:50%;
background:rgba(0,0,0,0.6);border:1px solid rgba(255,255,255,0.15);color:#fff;font-size:1rem;
display:flex;align-items:center;justify-content:center;cursor:pointer;transition:.2s}
.flash-btn.on{background:rgba(247,255,0,0.25);border-color:var(--yellow);color:var(--yellow);box-shadow:0 0 12px rgba(247,255,0,0.3)}
.target-indicator{position:absolute;top:50%;left:50%;width:44px;height:44px;border:2px solid var(--yellow);
border-radius:50%;transform:translate(-50%,-50%);display:none;pointer-events:none;box-shadow:0 0 15px rgba(247,255,0,0.4)}
.target-indicator.show{display:block}
.crosshair{position:absolute;top:50%;left:50%;width:14px;height:14px;transform:translate(-50%,-50%)}
.crosshair::before,.crosshair::after{content:'';position:absolute;background:var(--yellow)}
.crosshair::before{width:14px;height:1px;top:50%;left:0;transform:translateY(-50%)}
.crosshair::after{width:1px;height:14px;left:50%;top:0;transform:translateX(-50%)}
.ctrl-mode{display:flex;gap:0;margin-bottom:8px;border-radius:8px;overflow:hidden;border:1px solid var(--border)}
.cm-btn{flex:1;padding:10px;text-align:center;font-size:.6rem;font-weight:700;letter-spacing:2px;
cursor:pointer;background:var(--card);color:var(--dim);border:none;font-family:inherit;transition:.2s}
.cm-btn.active{color:var(--neon);background:rgba(255,45,123,0.12);box-shadow:inset 0 -2px 0 var(--neon)}
.ctrl{display:flex;align-items:center;justify-content:center;gap:14px;padding:4px 0}
.rot-btn{width:68px;height:68px;border-radius:50%;background:rgba(255,107,43,0.08);
border:2px solid rgba(255,107,43,0.3);color:var(--orange);font-size:1.5rem;
display:flex;align-items:center;justify-content:center;cursor:pointer;user-select:none;
touch-action:manipulation;transition:.15s;flex-shrink:0}
.rot-btn:active,.rot-btn.active{background:rgba(255,107,43,0.25);border-color:var(--orange);
box-shadow:0 0 20px rgba(255,107,43,0.4);transform:scale(.92)}
.rot-label{font-size:.45rem;color:var(--dim);text-align:center;margin-top:3px;letter-spacing:2px;text-transform:uppercase}
.joy-wrap{position:relative;flex-shrink:0}
.joy-canvas{display:block;touch-action:none}
.joy-info{position:absolute;bottom:-14px;left:50%;transform:translateX(-50%);font-size:.5rem;color:var(--cyan);letter-spacing:2px;white-space:nowrap}
.dpad{display:grid;grid-template-columns:repeat(3,1fr);gap:5px;max-width:220px;margin:0 auto}
.dpad-btn{height:56px;border-radius:8px;background:rgba(0,240,255,0.06);border:1.5px solid rgba(0,240,255,0.2);
color:var(--cyan);font-size:1.3rem;display:flex;align-items:center;justify-content:center;
cursor:pointer;user-select:none;touch-action:manipulation;transition:.15s}
.dpad-btn:active,.dpad-btn.active{background:rgba(0,240,255,0.2);border-color:var(--cyan);
box-shadow:0 0 15px rgba(0,240,255,0.3);transform:scale(.94)}
.dpad-btn.stop{background:rgba(255,45,123,0.08);border-color:rgba(255,45,123,0.3);color:var(--neon)}
.dpad-rot{display:flex;gap:5px;margin-top:6px}
.dpad-rot .rot-sm{flex:1;height:42px;border-radius:8px;background:rgba(255,107,43,0.06);
border:1.5px solid rgba(255,107,43,0.25);color:var(--orange);font-size:.65rem;font-weight:700;
display:flex;align-items:center;justify-content:center;gap:4px;cursor:pointer;user-select:none;
touch-action:manipulation;transition:.15s;font-family:inherit;letter-spacing:1px}
.dpad-rot .rot-sm:active,.dpad-rot .rot-sm.active{background:rgba(255,107,43,0.2);transform:scale(.95)}
.speed-row{display:flex;align-items:center;gap:10px;padding:8px 14px;background:rgba(255,255,255,0.02);
border:1px solid var(--border);border-radius:8px;margin-top:8px}
.speed-row label{font-size:.6rem;color:var(--dim);letter-spacing:3px;text-transform:uppercase;flex-shrink:0}
.speed-row input[type=range]{flex:1;-webkit-appearance:none;height:6px;border-radius:3px;background:rgba(255,45,123,0.15);outline:none}
.speed-row input::-webkit-slider-thumb{-webkit-appearance:none;width:28px;height:28px;border-radius:50%;
background:linear-gradient(135deg,var(--neon),var(--orange));box-shadow:0 0 12px var(--glow);
border:2px solid rgba(255,255,255,0.2);cursor:pointer}
.speed-row .val{font-size:.8rem;font-weight:700;color:var(--neon);min-width:30px;text-align:right}
.dir-display{text-align:center;padding:3px 0}
.dir-text{font-size:.7rem;font-weight:700;color:var(--cyan);letter-spacing:4px;text-transform:uppercase;
text-shadow:0 0 8px rgba(0,240,255,0.3)}
.horn-btn{width:100%;height:56px;border-radius:var(--r);margin-bottom:6px;
background:rgba(255,23,68,0.08);border:2px solid rgba(255,23,68,0.3);color:var(--danger);
font-size:1rem;font-weight:700;letter-spacing:3px;display:flex;align-items:center;justify-content:center;
gap:8px;cursor:pointer;user-select:none;touch-action:manipulation;transition:.15s;font-family:inherit}
.horn-btn:active,.horn-btn.active{background:rgba(255,23,68,0.3);border-color:var(--danger);
box-shadow:0 0 20px rgba(255,23,68,0.4);transform:scale(.98)}
.pt-row{display:flex;align-items:center;gap:6px;margin-bottom:8px}
.pt-row label{font-size:.55rem;color:var(--dim);letter-spacing:2px;text-transform:uppercase;min-width:30px;flex-shrink:0}
.pt-row input[type=range]{flex:1;-webkit-appearance:none;height:8px;border-radius:4px;background:rgba(0,240,255,0.1);outline:none}
.pt-row input::-webkit-slider-thumb{-webkit-appearance:none;width:30px;height:30px;border-radius:50%;
background:linear-gradient(135deg,var(--cyan),#0080a0);box-shadow:0 0 12px rgba(0,240,255,0.4);
border:2px solid rgba(255,255,255,0.2);cursor:pointer}
.pt-val{font-size:.75rem;font-weight:700;color:var(--cyan);min-width:34px;text-align:right}
.pt-adj{width:34px;height:34px;border-radius:8px;background:rgba(0,240,255,0.08);border:1px solid rgba(0,240,255,0.2);
color:var(--cyan);font-size:1rem;font-weight:700;display:flex;align-items:center;justify-content:center;
cursor:pointer;font-family:inherit;transition:.15s;flex-shrink:0}
.pt-adj:active{background:rgba(0,240,255,0.25);transform:scale(.92)}
.pt-presets{display:flex;gap:5px;margin-top:6px}
.pt-pre{flex:1;padding:10px;border-radius:8px;background:rgba(0,240,255,0.06);border:1px solid rgba(0,240,255,0.15);
color:var(--cyan);font-size:.55rem;font-weight:700;letter-spacing:2px;text-transform:uppercase;
cursor:pointer;text-align:center;font-family:inherit;transition:.15s}
.pt-pre:active{background:rgba(0,240,255,0.2);transform:scale(.97)}
.pt-pre.ctr{border-color:rgba(255,45,123,0.3);color:var(--neon);background:rgba(255,45,123,0.06)}
.dash-grid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:6px}
.dash-item{background:rgba(255,255,255,0.02);border-radius:8px;padding:10px 6px;text-align:center;
border:1px solid rgba(255,255,255,0.05)}
.dash-item.alert{border-color:rgba(255,23,68,0.5);background:rgba(255,23,68,0.08);animation:alertPulse 1s infinite}
@keyframes alertPulse{0%,100%{opacity:1}50%{opacity:.6}}
.dash-icon{font-size:1.1rem;margin-bottom:2px}
.dash-val{font-size:1.1rem;font-weight:800;color:var(--cyan);line-height:1.2}
.dash-item.alert .dash-val{color:var(--danger)}
.dash-label{font-size:.4rem;color:var(--dim);letter-spacing:2px;text-transform:uppercase;margin-top:2px}
.dash-alert{display:none;margin-top:8px;padding:8px 12px;border-radius:8px;background:rgba(255,23,68,0.12);
border:1px solid rgba(255,23,68,0.4);color:var(--danger);font-size:.6rem;font-weight:700;text-align:center;animation:alertPulse 1s infinite}
.dash-alert.show{display:block}
.gauge-wrap{width:66px;height:38px;margin:0 auto 2px}
.gauge-wrap canvas{display:block}
.auto-panel{display:none}
.auto-panel.show{display:block}
.auto-inner{background:var(--card);border:1px solid rgba(247,255,0,0.2);border-radius:var(--r);padding:14px;margin:5px 0}
.auto-inner h3{font-size:.65rem;color:var(--yellow);letter-spacing:3px;text-transform:uppercase;margin-bottom:8px;
text-shadow:0 0 8px rgba(247,255,0,0.3)}
.color-row{display:flex;gap:8px;margin-bottom:10px}
.color-btn{width:42px;height:42px;border-radius:50%;border:2px solid rgba(255,255,255,0.1);cursor:pointer;transition:.2s}
.color-btn.sel{border-color:#fff;transform:scale(1.15);box-shadow:0 0 14px rgba(255,255,255,0.3)}
.color-btn[data-c="0"]{background:#ff1744}
.color-btn[data-c="1"]{background:#2979ff}
.color-btn[data-c="2"]{background:#ffd600}
.auto-status{display:flex;gap:8px;flex-wrap:wrap}
.auto-stat{flex:1;min-width:60px;background:rgba(255,255,255,0.03);border-radius:8px;padding:6px;text-align:center;
border:1px solid rgba(247,255,0,0.1)}
.auto-stat .label{font-size:.45rem;color:var(--dim);letter-spacing:1px;text-transform:uppercase}
.auto-stat .value{font-size:.85rem;font-weight:700;color:var(--yellow);margin-top:2px}
.ftr{text-align:center;font-size:.45rem;color:var(--dim);letter-spacing:2px;padding:6px 0}
</style></head><body>
<div class="app">
<div class="hdr">
<h1>&#9881; Kesis-Robot</h1>
<div class="sub">Mecanum Kontrol Merkezi</div>
<div class="badges">
<span class="badge ok"><span class="dot"></span>WiFi</span>
<span class="badge esp"><span class="dot"></span>ESP-NOW</span>
</div></div>

<div class="toolbar" id="toolbar">
<div class="tb-btn on" onclick="togglePanelVis('pCam')">&#128247; Goruntu</div>
<div class="tb-btn on" onclick="togglePanelVis('pCtrl')">&#127918; Kontrol</div>
<div class="tb-btn on" onclick="togglePanelVis('pServo')">&#128249; Servo</div>
<div class="tb-btn on" onclick="togglePanelVis('pDash')">&#128200; Gosterge</div>
<div class="tb-sep"></div>
<div class="tb-btn prof" id="prof1" onclick="profAction(1)">P1</div>
<div class="tb-btn prof" id="prof2" onclick="profAction(2)">P2</div>
<div class="tb-btn prof" id="prof3" onclick="profAction(3)">P3</div>
<div class="tb-btn save" id="saveBtn" onclick="toggleSaveMode()">&#128190;</div>
<div class="tb-right">
<div class="tb-btn rst" onclick="resetLayout()">&#8634; Sifirla</div>
</div></div>

<div class="mode-switch">
<div class="mode-btn active-manual" id="btnManual" onclick="setMode('manual')">&#9881; Manuel</div>
<div class="mode-btn" id="btnAuto" onclick="setMode('auto')">&#9678; Otonom</div>
</div>

<div class="auto-panel" id="autoPanel">
<div class="auto-inner">
<h3>&#127919; Otonom Mod</h3>
<div class="color-row">
<div class="color-btn sel" data-c="0" onclick="pickColor(0,this)"></div>
<div class="color-btn" data-c="1" onclick="pickColor(1,this)"></div>
<div class="color-btn" data-c="2" onclick="pickColor(2,this)"></div>
</div>
<div class="auto-status">
<div class="auto-stat"><div class="label">Durum</div><div class="value" id="aState">ARANIYOR</div></div>
<div class="auto-stat"><div class="label">Alan</div><div class="value" id="aArea">0</div></div>
<div class="auto-stat"><div class="label">FPS</div><div class="value" id="aFps">0</div></div>
<div class="auto-stat"><div class="label">Konum</div><div class="value" id="aPos">-</div></div>
</div></div></div>

<div class="panels-grid" id="panelsGrid">

<div class="panel" id="pCam">
<div class="p-hdr"><span class="p-title">&#128247; Goruntu</span>
<div class="p-btn" onclick="toggleCollapse('pCam')">&#9660;</div>
<div class="p-btn" onclick="hidePanel('pCam')">&#10005;</div></div>
<div class="p-body" id="pCamBody">
<div class="cam-wrap" id="camWrap">
<img id="camImg" src="" style="display:none" alt="Canli Yayin">
<div class="cam-off" id="camOff"><span>&#128247;</span><span>Kamera Yukleniyor...</span></div>
<div class="cam-hud">&#9679; REC</div>
<div class="cam-hud r" id="fpsHud">--FPS</div>
<div class="flash-btn" id="flashBtn" onclick="toggleFlash()">&#128294;</div>
<div class="target-indicator" id="targetInd"><div class="crosshair"></div></div>
</div></div></div>

<div class="panel" id="pCtrl">
<div class="p-hdr"><span class="p-title">&#127918; Kontrol</span>
<div class="p-btn" onclick="toggleCollapse('pCtrl')">&#9660;</div>
<div class="p-btn" onclick="hidePanel('pCtrl')">&#10005;</div></div>
<div class="p-body" id="pCtrlBody">
<div class="ctrl-mode">
<div class="cm-btn active" id="cmJoy" onclick="setCtrlMode('joy')">&#128377; Joystick</div>
<div class="cm-btn" id="cmBtn" onclick="setCtrlMode('btn')">&#9632; Butonlar</div>
</div>
<div class="dir-display"><div class="dir-text" id="dirText">DUR</div></div>
<div class="horn-btn" id="hornBtn"
ontouchstart="setHorn(1);event.preventDefault()" onmousedown="setHorn(1)"
ontouchend="setHorn(0)" onmouseup="setHorn(0)" onmouseleave="setHorn(0)">&#128227; KORNA</div>
<div id="joyView">
<div class="ctrl">
<div style="text-align:center">
<div class="rot-btn" id="rotL" ontouchstart="startRot(-1,this);event.preventDefault()"
onmousedown="startRot(-1,this)" ontouchend="stopRot(this)" onmouseup="stopRot(this)" onmouseleave="stopRot(this)">&#8634;</div>
<div class="rot-label">Sol</div></div>
<div class="joy-wrap">
<canvas class="joy-canvas" id="joyCanvas" width="210" height="210"></canvas>
<div class="joy-info" id="joyInfo">X:0 Y:0</div></div>
<div style="text-align:center">
<div class="rot-btn" id="rotR" ontouchstart="startRot(1,this);event.preventDefault()"
onmousedown="startRot(1,this)" ontouchend="stopRot(this)" onmouseup="stopRot(this)" onmouseleave="stopRot(this)">&#8635;</div>
<div class="rot-label">Sag</div></div></div></div>
<div id="btnView" style="display:none">
<div class="dpad">
<div class="dpad-btn" onmousedown="dpadOn(-70,70,this)" ontouchstart="dpadOn(-70,70,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#8598;</div>
<div class="dpad-btn" onmousedown="dpadOn(0,100,this)" ontouchstart="dpadOn(0,100,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#9650;</div>
<div class="dpad-btn" onmousedown="dpadOn(70,70,this)" ontouchstart="dpadOn(70,70,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#8599;</div>
<div class="dpad-btn" onmousedown="dpadOn(-100,0,this)" ontouchstart="dpadOn(-100,0,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#9664;</div>
<div class="dpad-btn stop" onmousedown="dpadOn(0,0,this)" ontouchstart="dpadOn(0,0,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#9632;</div>
<div class="dpad-btn" onmousedown="dpadOn(100,0,this)" ontouchstart="dpadOn(100,0,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#9654;</div>
<div class="dpad-btn" onmousedown="dpadOn(-70,-70,this)" ontouchstart="dpadOn(-70,-70,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#8601;</div>
<div class="dpad-btn" onmousedown="dpadOn(0,-100,this)" ontouchstart="dpadOn(0,-100,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#9660;</div>
<div class="dpad-btn" onmousedown="dpadOn(70,-70,this)" ontouchstart="dpadOn(70,-70,this);event.preventDefault()" onmouseup="dpadOff(this)" ontouchend="dpadOff(this)" onmouseleave="dpadOff(this)">&#8600;</div>
</div>
<div class="dpad-rot">
<div class="rot-sm" id="rotL2" ontouchstart="startRot(-1,this);event.preventDefault()" onmousedown="startRot(-1,this)"
ontouchend="stopRot(this)" onmouseup="stopRot(this)" onmouseleave="stopRot(this)">&#8634; SOL</div>
<div class="rot-sm" id="rotR2" ontouchstart="startRot(1,this);event.preventDefault()" onmousedown="startRot(1,this)"
ontouchend="stopRot(this)" onmouseup="stopRot(this)" onmouseleave="stopRot(this)">SAG &#8635;</div>
</div></div>
<div class="speed-row">
<label>Hiz</label>
<input type="range" id="speedSlider" min="80" max="255" value="200" oninput="onSpeed(this.value)">
<span class="val" id="speedVal">200</span>
</div></div></div>

<div class="panel" id="pDash">
<div class="p-hdr"><span class="p-title">&#128200; Gosterge</span>
<div class="p-btn" onclick="toggleCollapse('pDash')">&#9660;</div>
<div class="p-btn" onclick="hidePanel('pDash')">&#10005;</div></div>
<div class="p-body" id="pDashBody">
<div class="dash-grid">
<div class="dash-item" id="gasItem"><div class="dash-icon">&#9729;</div>
<div class="gauge-wrap"><canvas id="gasGauge" width="66" height="38"></canvas></div>
<div class="dash-val" id="gasVal">--</div><div class="dash-label">Gaz</div></div>
<div class="dash-item"><div class="dash-icon">&#128168;</div><div class="dash-val" id="dashSpeed">200</div><div class="dash-label">Hiz</div></div>
<div class="dash-item"><div class="dash-icon">&#127912;</div><div class="dash-val" id="dashDir" style="font-size:.65rem">DUR</div><div class="dash-label">Yon</div></div>
<div class="dash-item"><div class="dash-icon">&#128377;</div><div class="dash-val"><span id="dashJoyX" style="color:var(--neon)">0</span>/<span id="dashJoyY" style="color:var(--cyan)">0</span></div><div class="dash-label">Joystick</div></div>
<div class="dash-item"><div class="dash-icon">&#128249;</div><div class="dash-val"><span id="dashPan">90</span>&deg;</div><div class="dash-label">Pan</div></div>
<div class="dash-item"><div class="dash-icon">&#128260;</div><div class="dash-val"><span id="dashTilt">90</span>&deg;</div><div class="dash-label">Tilt</div></div>
</div>
<div class="dash-alert" id="dashAlert">&#9888; UYARI</div>
</div></div>

<div class="panel" id="pServo">
<div class="p-hdr"><span class="p-title">&#128249; Kamera Servo</span>
<div class="p-btn" onclick="toggleCollapse('pServo')">&#9660;</div>
<div class="p-btn" onclick="hidePanel('pServo')">&#10005;</div></div>
<div class="p-body" id="pServoBody">
<div class="pt-row"><label>Pan</label>
<div class="pt-adj" onclick="ptAdj('pan',-5)">-</div>
<input type="range" id="panSlider" min="0" max="180" value="90" oninput="ptSlide()">
<div class="pt-adj" onclick="ptAdj('pan',5)">+</div>
<span class="pt-val" id="panVal">90&deg;</span></div>
<div class="pt-row"><label>Tilt</label>
<div class="pt-adj" onclick="ptAdj('tilt',-5)">-</div>
<input type="range" id="tiltSlider" min="0" max="130" value="90" oninput="ptSlide()">
<div class="pt-adj" onclick="ptAdj('tilt',5)">+</div>
<span class="pt-val" id="tiltVal">90&deg;</span></div>
<div class="pt-presets">
<div class="pt-pre" onclick="ptSet(0,90)">Sol</div>
<div class="pt-pre ctr" onclick="ptSet(90,90)">&#8634; Orta</div>
<div class="pt-pre" onclick="ptSet(180,90)">Sag</div>
</div></div></div>

</div>
<div class="ftr">KESIS-ROBOT &copy; 2026 &mdash; CYBERPUNK MECANUM</div>
</div>
<script>
const SEND_INTERVAL=80,DEADZONE=8,AXIS_LOCK_T=0.45;
let joyX=0,joyY=0,rot=0,speed=200,hornVal=0,joyActive=false,flashOn=false,sendTimer=null;
let currentMode='manual',selectedColor=0,autoPolling=null,panPos=90,tiltPos=90,topZ=100;
let saveMode=false,activeProf=1;

// === PROFILES & LAYOUT ===
const defaultLayout = {
  pCam:{x:0,y:0,w:0,h:0,f:false,c:false,v:true,s:1},
  pCtrl:{x:0,y:0,w:0,h:0,f:false,c:false,v:true,s:1},
  pDash:{x:0,y:0,w:0,h:0,f:false,c:false,v:true,s:1},
  pServo:{x:0,y:0,w:0,h:0,f:false,c:false,v:true,s:1}
};

function getLayout(){
  let l={};
  ['pCam','pCtrl','pDash','pServo'].forEach(id=>{
    const p=document.getElementById(id),b=document.getElementById(id+'Body');
    l[id]={
      x:p.style.left||'0px',y:p.style.top||'0px',w:p.style.width||'auto',
      f:p.classList.contains('floating'),c:b?b.classList.contains('collapsed'):false,
      v:!p.classList.contains('hidden'),s:p.style.transform||'scale(1)'
    };
  });
  return l;
}

function applyLayout(l){
  ['pCam','pCtrl','pDash','pServo'].forEach(id=>{
    if(!l[id])return;
    const p=document.getElementById(id),b=document.getElementById(id+'Body');
    if(l[id].f){
      p.classList.add('floating');
      p.style.left=l[id].x;p.style.top=l[id].y;p.style.width=l[id].w;
      p.style.transform=l[id].s;
    }else{
      p.classList.remove('floating');
      p.style.cssText='';
    }
    if(b) b.classList.toggle('collapsed',l[id].c);
    p.classList.toggle('hidden',!l[id].v);
  });
  updateToolbar();
}

function updateToolbar(){
  ['pCam','pCtrl','pDash','pServo'].forEach(id=>{
    const p=document.getElementById(id);
    document.querySelectorAll('.toolbar .tb-btn').forEach(b=>{
      if(b.textContent.includes(getPanelShort(id))) b.classList.toggle('on',!p.classList.contains('hidden'));
    });
  });
}

function toggleSaveMode(){
  saveMode=!saveMode;
  const b=document.getElementById('saveBtn');
  if(saveMode){
    b.classList.add('saving');
    b.innerHTML='&#128190; SEC..';
  }else{
    b.classList.remove('saving');
    b.innerHTML='&#128190;';
  }
}

function profAction(pNum){
  if(saveMode){
    localStorage.setItem('kr_prof_'+pNum,JSON.stringify(getLayout()));
    toggleSaveMode();
    activateProf(pNum);
  }else{
    const d=localStorage.getItem('kr_prof_'+pNum);
    if(d) applyLayout(JSON.parse(d));
    activateProf(pNum);
  }
}

function activateProf(pNum){
  activeProf=pNum;
  [1,2,3].forEach(i=>document.getElementById('prof'+i).classList.toggle('active',i===pNum));
}

function initLayout(){
  const d=localStorage.getItem('kr_prof_1');
  if(d) {applyLayout(JSON.parse(d)); activateProf(1);}
  else activateProf(1);
}

// === PANEL SYSTEM ===
function toggleCollapse(id){
  const b=document.getElementById(id+'Body');
  if(b)b.classList.toggle('collapsed');
}
function hidePanel(id){
  document.getElementById(id).classList.add('hidden');
  updateToolbar();
}
function togglePanelVis(id){
  document.getElementById(id).classList.toggle('hidden');
  updateToolbar();
}
function getPanelShort(id){return{pCam:'Goruntu',pCtrl:'Kontrol',pServo:'Servo',pDash:'Gosterge'}[id]||'';}
function resetLayout(){
  applyLayout(defaultLayout);
  ['pCam','pCtrl','pDash','pServo'].forEach(id=>{document.getElementById(id).style.cssText='';});
  updateToolbar();
  if(currentMode==='auto') setMode('auto');
}

// === DRAG & RESIZE (SCALE) ===
document.querySelectorAll('.panel').forEach(p=>{
  const hdr=p.querySelector('.p-hdr');
  if(!hdr)return;
  // Dragging
  let sx,sy,ox,oy,dragging=false,resizing=false;
  let oW,oScale,bodyScale=1;
  
  function onDragStart(e){
    if(e.target.closest('.p-btn'))return;
    const t=e.touches?e.touches[0]:e;
    const rect=p.getBoundingClientRect();
    
    // Check if clicking near bottom-right for resize
    if(t.clientX > rect.right-30 && t.clientY > rect.bottom-30){
      resizing=true;
      sx=t.clientX;sy=t.clientY;
      const tform = p.style.transform;
      oScale = tform.includes('scale') ? parseFloat(tform.split('scale(')[1]) : 1;
      oW = rect.width / oScale;
    } else {
      dragging=true;
      sx=t.clientX;sy=t.clientY;ox=rect.left;oy=rect.top;
    }
    
    const mf=ev=>{
      const tt=ev.touches?ev.touches[0]:ev;
      ev.preventDefault();
      if(dragging){
        if(!p.classList.contains('floating')){
          const currRect = p.getBoundingClientRect();
          p.classList.add('floating');
          p.style.width=(currRect.width)+'px';
          // maintain visual scale when switching to floating
          const currScale = p.style.transform.includes('scale') ? parseFloat(p.style.transform.split('scale(')[1]) : 1;
          ox = currRect.left; oy = currRect.top;
          sx = tt.clientX; sy = tt.clientY;
        }
        p.style.left=(ox+(tt.clientX-sx))+'px';p.style.top=(oy+(tt.clientY-sy))+'px';
        p.style.zIndex=++topZ;
      }else if(resizing){
        if(!p.classList.contains('floating')){
           const currRect = p.getBoundingClientRect();
           p.classList.add('floating');
           p.style.left=currRect.left+'px'; p.style.top=currRect.top+'px';
           p.style.width=(currRect.width/oScale)+'px';
        }
        const dx = tt.clientX - sx;
        let newScale = oScale + (dx / oW);
        newScale = Math.max(0.5, Math.min(2.5, newScale));
        p.style.transform = `scale(${newScale})`;
        p.style.transformOrigin = 'top left';
      }
    };
    const ef=()=>{
      document.removeEventListener('mousemove',mf);document.removeEventListener('mouseup',ef);
      document.removeEventListener('touchmove',mf);document.removeEventListener('touchend',ef);
      dragging=false; resizing=false;
    };
    document.addEventListener('mousemove',mf);document.addEventListener('mouseup',ef);
    document.addEventListener('touchmove',mf,{passive:false});document.addEventListener('touchend',ef);
  }
  hdr.addEventListener('mousedown',onDragStart);
  hdr.addEventListener('touchstart',onDragStart,{passive:false});
  
  // Add resize handle hint visually
  const b=p.querySelector('.p-body');
  if(b){
    const hnt = document.createElement('div');
    hnt.className='p-zoom-hint';
    hnt.innerHTML='&#8690;';
    b.appendChild(hnt);
    b.addEventListener('mousedown', (e)=>{
       const rect=p.getBoundingClientRect();
       if(e.clientX > rect.right-30 && e.clientY > rect.bottom-30) {
           e.stopPropagation();
           onDragStart(e);
       }
    });
    b.addEventListener('touchstart', (e)=>{
       const t=e.touches[0];
       const rect=p.getBoundingClientRect();
       if(t.clientX > rect.right-30 && t.clientY > rect.bottom-30) {
           e.stopPropagation();
           onDragStart(e);
       }
    },{passive:false});
  }
});

// === MODE ===
function setMode(mode){
  currentMode=mode;
  if(mode==='auto'){
    document.getElementById('btnManual').className='mode-btn';
    document.getElementById('btnAuto').className='mode-btn active-auto';
    document.getElementById('autoPanel').classList.add('show');
    // Hide control panel entirely
    document.getElementById('pCtrl').classList.add('hidden');
    // Ensure Servo panel is visible
    document.getElementById('pServo').classList.remove('hidden');
    
    joyX=0;joyY=0;rot=0;sendControl();
    fetch('/automode?mode=1&color='+selectedColor).catch(()=>{});
    startAutoPolling();
  } else {
    document.getElementById('btnAuto').className='mode-btn';
    document.getElementById('btnManual').className='mode-btn active-manual';
    document.getElementById('autoPanel').classList.remove('show');
    // Show control panel again
    document.getElementById('pCtrl').classList.remove('hidden');
    
    fetch('/automode?mode=0').catch(()=>{});
    stopAutoPolling();
    document.getElementById('targetInd').classList.remove('show');
  }
  updateToolbar();
}

function pickColor(c,el){
  selectedColor=c;
  document.querySelectorAll('.color-btn').forEach(b=>b.classList.remove('sel'));
  el.classList.add('sel');
  if(currentMode==='auto')fetch('/automode?mode=1&color='+c).catch(()=>{});
}
function startAutoPolling(){if(autoPolling)return;autoPolling=setInterval(pollTarget,300);}
function stopAutoPolling(){if(autoPolling){clearInterval(autoPolling);autoPolling=null;}}
function pollTarget(){
  fetch('/target').then(r=>r.json()).then(d=>{
    const ind=document.getElementById('targetInd');
    document.getElementById('aFps').textContent=d.fps||0;
    document.getElementById('aArea').textContent=d.area||0;
    if(d.found){
      document.getElementById('aState').textContent='BULUNDU';
      document.getElementById('aState').style.color='#00e676';
      document.getElementById('aPos').textContent=d.cx+','+d.cy;
      ind.style.left=(d.cx/320)*100+'%';ind.style.top=(d.cy/240)*100+'%';
      ind.classList.add('show');
    } else {
      document.getElementById('aState').textContent='ARANIYOR';
      document.getElementById('aState').style.color='var(--yellow)';
      document.getElementById('aPos').textContent='-';
      ind.classList.remove('show');
    }
  }).catch(()=>{});
}

// === CAMERA ===
const camImg=document.getElementById('camImg'),camOff=document.getElementById('camOff');
const streamUrl='http://'+location.hostname+':81/stream';
camImg.onload=()=>{camOff.style.display='none';camImg.style.display='block';};
camImg.onerror=()=>{setTimeout(()=>{camImg.src=streamUrl+'?t='+Date.now();},2000);};
setTimeout(()=>{camImg.src=streamUrl;},500);
function toggleFlash(){flashOn=!flashOn;document.getElementById('flashBtn').classList.toggle('on',flashOn);fetch('/flash?v='+(flashOn?1:0)).catch(()=>{});}

// === CTRL MODE ===
function setCtrlMode(m){
  document.getElementById('joyView').style.display=m==='joy'?'block':'none';
  document.getElementById('btnView').style.display=m==='btn'?'block':'none';
  document.getElementById('cmJoy').classList.toggle('active',m==='joy');
  document.getElementById('cmBtn').classList.toggle('active',m==='btn');
  if(m==='joy'){joyX=0;joyY=0;rot=0;updateDir();sendControl();}
}

// === DPAD ===
function dpadOn(x,y,el){joyX=x;joyY=y;el.classList.add('active');updateDir();updateDash();sendControl();}
function dpadOff(el){joyX=0;joyY=0;el.classList.remove('active');updateDir();updateDash();sendControl();}

// === JOYSTICK ===
const canvas=document.getElementById('joyCanvas'),ctx=canvas.getContext('2d');
const W=canvas.width,H=canvas.height,CX=W/2,CY=H/2,BASE_R=90,KNOB_R=34;
let knobX=CX,knobY=CY;

function drawJoy(){
  ctx.clearRect(0,0,W,H);
  ctx.beginPath();ctx.arc(CX,CY,BASE_R,0,Math.PI*2);
  ctx.strokeStyle='rgba(255,45,123,0.2)';ctx.lineWidth=2;ctx.stroke();
  ctx.strokeStyle='rgba(0,240,255,0.1)';ctx.lineWidth=1;
  ctx.beginPath();ctx.moveTo(CX,CY-BASE_R+10);ctx.lineTo(CX,CY+BASE_R-10);ctx.stroke();
  ctx.beginPath();ctx.moveTo(CX-BASE_R+10,CY);ctx.lineTo(CX+BASE_R-10,CY);ctx.stroke();
  ctx.fillStyle='rgba(0,240,255,0.25)';ctx.font='11px sans-serif';ctx.textAlign='center';
  ctx.fillText('\u25B2',CX,CY-BASE_R+16);ctx.fillText('\u25BC',CX,CY+BASE_R-8);
  ctx.fillText('\u25C0',CX-BASE_R+13,CY+4);ctx.fillText('\u25B6',CX+BASE_R-13,CY+4);
  if(joyActive){
    let g=ctx.createRadialGradient(knobX,knobY,0,knobX,knobY,KNOB_R*3);
    g.addColorStop(0,'rgba(255,45,123,0.15)');g.addColorStop(1,'transparent');
    ctx.fillStyle=g;ctx.fillRect(0,0,W,H);
  }
  let grad=ctx.createRadialGradient(knobX-3,knobY-3,0,knobX,knobY,KNOB_R);
  grad.addColorStop(0,joyActive?'#ff5090':'#a01050');
  grad.addColorStop(1,joyActive?'#c01060':'#601040');
  ctx.beginPath();ctx.arc(knobX,knobY,KNOB_R,0,Math.PI*2);
  ctx.fillStyle=grad;ctx.fill();
  ctx.strokeStyle=joyActive?'rgba(255,45,123,0.7)':'rgba(255,45,123,0.3)';
  ctx.lineWidth=2;ctx.stroke();
}
function getJoyPos(e){
  let rect=canvas.getBoundingClientRect(),t=e.touches?e.touches[0]:e;
  // Account for panel scaling!
  const p=document.getElementById('pCtrl');
  const tform = p.style.transform;
  const scale = tform.includes('scale') ? parseFloat(tform.split('scale(')[1]) : 1;
  
  let dx=(t.clientX-rect.left)/scale-CX,dy=(t.clientY-rect.top)/scale-CY;
  let dist=Math.sqrt(dx*dx+dy*dy),maxD=BASE_R-KNOB_R;
  if(dist>maxD){dx=dx/dist*maxD;dy=dy/dist*maxD;}
  let nx=Math.round(dx/maxD*100),ny=Math.round(-dy/maxD*100);
  let ax=Math.abs(nx),ay=Math.abs(ny);
  if(ax>DEADZONE||ay>DEADZONE){if(Math.min(ax,ay)/(Math.max(ax,ay)||1)<AXIS_LOCK_T){if(ax>ay)ny=0;else nx=0;}}
  return{kx:CX+dx,ky:CY+dy,nx:nx,ny:ny};
}
function joyDown(e){e.preventDefault();joyActive=true;joyMove(e);}
function joyMove(e){
  if(!joyActive)return;e.preventDefault();
  let p=getJoyPos(e);knobX=p.kx;knobY=p.ky;
  joyX=Math.abs(p.nx)<DEADZONE?0:p.nx;joyY=Math.abs(p.ny)<DEADZONE?0:p.ny;
  document.getElementById('joyInfo').textContent='X:'+joyX+' Y:'+joyY;
  updateDash();updateDir();drawJoy();
}
function joyUp(){joyActive=false;knobX=CX;knobY=CY;joyX=0;joyY=0;
  document.getElementById('joyInfo').textContent='X:0 Y:0';updateDash();updateDir();drawJoy();sendControl();}
canvas.addEventListener('mousedown',joyDown);canvas.addEventListener('mousemove',joyMove);
canvas.addEventListener('mouseup',joyUp);canvas.addEventListener('mouseleave',joyUp);
canvas.addEventListener('touchstart',joyDown,{passive:false});canvas.addEventListener('touchmove',joyMove,{passive:false});
canvas.addEventListener('touchend',joyUp);drawJoy();

function startRot(d,b){rot=d*100;b.classList.add('active');updateDir();updateDash();}
function stopRot(b){rot=0;b.classList.remove('active');updateDir();updateDash();}
function setHorn(v){hornVal=v;document.getElementById('hornBtn').classList.toggle('active',!!v);sendControl();}
function onSpeed(v){speed=parseInt(v);document.getElementById('speedVal').textContent=v;document.getElementById('dashSpeed').textContent=v;}
function updateDir(){
  let t='';
  if(joyY>20)t='ILERI';else if(joyY<-20)t='GERI';
  if(joyX>20)t+=(t?' + ':'')+'SAGA KAY';else if(joyX<-20)t+=(t?' + ':'')+'SOLA KAY';
  if(rot>0)t+=(t?' + ':'')+'SAGA DON';else if(rot<0)t+=(t?' + ':'')+'SOLA DON';
  if(!t)t='DUR';
  document.getElementById('dirText').textContent=t;
  document.getElementById('dashDir').textContent=t;
}
function sendControl(){if(currentMode==='auto')return;fetch('/control?x='+joyX+'&y='+joyY+'&r='+rot+'&s='+speed+'&h='+hornVal).catch(()=>{});}
sendTimer=setInterval(()=>{if((joyActive||rot!==0||hornVal!==0)&&currentMode==='manual')sendControl();},SEND_INTERVAL);
setInterval(()=>{if(!joyActive&&rot===0&&hornVal===0&&currentMode==='manual')sendControl();},500);

// === PAN-TILT ===
function ptSlide(){
  panPos=parseInt(document.getElementById('panSlider').value);
  tiltPos=parseInt(document.getElementById('tiltSlider').value);
  document.getElementById('panVal').innerHTML=panPos+'&deg;';
  document.getElementById('tiltVal').innerHTML=tiltPos+'&deg;';
  document.getElementById('dashPan').textContent=panPos;
  document.getElementById('dashTilt').textContent=tiltPos;
  fetch('/servo?pan='+panPos+'&tilt='+tiltPos).catch(()=>{});
}
function ptAdj(a,d){
  if(a==='pan'){panPos=Math.max(0,Math.min(180,panPos+d));document.getElementById('panSlider').value=panPos;}
  else{tiltPos=Math.max(0,Math.min(130,tiltPos+d));document.getElementById('tiltSlider').value=tiltPos;}
  ptSlide();
}
function ptSet(p,t){panPos=p;tiltPos=t;document.getElementById('panSlider').value=p;document.getElementById('tiltSlider').value=t;ptSlide();}

// === DASHBOARD ===
function updateDash(){document.getElementById('dashJoyX').textContent=joyX;document.getElementById('dashJoyY').textContent=joyY;}
function drawGasGauge(val){
  const c=document.getElementById('gasGauge');if(!c)return;
  const g=c.getContext('2d'),w=c.width,h=c.height;g.clearRect(0,0,w,h);
  const cx=w/2,cy=h-2,r=30;
  g.beginPath();g.arc(cx,cy,r,Math.PI,0);g.strokeStyle='rgba(255,255,255,0.08)';g.lineWidth=5;g.lineCap='round';g.stroke();
  let pct=Math.min(val/4095,1),angle=Math.PI+(Math.PI*pct);
  let color=pct<0.4?'#00e676':pct<0.7?'#ffd600':'#ff1744';
  g.beginPath();g.arc(cx,cy,r,Math.PI,angle);g.strokeStyle=color;g.lineWidth=5;g.lineCap='round';g.stroke();
}
let sensorPolling=null;
function startSensorPolling(){if(sensorPolling)return;sensorPolling=setInterval(pollSensors,1500);}
function pollSensors(){
  fetch('/sensors').then(r=>r.json()).then(d=>{
    if(d.gas!==undefined){document.getElementById('gasVal').textContent=d.gas;drawGasGauge(d.gas);}
    const gi=document.getElementById('gasItem'),ab=document.getElementById('dashAlert');
    if(d.gasAlert){gi.classList.add('alert');ab.innerHTML='&#9888; GAZ YUKSEK!';ab.classList.add('show');}
    else{gi.classList.remove('alert');ab.classList.remove('show');}
  }).catch(()=>{});
}
initLayout();
startSensorPolling();drawGasGauge(0);
</script></body></html>
)rawliteral";
#endif
