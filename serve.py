import http.server
import socketserver
import webbrowser
import os

PORT = 5000
DIRECTORY = os.path.dirname(os.path.abspath(__file__))

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)
    
    def end_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Cache-Control', 'no-cache, no-store, must-revalidate')
        super().end_headers()

print(f"Starting chronosMIS 24/7 Biometric Cloud ERP on port {PORT}...")
print(f"Serving directory: {DIRECTORY}")
print(f"chronosMIS is online locally at: http://localhost:{PORT}/")

try:
    webbrowser.open(f"http://localhost:{PORT}/")
except Exception:
    pass

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped.")
