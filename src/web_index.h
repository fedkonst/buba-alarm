#pragma once
#include <Arduino.h>

const char PAGE_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru" data-theme="green">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no, viewport-fit=cover">
    <title>Volvo 850 - Telematics Dashboard</title>
    <style>
        :root[data-theme="modern"] {
            --bg-color: #101216;
            --card-bg: #1a1d24;
            --card-inner: #14161c;
            --border-color: #272b35;
            --accent-blue: #4a90e2;
            --accent-amber: #e6a23c;
            --accent-green: #41b883;
            --accent-red: #e74c3c;
            --text-main: #f0f2f5;
            --text-sub: #8c93a0;
            --text-glow: none;
        }

        :root[data-theme="green"] {
            --bg-color: #070d0a;
            --card-bg: #0d1712;
            --card-inner: #09100c;
            --border-color: #1b3827;
            --accent-blue: #00ff66;
            --accent-amber: #ffb300;
            --accent-green: #00ff66;
            --accent-red: #ff4d4d;
            --text-main: #33ff88;
            --text-sub: #1b9950;
            --text-glow: 0 0 6px rgba(51, 255, 136, 0.4);
        }

        :root[data-theme="amber"] {
            --bg-color: #120b06;
            --card-bg: #1f130a;
            --card-inner: #140c06;
            --border-color: #3d2412;
            --accent-blue: #ff9933;
            --accent-amber: #ffb300;
            --accent-green: #41b883;
            --accent-red: #e74c3c;
            --text-main: #ffaa55;
            --text-sub: #aa6622;
            --text-glow: 0 0 6px rgba(255, 170, 85, 0.3);
        }

        :root { --nav-height: 64px; }
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, monospace, sans-serif; -webkit-tap-highlight-color: transparent; }
        body { background: var(--bg-color); color: var(--text-main); padding: 14px 14px calc(var(--nav-height) + 20px) 14px; min-height: 100vh; transition: background 0.3s, color 0.3s; }

        .header { display: flex; flex-direction: column; gap: 8px; padding-bottom: 12px; border-bottom: 1px solid var(--border-color); margin-bottom: 14px; }
        .header-top { display: flex; justify-content: space-between; align-items: center; }
        .header h1 { font-size: 1.1rem; font-weight: 700; letter-spacing: 1.5px; color: var(--text-main); text-shadow: var(--text-glow); }
        
        .status-bar { display: flex; align-items: center; gap: 6px; flex-wrap: wrap; }
        .status-badge { display: inline-flex; align-items: center; gap: 5px; padding: 4px 9px; border-radius: 8px; background: var(--card-inner); border: 1px solid var(--border-color); font-size: 0.72rem; color: var(--text-sub); font-weight: 500; }
        .status-badge.online { color: var(--accent-green); border-color: rgba(65, 184, 131, 0.3); }
        .status-badge.gps { color: var(--accent-blue); }
        .status-pill { display: inline-flex; align-items: center; padding: 4px 10px; border-radius: 8px; background: rgba(231, 76, 60, 0.15); color: var(--accent-red); font-size: 0.72rem; font-weight: 700; border: 1px solid var(--border-color); margin-left: auto; }

        .tab-content { display: none; flex-direction: column; gap: 14px; }
        .tab-content.active { display: flex; }

        .section-title { font-size: 0.72rem; text-transform: uppercase; letter-spacing: 1.2px; color: var(--text-sub); margin-top: 4px; }

        .car-hero-card {
            position: relative;
            background: var(--card-bg);
            border: 1px solid var(--border-color);
            border-radius: 16px;
            overflow: hidden;
            box-shadow: 0 8px 24px rgba(0,0,0,0.4);
            padding: 10px;
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .car-stage {
            position: relative;
            width: 100%;
            max-width: 600px;
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .car-stage svg {
            width: 100%;
            height: auto;
            max-height: 45vh;
            overflow: visible;
            filter: drop-shadow(0 20px 20px rgba(0,0,0,0.8));
        }

        .security-ring {
            fill: rgba(65, 184, 131, 0.02);
            stroke: var(--accent-green);
            stroke-width: 2;
            stroke-dasharray: 6 12;
            filter: drop-shadow(0 0 10px rgba(65, 184, 131, 0.4));
            transition: all 0.5s cubic-bezier(0.4, 0, 0.2, 1);
            transform-origin: center;
        }

        @keyframes ringPulse {
            0% { transform: scale(1); stroke-width: 2; }
            50% { transform: scale(1.02); stroke-width: 3; }
            100% { transform: scale(1); stroke-width: 2; }
        }
        .car-stage.armed .security-ring { animation: ringPulse 4s infinite alternate; }

        .car-stage.disarmed .security-ring {
            fill: transparent;
            stroke: var(--accent-red);
            stroke-dasharray: 4 20;
            filter: none;
            transform: scale(1.05);
            animation: none;
        }

        .smoke-particle { fill: var(--text-sub); opacity: 0; filter: blur(6px); }
        .car-stage.engine-running .smoke-particle { animation: smokeRise 2s infinite ease-out; }
        .car-stage.engine-running .smoke-1 { animation-delay: 0s; }
        .car-stage.engine-running .smoke-2 { animation-delay: 0.6s; }
        .car-stage.engine-running .smoke-3 { animation-delay: 1.2s; }

        @keyframes smokeRise {
            0% { transform: translate(540px, 100px) scale(0.3); opacity: 0; }
            20% { opacity: 0.6; }
            100% { transform: translate(620px, 60px) scale(3.5); opacity: 0; }
        }

        .car-stage.engine-running .car-body-group { animation: engineVibe 0.04s infinite alternate; }
        @keyframes engineVibe { from { transform: translateX(0); } to { transform: translateX(0.5px); } }

        .beam { opacity: 0; transition: opacity 0.4s; }
        .car-stage.lights-active .beam { opacity: 1; }
        .headlight-lens { fill: #e2e8f0; transition: fill 0.3s; }
        .car-stage.lights-active .headlight-lens { fill: #fff; filter: drop-shadow(0 0 8px #fff); }

        .turn-signal { opacity: 0.2; fill: var(--accent-amber); transition: opacity 0.15s; }

        .car-stage.blinking-hazards-3 .turn-signal {
            animation: threeBlinks 1.5s ease-in-out forwards;
        }
        @keyframes threeBlinks {
            0%, 100% { opacity: 0.2; filter: none; }
            16%, 50%, 84% { opacity: 1; filter: drop-shadow(0 0 12px var(--accent-amber)); }
            33%, 66% { opacity: 0.2; filter: none; }
        }

        .car-stage.blinking-hazards-2 .turn-signal {
            animation: twoBlinks 0.8s ease-in-out forwards;
        }
        @keyframes twoBlinks {
            0%, 100% { opacity: 0.2; filter: none; }
            25%, 75% { opacity: 1; filter: drop-shadow(0 0 12px var(--accent-amber)); }
            50% { opacity: 0.2; filter: none; }
        }

        .status-actions-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 10px; }
        .action-card {
            background: var(--card-bg);
            border: 1px solid var(--border-color);
            border-radius: 14px;
            padding: 14px;
            display: flex;
            align-items: center;
            gap: 12px;
            cursor: pointer;
            transition: all 0.2s;
            user-select: none;
        }
        .action-card:active { transform: scale(0.97); }
        .action-icon-wrap {
            width: 42px; height: 42px;
            border-radius: 10px;
            background: var(--card-inner);
            border: 1px solid var(--border-color);
            display: flex; align-items: center; justify-content: center;
            font-size: 1.2rem;
            flex-shrink: 0;
        }
        .action-info { display: flex; flex-direction: column; gap: 2px; }
        .action-title { font-size: 0.72rem; color: var(--text-sub); text-transform: uppercase; font-weight: 600; }
        .action-state { font-size: 0.95rem; font-weight: 700; color: var(--text-main); }

        .action-card.engine-active { border-color: rgba(65, 184, 131, 0.4); background: rgba(65, 184, 131, 0.08); }
        .action-card.engine-active .action-icon-wrap { color: var(--accent-green); border-color: rgba(65, 184, 131, 0.3); }
        .action-card.engine-active .action-state { color: var(--accent-green); }

        .action-card.armed-state { border-color: rgba(65, 184, 131, 0.4); background: rgba(65, 184, 131, 0.08); }
        .action-card.armed-state .action-icon-wrap { color: var(--accent-green); border-color: rgba(65, 184, 131, 0.3); }
        .action-card.armed-state .action-state { color: var(--accent-green); }

        .action-card.disarmed-state { border-color: rgba(231, 76, 60, 0.4); background: rgba(231, 76, 60, 0.08); }
        .action-card.disarmed-state .action-icon-wrap { color: var(--accent-red); border-color: rgba(231, 76, 60, 0.3); }
        .action-card.disarmed-state .action-state { color: var(--accent-red); }

        .metrics-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 10px; }
        .metric-card { background: var(--card-bg); border: 1px solid var(--border-color); border-radius: 14px; padding: 14px; }
        .metric-label { font-size: 0.72rem; color: var(--text-sub); text-transform: uppercase; }
        .metric-val { font-size: 1.35rem; font-weight: 700; color: var(--text-main); margin-top: 4px; text-shadow: var(--text-glow); }
        .metric-unit { font-size: 0.8rem; color: var(--text-sub); font-weight: 400; }

        .form-group { background: var(--card-bg); border: 1px solid var(--border-color); border-radius: 14px; padding: 14px; display: flex; flex-direction: column; gap: 12px; }
        .form-row { display: flex; justify-content: space-between; align-items: center; }
        .form-label { font-size: 0.85rem; color: var(--text-main); font-weight: 500; }
        .form-sublabel { font-size: 0.72rem; color: var(--text-sub); }
        .form-val-badge { font-family: monospace; font-size: 0.82rem; color: var(--accent-blue); background: var(--card-inner); padding: 2px 8px; border-radius: 6px; border: 1px solid var(--border-color); }
        input[type="range"] { width: 100%; accent-color: var(--accent-blue); margin-top: 4px; }
        select { background: var(--card-inner); border: 1px solid var(--border-color); color: var(--text-main); padding: 8px 12px; border-radius: 8px; font-size: 0.8rem; outline: none; }

        .theme-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 8px; margin-top: 4px; }
        .theme-btn { background: var(--card-inner); border: 1px solid var(--border-color); padding: 10px 6px; border-radius: 10px; text-align: center; font-size: 0.75rem; color: var(--text-sub); cursor: pointer; transition: all 0.2s; font-weight: 600; }
        .theme-btn.active { border-color: var(--accent-blue); color: var(--accent-blue); background: rgba(74, 144, 226, 0.1); text-shadow: var(--text-glow); }

        .switch { position: relative; display: inline-block; width: 46px; height: 24px; flex-shrink: 0; }
        .switch input { opacity: 0; width: 0; height: 0; }
        .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: var(--card-inner); border: 1px solid var(--border-color); transition: .3s; border-radius: 24px; }
        .slider:before { position: absolute; content: ""; height: 16px; width: 16px; left: 3px; bottom: 3px; background-color: var(--text-sub); transition: .3s; border-radius: 50%; }
        input:checked + .slider { background-color: rgba(74, 144, 226, 0.25); border-color: var(--accent-blue); }
        input:checked + .slider:before { transform: translateX(22px); background-color: var(--accent-blue); }

        .btn-save { width: 100%; padding: 15px; border-radius: 12px; border: none; background: var(--accent-blue); color: #000; font-size: 0.95rem; font-weight: 700; cursor: pointer; transition: opacity 0.2s; }
        .btn-save:active { opacity: 0.85; }

        .log-toolbar { display: flex; justify-content: space-between; gap: 8px; }
        .log-search { flex: 1; background: var(--card-inner); border: 1px solid var(--border-color); padding: 8px 12px; border-radius: 8px; color: var(--text-main); font-size: 0.82rem; }
        .btn-clear-log { background: var(--card-bg); border: 1px solid var(--border-color); color: var(--accent-red); padding: 8px 12px; border-radius: 8px; font-size: 0.78rem; font-weight: 600; cursor: pointer; }
        
        .log-container { background: var(--card-inner); border: 1px solid var(--border-color); border-radius: 14px; padding: 10px; max-height: 420px; overflow-y: auto; display: flex; flex-direction: column; gap: 6px; font-family: monospace; }
        .log-item { font-size: 0.75rem; padding: 6px 8px; border-radius: 6px; background: rgba(255,255,255,0.02); display: flex; gap: 8px; align-items: flex-start; border-left: 3px solid var(--border-color); }
        .log-item.info { border-left-color: var(--accent-blue); }
        .log-item.warn { border-left-color: var(--accent-amber); }
        .log-item.alarm { border-left-color: var(--accent-red); background: rgba(231, 76, 60, 0.08); }
        .log-time { color: var(--text-sub); white-space: nowrap; }
        .log-msg { color: var(--text-main); word-break: break-all; }

        .nav-bar { position: fixed; bottom: 0; left: 0; right: 0; height: var(--nav-height); background: var(--card-bg); border-top: 1px solid var(--border-color); display: flex; justify-content: space-around; align-items: center; z-index: 100; backdrop-filter: blur(10px); }
        .nav-item { display: flex; flex-direction: column; align-items: center; gap: 4px; color: var(--text-sub); font-size: 0.7rem; font-weight: 500; cursor: pointer; width: 33.3%; padding: 8px 0; transition: color 0.2s; }
        .nav-item .nav-icon { font-size: 1.3rem; }
        .nav-item.active { color: var(--accent-blue); font-weight: 700; text-shadow: var(--text-glow); }
    </style>
</head>
<body>

    <div class="header">
        <div class="header-top">
            <h1>VOLVO 850 TELEMATICS</h1>
            <div class="status-pill" id="globalStatusPill">CONNECTING...</div>
        </div>
        <div class="status-bar">
            <div class="status-badge online" id="securityBadge">
                <span id="securityIcon">🔒</span>
                <span id="securityText">В охране</span>
            </div>
            <div class="status-badge" id="statusBadge">
                <div style="width: 6px; height: 6px; border-radius: 50%; background: currentColor;"></div>
                <span id="statusText">ДВС заглушен</span>
            </div>
            <div class="status-badge gps">📍 BLE Smart</div>
        </div>
    </div>

    <!-- ТАБ 1: СТАТУС -->
    <div id="tab-status" class="tab-content active">
        <div class="car-hero-card">
            <div class="car-stage armed" id="carModel">
                <svg viewBox="0 0 650 360">
                    <defs>
                        <linearGradient id="paintGrad" x1="0%" y1="0%" x2="0%" y2="100%">
                            <stop offset="0%" stop-color="#1e232d" />
                            <stop offset="20%" stop-color="#2a3240" />
                            <stop offset="50%" stop-color="#3b465a" />
                            <stop offset="80%" stop-color="#2a3240" />
                            <stop offset="100%" stop-color="#1e232d" />
                        </linearGradient>

                        <linearGradient id="glassGrad" x1="0%" y1="0%" x2="100%" y2="100%">
                            <stop offset="0%" stop-color="#05070a" />
                            <stop offset="45%" stop-color="#0a0e15" />
                            <stop offset="50%" stop-color="#1f2c42" />
                            <stop offset="55%" stop-color="#0a0e15" />
                            <stop offset="100%" stop-color="#05070a" />
                        </linearGradient>

                        <linearGradient id="lightBeam" x1="100%" y1="0%" x2="0%" y2="0%">
                            <stop offset="0%" stop-color="rgba(255, 255, 255, 0.85)" />
                            <stop offset="40%" stop-color="rgba(255, 255, 255, 0.15)" />
                            <stop offset="100%" stop-color="rgba(255, 255, 255, 0)" />
                        </linearGradient>
                    </defs>

                    <rect x="90" y="75" width="490" height="210" rx="20" fill="rgba(0,0,0,0.6)" filter="blur(15px)" />
                    <rect x="25" y="25" width="600" height="310" rx="155" class="security-ring" />

                    <path class="beam" d="M 86,90 L -100,10 L -100,210 L 86,130 Z" fill="url(#lightBeam)" />
                    <path class="beam" d="M 86,230 L -100,150 L -100,350 L 86,270 Z" fill="url(#lightBeam)" />

                    <g class="car-body-group">
                        <rect x="100" y="90" width="450" height="180" rx="8" fill="#0d1117" />
                        <rect x="85" y="85" width="20" height="190" rx="4" fill="#181818" stroke="#222" stroke-width="1" />
                        <rect x="545" y="85" width="20" height="190" rx="4" fill="#181818" stroke="#222" stroke-width="1" />

                        <path d="M 100,90 L 100,270 L 200,270 L 200,90 Z" fill="url(#paintGrad)" stroke="#2a3240" stroke-width="1" />
                        <polygon points="200,90 200,270 240,255 240,105" fill="url(#glassGrad)" stroke="#1a2235" stroke-width="1" />
                        
                        <path d="M 460,90 L 460,270 L 550,270 L 550,90 Z" fill="url(#paintGrad)" stroke="#2a3240" stroke-width="1" />
                        <polygon points="460,90 460,270 420,255 420,105" fill="url(#glassGrad)" stroke="#1a2235" stroke-width="1" />

                        <rect x="240" y="105" width="180" height="150" fill="url(#paintGrad)" stroke="#2a3240" stroke-width="1" />
                        <rect x="270" y="135" width="50" height="90" rx="4" fill="url(#glassGrad)" stroke="#1a2235" stroke-width="1" />

                        <g class="door door-fl">
                            <rect x="240" y="90" width="100" height="25" fill="url(#paintGrad)" stroke="#2a3240" stroke-width="1" />
                            <rect x="245" y="100" width="90" height="15" fill="url(#glassGrad)" stroke="#1a2235" stroke-width="1" />
                        </g>

                        <g class="door door-fr">
                            <rect x="240" y="245" width="100" height="25" fill="url(#paintGrad)" stroke="#2a3240" stroke-width="1" />
                            <rect x="245" y="245" width="90" height="15" fill="url(#glassGrad)" stroke="#1a2235" stroke-width="1" />
                        </g>

                        <g class="door door-rl">
                            <rect x="340" y="90" width="80" height="25" fill="url(#paintGrad)" stroke="#2a3240" stroke-width="1" />
                            <rect x="340" y="100" width="75" height="15" fill="url(#glassGrad)" stroke="#1a2235" stroke-width="1" />
                        </g>

                        <g class="door door-rr">
                            <rect x="340" y="245" width="80" height="25" fill="url(#paintGrad)" stroke="#2a3240" stroke-width="1" />
                            <rect x="340" y="245" width="75" height="15" fill="url(#glassGrad)" stroke="#1a2235" stroke-width="1" />
                        </g>

                        <rect class="headlight-lens" x="86" y="90" width="12" height="40" rx="2" />
                        <rect class="headlight-lens" x="86" y="230" width="12" height="40" rx="2" />
                        <rect x="535" y="90" width="18" height="35" rx="2" fill="#dc2626" />
                        <rect x="535" y="235" width="18" height="35" rx="2" fill="#dc2626" />

                        <rect x="87" y="85" width="14" height="8" rx="1" class="turn-signal" />
                        <rect x="87" y="267" width="14" height="8" rx="1" class="turn-signal" />
                        <rect x="537" y="85" width="14" height="12" rx="1" class="turn-signal" />
                        <rect x="537" y="263" width="14" height="12" rx="1" class="turn-signal" />
                    </g>

                    <g id="exhaust-system">
                        <circle cx="0" cy="0" r="10" class="smoke-particle smoke-1" />
                        <circle cx="0" cy="0" r="12" class="smoke-particle smoke-2" />
                        <circle cx="0" cy="0" r="15" class="smoke-particle smoke-3" />
                    </g>
                </svg>
            </div>
        </div>

        <div class="section-title">Быстрое управление</div>
        <div class="status-actions-grid">
            <div class="action-card" id="cardEngine" onclick="toggleEngine()">
                <div class="action-icon-wrap">⚡</div>
                <div class="action-info">
                    <span class="action-title">Запуск ДВС</span>
                    <span class="action-state" id="cardEngineState">Заглушен</span>
                </div>
            </div>
            <div class="action-card armed-state" id="cardSecurity" onclick="toggleSecurity()">
                <div class="action-icon-wrap" id="cardSecIcon">🔒</div>
                <div class="action-info">
                    <span class="action-title">Режим охраны</span>
                    <span class="action-state" id="cardSecState">В охране</span>
                </div>
            </div>
        </div>

        <div class="section-title">Параметры B5254</div>
        <div class="metrics-grid">
            <div class="metric-card">
                <div class="metric-label">Аккумулятор</div>
                <div class="metric-val" id="vBatVal">12.6 <span class="metric-unit">В</span></div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Обороты ДВС</div>
                <div class="metric-val" id="rpmValue">0 <span class="metric-unit">RPM</span></div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Температура ОЖ</div>
                <div class="metric-val" id="tCoolantVal">87 <span class="metric-unit">°C</span></div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Температура в салоне</div>
                <div class="metric-val" id="tCabinVal">+21 <span class="metric-unit">°C</span></div>
            </div>
        </div>
    </div>

    <!-- ТАБ 2: НАСТРОЙКИ -->
    <div id="tab-settings" class="tab-content">
        <div class="section-title">Визуальное оформление</div>
        <div class="form-group">
            <div class="form-label">Цветовая схема панели</div>
            <div class="theme-grid">
                <div class="theme-btn" onclick="setTheme('modern', event)">Modern Dark</div>
                <div class="theme-btn active" onclick="setTheme('green', event)">VDO Green</div>
                <div class="theme-btn" onclick="setTheme('amber', event)">Volvo Amber</div>
            </div>
        </div>

        <div class="section-title">Автозапирание ЦЗ (Speed Lock)</div>
        <div class="form-group">
            <div class="form-row">
                <div>
                    <div class="form-label">Запирание в движении</div>
                    <div class="form-sublabel">Автозакрытие дверей при начале поездки</div>
                </div>
                <label class="switch">
                    <input type="checkbox" id="autoLockDriveToggle" checked>
                    <span class="slider"></span>
                </label>
            </div>
            <div class="form-row">
                <span class="form-label">Условие запирания</span>
                <select id="autoLockCondition">
                    <option value="speed" selected>По скорости (>15 км/ч)</option>
                    <option value="rpm">По оборотам (>1000 RPM)</option>
                    <option value="gear">Перевод АКПП из положения «P»</option>
                </select>
            </div>
            
            <div class="form-row" style="margin-top: 4px; padding-top: 8px; border-top: 1px solid rgba(255,255,255,0.05);">
                <div>
                    <div class="form-label">Отпирание при глушении</div>
                    <div class="form-sublabel">Разблокировать двери при выключении зажигания</div>
                </div>
                <label class="switch">
                    <input type="checkbox" id="autoUnlockStopToggle" checked>
                    <span class="slider"></span>
                </label>
            </div>
            <div class="form-row">
                <span class="form-label">Условие отпирания</span>
                <select id="autoUnlockCondition">
                    <option value="ign_off" selected>При выключении зажигания / ДВС</option>
                    <option value="gear_p">При переводе АКПП в положение «P»</option>
                </select>
            </div>
        </div>

        <div class="section-title">Свободные руки (Hands-Free BLE)</div>
        <div class="form-group">
            <div class="form-row">
                <div>
                    <div class="form-label">Функция «Свободные руки»</div>
                    <div class="form-sublabel">Снятие/постановка по BLE метке</div>
                </div>
                <label class="switch">
                    <input type="checkbox" checked>
                    <span class="slider"></span>
                </label>
            </div>
            <div class="form-row">
                <span class="form-label">Дальность обнаружения</span>
                <span class="form-val-badge" id="hfRangeVal">5 м (RSSI -65)</span>
            </div>
            <input type="range" min="1" max="10" value="5" oninput="document.getElementById('hfRangeVal').innerText = this.value + ' м (RSSI ' + (-85 + this.value*4) + ')'">
            
            <div class="form-row">
                <span class="form-label">Задержка постановки при уходе</span>
                <select>
                    <option>Немедленно</option>
                    <option selected>5 секунд</option>
                    <option>15 секунд</option>
                </select>
            </div>
        </div>

        <div class="section-title">Вежливый свет (Coming / Leaving Home)</div>
        <div class="form-group">
            <div class="form-row">
                <div>
                    <div class="form-label">Вежливая подсветка</div>
                    <div class="form-sublabel">Включение фар/габаритов при открытии/закрытии</div>
                </div>
                <label class="switch">
                    <input type="checkbox" checked>
                    <span class="slider"></span>
                </label>
            </div>
            <div class="form-row">
                <span class="form-label">Время свечения фар</span>
                <span class="form-val-badge" id="politeTimeVal">30 сек</span>
            </div>
            <input type="range" min="10" max="90" step="5" value="30" oninput="document.getElementById('politeTimeVal').innerText = this.value + ' сек'">
            
            <div class="form-row">
                <span class="form-label">Срабатывание только в темноте</span>
                <label class="switch">
                    <input type="checkbox" checked>
                    <span class="slider"></span>
                </label>
            </div>
        </div>

        <div class="section-title">Параметры автозапуска ДВС</div>
        <div class="form-group">
            <div class="form-row">
                <span class="form-label">Время работы прогрева</span>
                <select>
                    <option>10 минут</option>
                    <option selected>15 минут</option>
                    <option>20 минут</option>
                    <option>30 минут</option>
                </select>
            </div>
            <div class="form-row">
                <div>
                    <div class="form-label">Автозапуск по температуре ОЖ</div>
                    <div class="form-sublabel">Запуск при падении ниже порога</div>
                </div>
                <label class="switch">
                    <input type="checkbox" checked>
                    <span class="slider"></span>
                </label>
            </div>
            <div class="form-row">
                <span class="form-label">Порог температуры</span>
                <span class="form-val-badge" id="tempStartVal">-18 °C</span>
            </div>
            <input type="range" min="-30" max="-5" value="-18" oninput="document.getElementById('tempStartVal').innerText = this.value + ' °C'">

            <div class="form-row">
                <div>
                    <div class="form-label">Автозапуск по падению АКБ</div>
                    <div class="form-sublabel">Подзарядка при просадке батареи</div>
                </div>
                <label class="switch">
                    <input type="checkbox" checked>
                    <span class="slider"></span>
                </label>
            </div>
            <div class="form-row">
                <span class="form-label">Порог напряжения АКБ</span>
                <span class="form-val-badge" id="voltStartVal">11.8 В</span>
            </div>
            <input type="range" min="11.4" max="12.2" step="0.1" value="11.8" oninput="document.getElementById('voltStartVal').innerText = Number(this.value).toFixed(1) + ' В'">
        </div>

        <button class="btn-save" onclick="saveSettings()">Сохранить конфигурацию</button>
    </div>

    <!-- ТАБ 3: ЖУРНАЛ -->
    <div id="tab-logs" class="tab-content">
        <div class="log-toolbar">
            <input type="text" class="log-search" placeholder="Поиск по логам..." id="logSearchInput" oninput="filterLogs()">
            <button class="btn-clear-log" onclick="clearLogs()">Очистить</button>
        </div>
        <div class="log-container" id="logContainer">
            <div class="log-item info">
                <span class="log-time">[Init]</span>
                <span class="log-msg">Ожидание подключения к WebSocket...</span>
            </div>
        </div>
    </div>

    <nav class="nav-bar">
        <div class="nav-item active" onclick="switchTab('status', this)">
            <span class="nav-icon">🚗</span>
            <span>Статус</span>
        </div>
        <div class="nav-item" onclick="switchTab('settings', this)">
            <span class="nav-icon">⚙️</span>
            <span>Настройки</span>
        </div>
        <div class="nav-item" onclick="switchTab('logs', this)">
            <span class="nav-icon">📋</span>
            <span>Журнал</span>
        </div>
    </nav>

    <script>
        let ws;
        const car = document.getElementById('carModel');
        const statusBadge = document.getElementById('statusBadge');
        const statusText = document.getElementById('statusText');
        const secBadge = document.getElementById('securityBadge');
        const secIcon = document.getElementById('securityIcon');
        const secText = document.getElementById('securityText');

        const cardEngine = document.getElementById('cardEngine');
        const cardEngineState = document.getElementById('cardEngineState');
        const cardSecurity = document.getElementById('cardSecurity');
        const cardSecState = document.getElementById('cardSecState');
        const cardSecIcon = document.getElementById('cardSecIcon');

        const rpmValue = document.getElementById('rpmValue');
        const vBatVal = document.getElementById('vBatVal');
        const tCoolantVal = document.getElementById('tCoolantVal');
        const tCabinVal = document.getElementById('tCabinVal');
        const globalStatusPill = document.getElementById('globalStatusPill');

        function connectWebSocket() {
            const host = window.location.hostname || "192.168.4.1";
            ws = new WebSocket(`ws://${host}/ws`);

            ws.onopen = () => {
                globalStatusPill.innerText = 'ONLINE';
                globalStatusPill.style.background = 'rgba(65, 184, 131, 0.15)';
                globalStatusPill.style.color = 'var(--accent-green)';
                addLog('info', 'Соединение с ESP32 установлено.');
            };

            ws.onclose = () => {
                globalStatusPill.innerText = 'OFFLINE';
                globalStatusPill.style.background = 'rgba(231, 76, 60, 0.15)';
                globalStatusPill.style.color = 'var(--accent-red)';
                addLog('warn', 'Связь потеряна. Повторное подключение через 2с...');
                setTimeout(connectWebSocket, 2000);
            };

            ws.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    if (data.event === "telemetry") {
                        syncTelemetry(data);
                    } else if (data.event === "config_saved") {
                        addLog('info', 'Конфигурация успешно записана в NVS контроллера.');
                    }
                } catch (e) {
                    console.error("Ошибка парсинга WS:", e);
                }
            };
        }

        function syncTelemetry(data) {
            if (data.engineRunning) {
                car.classList.add('engine-running', 'lights-active');
                statusBadge.classList.add('online');
                statusText.innerText = 'ДВС запущен';
                cardEngine.classList.add('engine-active');
                cardEngineState.innerText = 'Запущен';
            } else {
                car.classList.remove('engine-running', 'lights-active');
                statusBadge.classList.remove('online');
                statusText.innerText = 'ДВС заглушен';
                cardEngine.classList.remove('engine-active');
                cardEngineState.innerText = 'Заглушен';
            }

            if (data.isArmed) {
                car.classList.add('armed');
                car.classList.remove('disarmed');
                secBadge.classList.add('online');
                secBadge.style.color = '';
                secBadge.style.borderColor = '';
                secIcon.innerText = '🔒';
                secText.innerText = 'В охране';
                cardSecurity.className = 'action-card armed-state';
                cardSecIcon.innerText = '🔒';
                cardSecState.innerText = 'В охране';
            } else {
                car.classList.remove('armed');
                car.classList.add('disarmed');
                secBadge.classList.remove('online');
                secBadge.style.color = 'var(--accent-red)';
                secBadge.style.borderColor = 'rgba(255, 77, 77, 0.3)';
                secIcon.innerText = '🔓';
                secText.innerText = 'Не в охране';
                cardSecurity.className = 'action-card disarmed-state';
                cardSecIcon.innerText = '🔓';
                cardSecState.innerText = 'Не в охране';
            }

            rpmValue.innerHTML = `${data.rpm} <span class="metric-unit">RPM</span>`;
            vBatVal.innerHTML = `${Number(data.vBat).toFixed(1)} <span class="metric-unit">В</span>`;
            tCoolantVal.innerHTML = `${data.tCoolant} <span class="metric-unit">°C</span>`;
            tCabinVal.innerHTML = `+${data.tCabin} <span class="metric-unit">°C</span>`;
        }

        function toggleEngine() {
            const isCurrentlyRunning = car.classList.contains('engine-running');
            const targetState = !isCurrentlyRunning;

            if (targetState) {
                car.classList.remove('blinking-hazards-2');
                car.classList.add('blinking-hazards-3');
                setTimeout(() => car.classList.remove('blinking-hazards-3'), 1500);
            }

            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ cmd: "engine", state: targetState }));
                addLog('info', targetState ? 'Команда автозапуска отправлена.' : 'Команда глушения отправлена.');
            } else {
                addLog('warn', 'Нет связи с ESP32.');
            }
        }

        function toggleSecurity() {
            const isCurrentlyArmed = car.classList.contains('armed');
            const targetArmed = !isCurrentlyArmed;

            car.classList.remove('blinking-hazards-2');
            void car.offsetWidth;
            car.classList.add('blinking-hazards-2');
            setTimeout(() => car.classList.remove('blinking-hazards-2'), 800);

            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ cmd: "security", state: targetArmed }));
                addLog('info', targetArmed ? 'Команда постановки на охрану.' : 'Команда снятия с охраны.');
            } else {
                addLog('warn', 'Нет связи с ESP32.');
            }
        }

        function saveSettings() {
            const payload = {
                cmd: "save_config",
                autoLockDrive: document.getElementById('autoLockDriveToggle').checked,
                lockCondition: document.getElementById('autoLockCondition').value,
                autoUnlockStop: document.getElementById('autoUnlockStopToggle').checked,
                unlockCondition: document.getElementById('autoUnlockCondition').value,
                autostartTemp: parseInt(document.getElementById('tempStartVal').innerText),
                autostartVolt: parseFloat(document.getElementById('voltStartVal').innerText),
                politeLightSec: parseInt(document.getElementById('politeTimeVal').innerText)
            };

            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify(payload));
                alert('Конфигурация успешно передана на контроллер!');
            } else {
                alert('Ошибка: нет связи с ESP32.');
            }
        }

        function switchTab(tabId, el) {
            document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
            document.getElementById('tab-' + tabId).classList.add('active');
            document.querySelectorAll('.nav-item').forEach(i => i.classList.remove('active'));
            el.classList.add('active');
        }

        function setTheme(themeName, event) {
            document.documentElement.setAttribute('data-theme', themeName);
            document.querySelectorAll('.theme-btn').forEach(b => b.classList.remove('active'));
            if (event && event.currentTarget) event.currentTarget.classList.add('active');
            addLog('info', 'Цветовая схема изменена на: ' + themeName);
        }

        function addLog(type, message) {
            const container = document.getElementById('logContainer');
            const now = new Date();
            const timeStr = '[' + String(now.getHours()).padStart(2, '0') + ':' + String(now.getMinutes()).padStart(2, '0') + ']';
            const item = document.createElement('div');
            item.className = 'log-item ' + type;
            item.innerHTML = `<span class="log-time">${timeStr}</span><span class="log-msg">${message}</span>`;
            container.prepend(item);
        }

        function clearLogs() {
            document.getElementById('logContainer').innerHTML = '';
            addLog('info', 'Журнал событий очищен.');
        }

        function filterLogs() {
            const query = document.getElementById('logSearchInput').value.toLowerCase();
            document.querySelectorAll('.log-item').forEach(item => {
                const text = item.innerText.toLowerCase();
                item.style.display = text.includes(query) ? 'flex' : 'none';
            });
        }

        window.addEventListener('DOMContentLoaded', connectWebSocket);
    </script>
</body>
</html>
)rawliteral";