#!/usr/bin/env python3
"""
ESP32 Robot WebSocket Test Server v1.0.0
========================================
Receives telemetry, sends automated test commands
Run: python test/websocket_test_server.py
Connect: ws://0.0.0.0:8080/ws
"""

import asyncio
import websockets
import json
from datetime import datetime
import argparse

class RobotTestServer:
    def __init__(self, port=8080):
        self.port = port
        self.clients = set()
        self.test_sequence = [            {"type": "control", "action": "forward", "speed": 100},
            {"type": "control", "action": "stop"},
            {"type": "control", "action": "left", "speed": 120},
            {"type": "control", "action": "stop"},
            {"type": "control", "action": "servo", "angle": 45},
        ]
        self.seq_index = 0
        
    async def handler(self, websocket, path):
        client_addr = websocket.remote_address
        print(f"[{datetime.now()}] 🚀 Client {client_addr} connected")
        self.clients.add(websocket)
        
        try:
            async for message in websocket:
                data = json.loads(message)
                
                if data.get('type') == 'telemetry':
                    gps = data.get('gps', {})
                    print(f"📡 TELEMETRY: GPS={'✓' if gps.get('fix') else '✗'} "
                          f"State={data.get('state')} Heap={data['free_heap']}B")
                
                elif data.get('type') == 'error':
                    print(f"❌ ERROR: {data.get('message')}")
                
        except websockets.exceptions.ConnectionClosed:
            print(f"[{datetime.now()}] 💥 Client {client_addr} disconnected")
        finally:
            self.clients.discard(websocket)
    
    async def send_test_commands(self):
        """Send test sequence every 8 seconds"""
        while True:
            if self.clients:
                cmd = self.test_sequence[self.seq_index % len(self.test_sequence)]
                self.seq_index += 1
                msg = json.dumps(cmd)
                
                disconnected = set()
                for client in self.clients:
                    try:
                        await client.send(msg)
                        print(f"📤 SENT: {cmd['action']}({cmd.get('speed', cmd.get('angle', ''))})")
                    except:
                        disconnected.add(client)
                
                self.clients -= disconnected
            
            await asyncio.sleep(8)
    
    async def run(self):
        print(f"🌐 WebSocket Test Server running on ws://0.0.0.0:{self.port}/ws")
        print("📱 ESP32 should connect automatically after BLE auth")
        print("🎮 Auto-sending test commands every 8 seconds")
        print("\n🧪 Expected sequence: forward → stop → left → stop → servo")
        
        server = await websockets.serve(self.handler, "0.0.0.0", self.port)
        cmd_task = asyncio.create_task(self.send_test_commands())
        
        await asyncio.gather(server.wait_closed(), cmd_task)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="ESP32 Robot Test Server")
    parser.add_argument("--port", type=int, default=8080, help="WebSocket port")
    args = parser.parse_args()
    
    server = RobotTestServer(args.port)
    asyncio.run(server.run())
