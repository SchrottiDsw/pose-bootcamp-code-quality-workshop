#!/usr/bin/env python3
import http.server
import socketserver

PORT = 3333

address_book_data = b''

class AddressBookRequestHandler(http.server.BaseHTTPRequestHandler):
    def __init__(self, request, client_address, server):
        super().__init__(request, client_address, server)

    def do_GET(self):
        if self.path == '/address-book':
            self.send_response(200)
            self.send_header('Content-type', 'application/text')
            self.end_headers()
            global address_book_data
            self.wfile.write(address_book_data)

    def do_POST(self):
        if self.path == '/address-book':
            self.send_response(200)
            self.send_header('Content-type', 'application/text')
            self.end_headers()
            content_length = int(self.headers['Content-Length'])
            global address_book_data
            address_book_data = self.rfile.read(content_length)

with socketserver.TCPServer(("", PORT), AddressBookRequestHandler) as httpd:
    print("Serving on port", PORT)
    httpd.serve_forever()