
#! /usr/bin/python3

import cgi
import os
import sys
import datetime

form = cgi.FieldStorage()
date =	datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S") 
page = "HTTP/1.1 200 OK\r\n"
page += "Date: " + date + "\r\n"
page += "Server: Awesome SAD Server/1.0\r\n"
page += "Content-Type: text/html\r\n"
page += "Connection: close"
page += "\r\n"
page += "\r\n"
page += '''<!DOCTYPE html>
<html>
<head>
<title>Webserv | Server Tests</title>
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
        <li><a href="/tests.html" class="active">Server Tests</a></li>
        <li><a href="/session.html">Session</a></li>
        <li><a href="/contact.html">Contact</a></li>
      </ul>
    </div>
  </div>
</div>
<div id="main_part_inner">
	<div id="main_part_inner_in">
	  <h2>Server Tests</h2>
  </div>
</div>
<div>
	<h3>Click the button to get the current date:</h3>
	<form method="post" action="/cgi-bin/date.py" class="formit">
        <input type="submit" class="button_submit" value="PYTHON SCRIPT: DATE">
		<input type="text" value="'''	
page += date
page +='''"></form>
</div>
<div>
	<h3>Click the button to see ASCII Art:</h3>
	<form method="post" action="/cgi-bin/do_shell.sh" class="formit">
		<input type="submit" class="button_submit" value="SHELL SCRIPT: ASCII ART">
	</form>	
</body>
</html>'''
print(page);
