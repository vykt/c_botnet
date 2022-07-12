from flask import Flask, request, jsonify
import socket

app = Flask(__name__)

HOST = "127.0.0.1"
PORT = 26969
API_SIZE = 256

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((HOST, PORT))

ret_dict = {}

def get_num():
    num_str = request.args.get("number")
    return str(num_str)


def check_num(num_str):
    try:
        num_int = int(num_str)
    except:
        print("check_num(): " + num_str + " failed cast check.")
        ret_dict["reason"] = "Invalid"
        return -1
    if num_int > 0 and num_int <= 65535:
        print("check_num(): " + num_str + " passed checks.")
        return 0
    else:
        print("check_num(): " + num_str + " failed range check.")
        ret_dict["reason"] = "Out of range (0 - 65535)"
        return -1


@app.route("/post", methods=["POST"])
def send_to_c():

    #Don't know if global is necessary. Don't want to find out either after
    #having it not work for 25hrs+.
    global sock

    num_str = get_num()
    #If invalid input
    if check_num(num_str) == -1:
        ret_dict["number"] = num_str
        ret_dict["in_fibonacci"] = "Error"
        return jsonify(ret_dict)
    #Else valid input
    else:
        sock.sendall(bytes(num_str, "utf-8"))
        ret = sock.recv(API_SIZE)
        ret_decoded = ret.decode("utf-8")
        ret_formatted = ret_decoded.replace('\u0000', '')
        ret_dict["number"] = num_str

        if ret_formatted == "No hosts":
            ret_dict["in_fibonacci"] = "Error"
            ret_dict["reason"] = ret_formatted
        else:
            ret_dict["in_fibonacci"] = ret_formatted
            ret_dict["reason"] = "None"
        
        return jsonify(ret_dict)


if __name__ == '__main__':
    app.run(host="127.0.0.1", port=5000, debug=False)
