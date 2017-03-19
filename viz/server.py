import SimpleHTTPServer
import SocketServer

PORT = 8000

Handler = SimpleHTTPServer.SimpleHTTPRequestHandler

httpd = SocketServer.TCPServer(("", PORT), Handler)

print "Please go to : http://localhost:"+str(PORT)+"/viz/index.html"
httpd.serve_forever()
