import sqlite3
import json
import datetime
from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse
import os

DB_FILE = "attendance.db"

def init_db():
    conn = sqlite3.connect(DB_FILE)
    c = conn.cursor()
    c.execute('''
        CREATE TABLE IF NOT EXISTS attendance_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp INTEGER NOT NULL,
            user_id TEXT NOT NULL,
            name TEXT NOT NULL,
            role TEXT NOT NULL,
            direction TEXT NOT NULL,
            status TEXT NOT NULL,
            type TEXT NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    ''')
    conn.commit()
    conn.close()

class SQLBridgeHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        # Suppress logging every request to the console to keep output clean
        pass

    def do_POST(self):
        if self.path in ['/add-log', '/webhook/scan', '/webhook']:
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            try:
                log = json.loads(post_data.decode('utf-8'))
                ts = int(log.get('timestamp', 0))
                if ts <= 0:
                    ts = int(datetime.datetime.now().timestamp())
                
                user_id = str(log.get('user_id', log.get('uid', '')))
                name = str(log.get('name', ''))
                role = str(log.get('role', ''))
                direction = str(log.get('direction', log.get('dir', '')))
                status = str(log.get('status', ''))
                log_type = str(log.get('type', log.get('method', '')))

                conn = sqlite3.connect(DB_FILE)
                c = conn.cursor()
                c.execute('''
                    INSERT INTO attendance_logs (timestamp, user_id, name, role, direction, status, type)
                    VALUES (?, ?, ?, ?, ?, ?, ?)
                ''', (ts, user_id, name, role, direction, status, log_type))
                conn.commit()
                conn.close()

                self.send_response(200)
                self.send_header('Content-Type', 'text/plain')
                self.end_headers()
                self.wfile.write(b"OK")
                print(f"[SQL BRIDGE] Punch synced via {self.path}: {name} ({role}) - {direction} at {datetime.datetime.fromtimestamp(ts)}")
            except Exception as e:
                self.send_response(400)
                self.send_header('Content-Type', 'text/plain')
                self.end_headers()
                self.wfile.write(f"Error: {str(e)}".encode('utf-8'))
                print(f"[SQL BRIDGE ERROR] Failed to record log: {e}")
        else:
            self.send_response(404)
            self.end_headers()

    def do_GET(self):
        parsed_url = urllib.parse.urlparse(self.path)
        if parsed_url.path == '/get-logs':
            query = urllib.parse.parse_qs(parsed_url.query)
            date_str = query.get('date', [None])[0]
            filter_str = query.get('filter', [None])[0]
            role_str = query.get('role', [None])[0]

            if not date_str:
                self.send_response(400)
                self.send_header('Content-Type', 'text/plain')
                self.end_headers()
                self.wfile.write(b"Missing date parameter")
                return

            try:
                # Convert DD-MM-YYYY to day start & end timestamps
                day_start = datetime.datetime.strptime(date_str, '%d-%m-%Y')
                day_end = day_start + datetime.timedelta(days=1)
                start_ts = int(day_start.timestamp())
                end_ts = int(day_end.timestamp())

                conn = sqlite3.connect(DB_FILE)
                c = conn.cursor()
                
                q = "SELECT timestamp, user_id, name, role, direction, status, type FROM attendance_logs WHERE timestamp >= ? AND timestamp < ?"
                params = [start_ts, end_ts]

                if role_str:
                    q += " AND LOWER(role) = ?"
                    params.append(role_str.lower())
                
                if filter_str:
                    q += " AND (LOWER(name) LIKE ? OR LOWER(user_id) LIKE ?)"
                    params.append(f"%{filter_str.lower()}%")
                    params.append(f"%{filter_str.lower()}%")

                q += " ORDER BY timestamp ASC"
                c.execute(q, params)
                rows = c.fetchall()
                conn.close()

                # Generate CSV matching the format expected by the frontend
                csv_out = "Time,ID,Name,Role,Direction,Status,Type\n"
                for row in rows:
                    ts, uid, name, role, direction, status, log_type = row
                    time_str = datetime.datetime.fromtimestamp(ts).strftime('%H:%M:%S')
                    csv_out += f"{time_str},{uid},{name},{role},{direction},{status},{log_type}\n"

                self.send_response(200)
                self.send_header('Content-Type', 'text/csv')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(csv_out.encode('utf-8'))
            except Exception as e:
                self.send_response(500)
                self.send_header('Content-Type', 'text/plain')
                self.end_headers()
                self.wfile.write(f"Error: {str(e)}".encode('utf-8'))
                print(f"[SQL BRIDGE ERROR] Failed to fetch logs: {e}")
        else:
            self.send_response(404)
            self.end_headers()

def run(port=5000):
    init_db()
    server_address = ('', port)
    httpd = HTTPServer(server_address, SQLBridgeHandler)
    print("==================================================================")
    print(f" [SQL BRIDGE] Attendance Sync Server Running on port {port}")
    print("==================================================================")
    print(f" SQLite Database File: {os.path.abspath(DB_FILE)}")
    print(f" Endpoints:")
    print(f"  - POST /webhook/scan : Receive webhook/log scans from ESP32")
    print(f"  - POST /add-log       : Receive logs from ESP32")
    print(f"  - GET  /get-logs      : Fetch daily historical logs")
    print("==================================================================")
    print(" Press Ctrl+C to stop.")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n[SQL BRIDGE] Server stopped.")

if __name__ == '__main__':
    run()
