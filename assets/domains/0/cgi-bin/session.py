import cgi
import os
from http import cookies
import json

def authenticate(username, password):
	if os.path.exists('assets/domains/0/cgi-bin/db'):
		with open('assets/domains/0/cgi-bin/db', 'rb') as f:
			db = json.load(f)
		if db[username][0] == username and db[username][1] == password:



	
def	page_info(info):
	response = '''<!DOCTYPE html>
<html>
<head>
<title>Webserv | Session</title>
<meta charset="utf-8">
<link rel="stylesheet" href="styles/reset.css" type="text/css">
<link rel="stylesheet" href="styles/styl.css" type="text/css">
<link href='http://fonts.googleapis.com/css?family=Source+Sans+Pro:200,400,600,700&subset=latin,latin-ext' rel='stylesheet' type='text/css'>
</head>
<body>
<div id="header">
  <div id="header_in">
    <h1><a href="index.html"><b>WEBSERV</b></a></h1>
    <div id="menu">
      <ul>
        <li><a href="index.html">Home</a></li>
        <li><a href="about.html">About</a></li>
        <li><a href="tests.html">Server Tests</a></li>
        <li><a href="session.html" class="active">Session</a></li>
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
    </style>
 	<div class="container">
        <form action="login.py" method="POST">
			<label>''' + info + '''</label>
            <label for="username">Username</label>
            <input type="text" id="username" name="username" required>

            <label for="password">Password</label>
            <input type="password" id="password" name="password" required>

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

def login():
    form = cgi.FieldStorage()
	user = form.getvalue('username')
	pass = form.getvalue('password')
	if user == '' or pass == '':
		page_info('Error. Need Username and Password')
	elif  


def main():
	env = os.get_environ()
	if 'COOKIE' in env:
		cookie = cookies.SimpleCookie()
		cookie.load(env['COOKIE'])
		if ('Session' in cookie)
			;
		else

		


    # username = form.getvalue('username')
    # password = form.getvalue('password')

    # if username and password:
    #     if authenticate(username, password):
    #         print("Content-type: text/html\r\n\r\n")
    #         print("<h1>Login Successful!</h1>")
    #     else:
    #         print("Content-type: text/html\r\n\r\n")
    #         print("<h1>Login Failed. Please try again.</h1>")
    # else:
    #     print("Content-type: text/html\r\n\r\n")
    #     print("<h1>Please provide both username and password.</h1>")

if __name__ == "__main__":
    main()
