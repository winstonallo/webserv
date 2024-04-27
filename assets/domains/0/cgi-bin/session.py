#!/usr/bin/python3

import cgi
import os
from http import cookies
from datetime import datetime, timedelta
import json
import time
import hashlib
import sys

db_filename = 'assets/domains/0/cgi-bin/db'
session_file = 'assets/domains/0/cgi-bin/session-'

def success(username, cook):
	response = "HTTP/1.1 200 OK\r\n"
	response += "Server: Awesome SAD Server/1.0\r\n"
	response += "Content-Type: text/html\r\n"
	#print(cook.output(), file=sys.stderr)
	response += cook.output() + "\r\n"
	response += "Location: cgi-bin/session.py\r\n"
	response += "Connection: close\r\n"
	response += "\r\n"
	response += '''<!DOCTYPE html>
<html>
<head>
<title>Webserv | Session</title>
<meta charset="utf-8">
<link rel="stylesheet" href="/styles/reset.css" type="text/css">
<link rel="stylesheet" href="/styles/styl.css" type="text/css">
<link href='http://fonts.googleapis.com/css?family=Source+Sans+Pro:200,400,600,700&subset=latin,latin-ext' rel='stylesheet' type='text/css'>
</head>
<body>
<div id="header">
  <div id="header_in">
    <h1><a href="/index.html"><b>WEBSERV</b></a></h1>
    <div id="menu">
      <ul>
        <li><a href="/index.html">Home</a></li>
        <li><a href="/about.html">About</a></li>
        <li><a href="/tests.html">Server Tests</a></li>
        <li><a href="/session.html" class="active">Session</a></li>
      </ul>
    </div>
  </div>
</div>
<div id="main_part_inner">
	<div id="main_part_inner_in">
	  <h2>Login</h2>
  </div>
</div>
<div id="content_inner">
	<style>
        .container {
            max-width: 400px;
            margin: 60px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
		h2 {
            text-align: center;
            color: #e55e0d;
			font-size: 22px;
        }
        input[type="text"],
        input[type="password"],
        input[type="submit"] {
            width: 100%;
            padding: 10px;
            margin: 8px 0;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }
		.welcome-message {
            text-align: center;
            margin-top: 20px;
            font-size: 18px;
        }
        .logout-link {
            text-align: center;
            margin-top: 20px;
        }
        .logout-link a {
            color: #e55e0d;
            text-decoration: none;
        }
        .logout-link a:hover {
            text-decoration: underline;
        }
    </style>
	<form id="hiddenForm" action="session.py" method="post">
		<input type="hidden" name="delSession" value="true">
	</form>
	<div class="container">
        <h2>Welcome!</h2>
        <div class="welcome-message">
            <p>Hello, <strong>''' + username + '''</strong>! Welcome to our website.</p>
        </div>
        <div class="logout-link">
            <p>Not <strong>''' + username + '''</strong>? <a href="logout.html" id="submitlink">Logout</a>.</p>
        </div>
    </div>
</div>
<script>
	document.getElementById('submitlink').addEventListener('click', function(event) {
		event.preventDefault();
		document.getElementById('hiddenForm').submit();
	});
</script>
</body>
</html>'''
	print(response)

def	login_page_info(info, cookies):
	response = "HTTP/1.1 200 OK\r\n"
	response += "Server: Awesome SAD Server/1.0\r\n"
	response += "Content-Type: text/html\r\n"
	if cookies: 
		response += cookies.output() + "\r\n"
	response += "Location: cgi-bin/session.py\r\n"
	response += "Connection: close"
	response += "\r\n"
	response += "\r\n"
	response += '''<!DOCTYPE html>
<html>
<head>
<title>Webserv | Session</title>
<meta charset="utf-8">
<link rel="stylesheet" href="/styles/reset.css" type="text/css">
<link rel="stylesheet" href="/styles/styl.css" type="text/css">
<link href='http://fonts.googleapis.com/css?family=Source+Sans+Pro:200,400,600,700&subset=latin,latin-ext' rel='stylesheet' type='text/css'>
</head>
<body>
<div id="header">
  <div id="header_in">
    <h1><a href="/index.html"><b>WEBSERV</b></a></h1>
    <div id="menu">
      <ul>
        <li><a href="/index.html">Home</a></li>
        <li><a href="/about.html">About</a></li>
        <li><a href="/tests.html">Server Tests</a></li>
        <li><a href="/session.html" class="active">Session</a></li>
      </ul>
    </div>
  </div>
</div>
<div id="main_part_inner">
	<div id="main_part_inner_in">
	  <h2>Login</h2>
  </div>
</div>
<div id="content_inner">
	<style>
        .container {
            max-width: 400px;
            margin: 40px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
        input[type="text"],
        input[type="password"],
        input[type="submit"] {
            width: 100%;
            padding: 10px;
            margin: 8px 0;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }
        input[type="submit"] {
            background-color: #e66e0d;
            color: #fff;
            cursor: pointer;
        }
        input[type="submit"]:hover {
            background-color: #e55e0d;
        }
        .register-link {
            text-align: center;
            margin-top: 20px;
        }
		.red {
			text-align: center;
			color:#ae0000;
		}
    </style>
 	<div class="container">
        <form action="session.py" method="POST">
            <label for="username">Username</label>
            <input type="text" id="username" name="username" required>
            <label for="password">Password</label>
            <input type="password" id="password" name="password" required>
			<div class="red">''' + info + '''</div>
            <input type="submit" value="Login">
        </form>
        <div class="register-link">
            <p>Not registered yet? <a href="/session_reg.html">Register here</a>.</p>
        </div>
    </div>   
</div>
</body>
</html>'''
	print(response)

def	register_page_info(info):
	response = "HTTP/1.1 200 OK\r\n"
	response += "Server: Awesome SAD Server/1.0\r\n"
	response += "Content-Type: text/html\r\n"
	response += "Location: cgi-bin/session.py\r\n"
	response += "Connection: close\r\n"
	response += "\r\n"
	response += '''<!DOCTYPE html>
<html>
<head>
<title>Webserv | Session</title>
<meta charset="utf-8">
<link rel="stylesheet" href="/styles/reset.css" type="text/css">
<link rel="stylesheet" href="/styles/styl.css" type="text/css">
<link href='http://fonts.googleapis.com/css?family=Source+Sans+Pro:200,400,600,700&subset=latin,latin-ext' rel='stylesheet' type='text/css'>
</head>
<body>
<div id="header">
  <div id="header_in">
    <h1><a href="/index.html"><b>WEBSERV</b></a></h1>
    <div id="menu">
      <ul>
        <li><a href="/index.html">Home</a></li>
        <li><a href="/about.html">About</a></li>
        <li><a href="/tests.html">Server Tests</a></li>
        <li><a href="/session.html" class="active">Session</a></li>
      </ul>
    </div>
  </div>
</div>
<div id="main_part_inner">
	<div id="main_part_inner_in">
	  <h2>Register</h2>
  </div>
</div>
<div id="content_inner">
	<style>
        .container {
            max-width: 400px;
            margin: 40px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
        input[type="text"],
        input[type="password"],
        input[type="submit"] {
            width: 100%;
            padding: 10px;
            margin: 8px 0;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }
        input[type="submit"] {
            background-color: #e66e0d;
            color: #fff;
            cursor: pointer;
        }
        input[type="submit"]:hover {
            background-color: #e55e0d;
        }
        .register-link {
            text-align: center;
            margin-top: 20px;
        }
		.red {
			text-align: center;
			color:#ae0000;
		}
    </style>
 	<div class="container">
        <form action="session.py" method="POST">
            <label for="username">Username*</label>
            <input type="text" id="username" name="username" required>
            <label for="password">Password*</label>
            <input type="password" id="password" name="password" required>

            <label for="confirm-password">Confirm Password*</label>
            <input type="password" id="confirm-password" name="confirm-password" required>
			<div class="red">''' + info + '''</div>
            <input type="submit" value="Register">
        </form>
        <div class="login-link">
            <p>Already registered? <a href="/session.html">Login here</a>.</p>
        </div>
    </div>
</div>
</body>
</html>'''
	print(response)

def initialize_database():
	if not os.path.exists(db_filename):
		with open(db_filename, 'w') as file:
			json.dump({}, file)

def add_user(username, password):
    with open(db_filename, 'r') as file:
        data = json.load(file)
    data[username] = password
    with open(db_filename, 'w') as file:
        json.dump(data, file)

def check_username(username):
    with open(db_filename, 'r') as file:
        data = json.load(file)
        return username in data

def verify_user(username, password):
    with open(db_filename, 'r') as file:
        data = json.load(file)
        return data.get(username) == password

def create_session(username, password):
	id = hashlib.sha1(str(time.time()).encode("utf-8")).hexdigest()
	data = {'username': username, 'password': password, 'id': id}
	with open(session_file + id, 'w') as f:
		json.dump(data, f)
	return id 
	

def login():
	global user
	global passw
	global passw2
	global mycookies
	global current_session
	# print(current_session, file=sys.stderr)
	# print(user, file=sys.stderr)
	# print(passw, file=sys.stderr)
	# print(passw2, file=sys.stderr)
	# print(delSes, file=sys.stderr)
	if not passw2: #login
		if os.path.exists(db_filename):
			if current_session and not user:
				# print(current_session['username'], file=sys.stderr)
				print("got here", file=sys.stderr)
				if verify_user(current_session['username'], current_session['password']):
					success(current_session['username'], mycookies)

					return
				else:
					login_page_info("Username / Password not in the database. You have to register.")
					return
			else:
				if user and passw:
					if verify_user(user, passw):
						session_id = create_session(user, passw)
						mycookies.clear()
						mycookies['session'] = session_id
						mycookies['session']['expires'] = 22480 
						mycookies['session']['path'] = '/'
						success(user, mycookies)
						return
					else:
						login_page_info("Login Error. Username / Password wrong. Try again.", None)
						return
				else:
					login_page_info("", None)
					return
		else:
			login_page_info("Sorry. User doesn't exist. Please register.", None)
			return
	else: #register
		if passw != passw2:
			register_page_info("Error. Passwords don't match.")
			return
		if os.path.exists(db_filename):
			if check_username(user):
				register_page_info("Error. Username is allready used!")
				return
		add_user(user, passw)
		login_page_info("Succefully registered. You can loggin now", None)	
		return

form = cgi.FieldStorage()
user = form.getvalue('username')
passw = form.getvalue('password')
passw2 = form.getvalue('confirm-password')
delSes = form.getvalue('delSession')
mycookies = cookies.SimpleCookie()
env = os.environ
current_session = None
initialize_database()
# print(user, file=sys.stderr)
# print(passw, file=sys.stderr)
# print(passw2, file=sys.stderr)
# print(delSes, file=sys.stderr)

def resetCookies(cookie_name):
	mycookies['session']=''
	mycookies['session']['expires'] = (datetime.now() - timedelta(days=1)).strftime('%a, %d %b %Y %H:%M:%S GMT')

def main():
	global current_session
	global mycookies
	if 'HTTP_COOKIE' in env:
		mycookies.load(env['HTTP_COOKIE'])
		if ('session' in mycookies):
			# print(mycookies['session'], file=sys.stderr)
			sessname = 'assets/domains/0/cgi-bin/session-' + mycookies['session'].value
			if os.path.exists(sessname):
				with open(sessname, 'rb') as f:
					current_session = json.load(f)
					# print(current_seion, file=sys.stderr)
				if delSes == "true":
					delSes == "false"
					os.remove(sessname)
					current_session = None
					resetCookies('session')
					login_page_info("", mycookies)
					return
			else:
				current_session = None
				resetCookies('session')
				# login_page_info("", mycookies)
				# return
	login()
# 			login()
# 		else:
# 			login()
# 	else:
# 		login()
# else:
# 	login()

if __name__ == "__main__":
	main()
