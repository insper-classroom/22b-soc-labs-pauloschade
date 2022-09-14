from flask import Flask
from flask import request, render_template
from helpers.sock import Sock

file_name = "/root/webapp/soc/soc.socket"
# hostname = "169.254.0.13"
# port = 10000

sock = Sock(file_name)
# sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
# sock.connect((file_name))


app = Flask(__name__)

@app.route('/', methods=['GET'])
def index():
    # if request.method == 'POST':
    #     if request.form.get('action1') == 'Toggle Led':
    #         #sock.sendall(bytes('Toggled'))
    #         return "Toggled"
    #     elif  request.form.get('action2') == 'Get Status':
    #         return "Status"
    #     else:
    #         pass # unknown
    # if request.method == 'GET':
    #     return render_template('index.html')
    
    return render_template("index.html")
    
@app.route('/toggle', methods=['POST'])
def toggle():
    if request.form.get('action1') == 'On':
        sock.send("on")
    elif  request.form.get('action2') == 'Off':
        sock.send("off")
    return render_template("index.html")

@app.route('/status', methods=['GET'])
def status():
    sock.send("status")
    st = sock.receive()
    if st != '0':
        btn_status = "pressed"
    else:
        btn_status = "not pressed"
    return render_template("index.html", status = btn_status)

if __name__ == '__main__':
    sock.connect()
    #app.run(host='0.0.0.0')
    app.run(host='169.254.0.13')